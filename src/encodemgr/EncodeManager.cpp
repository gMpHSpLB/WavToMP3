/* wave to mp3 encoding using Lame lib
   @mpathak
*/
#include <tuple>
#include "EncodeManager.h"
#include "ThreadManager.h"
#include "WavFile.h"
#include "Mp3File.h"
#include "LameLib.h"
#include "Log.h"

constexpr const unsigned int EncodeManager::m_MaxMp3BufferSize = int((2 * EncodeManager::m_MaxChannelSamples) * 1.25 + 7200) + 1;

bool EncodeManager::GetWaveFile(FileManager* pObjFM, const char** pFileName) {
	*pFileName = pObjFM->PopWAVFileFromQueue();

	if (!*pFileName) {
		Log::pLog.LogInfo("There is no file in queue.\n");
		return(false);
	}
	return(true);
}

bool EncodeManager::ProcessFileHeader(FileManager* pObjFM,
									  StreamInfo*  pStreamInfo,
									  const char*  pFileName,
									  WavFile*     pWavFile) {
	if (!pWavFile->Initialize(pFileName, pStreamInfo)) {
		Log::pLog.LogError(__FILE__,
						   __FUNCTION__,
						   __LINE__,
						   "Failed to initialize WavFile object \n");
		return(false);
	}
	return(pWavFile->ProcessHeader());
}

void* EncodeManager::EncodeToMP3(void* args) {
	bool			doneFlag					= false;
	int				numSamplesRead				= 0;
	unsigned int	mp3DataLen					= 0;
	bool			isExit						= false;
	const char*		pFileName					= nullptr;
	FileManager*	pObjFM						= nullptr;
	StreamInfo		streamInfo;
	int				pcmDataBufferLeft[EncodeManager::m_MaxChannelSamples];
	int				pcmDataBufferRight[EncodeManager::m_MaxChannelSamples];
	unsigned char	mp3DataBuffer[EncodeManager::m_MaxMp3BufferSize];
	LameLib objLameLib;
	WavFile wavFileObj;
	Mp3File mp3FileObj;

	if (nullptr == args) {
		return((void*)0);
	}
	std::tie(pObjFM, streamInfo) = *(static_cast<std::tuple<FileManager*, StreamInfo>*>(args));

	if (nullptr == pObjFM) {
		Log::pLog.LogError(__FILE__,
						   __FUNCTION__,
						   __LINE__,
						   "Bad Parameter\n");
		return((void*)0);
	}

	memset(mp3DataBuffer, 0, EncodeManager::m_MaxMp3BufferSize);
	do {
		if (!GetWaveFile(pObjFM, &pFileName)) {
			break;
		}

		if (!ProcessFileHeader(pObjFM, &streamInfo, pFileName, &wavFileObj)) {
			continue;
		}

		if (!mp3FileObj.Initialize(pFileName)) {
			Log::pLog.LogError(__FILE__, 
							   __FUNCTION__,
							   __LINE__,
							   "Failed to initialize Mp3File object \n");
			continue;
		}

		if (!objLameLib.InitializeLameLib(streamInfo)) {
			break;
		}
		
		wavFileObj.SetBytesPerSampleToRead();

		while (!doneFlag) {
			bool ret		= false;
			wavFileObj.GetNextPCMBlockForChannels(pcmDataBufferLeft,
												  pcmDataBufferRight,
												  numSamplesRead);

			if (0 == numSamplesRead) {
				ret		   = objLameLib.FlushEncodedBuffer(mp3DataBuffer,
														   EncodeManager::m_MaxMp3BufferSize,
														   mp3DataLen);
				doneFlag = true;
			}
			else {
				ret		   = objLameLib.EncodeBuffer(pcmDataBufferLeft,
													 pcmDataBufferRight,
													 numSamplesRead,
													 mp3DataBuffer,
													 EncodeManager::m_MaxMp3BufferSize,
													 mp3DataLen);
			}

			if (!ret) {
				break;
			}

			if (!mp3FileObj.WriteNextMp3Block(mp3DataBuffer, mp3DataLen)) {
				break;
			}
		}
		if (doneFlag) {
			mp3DataLen = 0;

			if(objLameLib.GetLameTagFrame(mp3DataBuffer, 
				  						  EncodeManager::m_MaxMp3BufferSize,
										  mp3DataLen)){
				mp3FileObj.WriteLameTagFrame(mp3DataBuffer, mp3DataLen);
			}
		}
		doneFlag = false;
		mp3DataLen = 0;
		pFileName = nullptr;
		wavFileObj.Reset();
		mp3FileObj.Reset();
		objLameLib.CloseLameLib();
	} while (!isExit);
}

void EncodeManager::WavToMp3(FileManager* pObjFM, StreamInfo* pStreamObj) {
	std::tuple<FileManager*, StreamInfo> args = std::make_tuple(pObjFM, *pStreamObj);
	ThreadManager threadObj;
	threadObj.ExecuteTaskInSeperateThreads(EncodeToMP3, (void*)&args, ThreadManager::eWaitThread::join);
}