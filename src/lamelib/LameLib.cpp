/* wave to mp3 encoding using Lame lib
   @mpathak
*/
#include <sstream>
#include <tuple>
#include <algorithm>
#include <fstream>
#include "LameLib.h"
#include "Log.h"


bool LameLib::InitializeLameLib(StreamInfo& streamInfo) {
	bool			   ret = false;

	if (nullptr == m_plibContext) {
		m_plibContext = lame_init();

		if (NULL == m_plibContext) {
			Log::pLog.LogError(__FILE__,
							   __FUNCTION__,
							   __LINE__,
							   "Failed to Intialize Lame Library\n");
		}
		else {
			SetDebugMesgHandler();
			SetInfoMesgHandler();
			SetErrorMesgHandler();
			UpdateStreamSetting(streamInfo);
			if (-1 == lame_init_params(m_plibContext)) {
				Log::pLog.LogError(__FILE__,
								   __FUNCTION__,
								   __LINE__,
								   "Failed to Intialize stream parameters in Lame Library\n");

				lame_close( m_plibContext );
				m_plibContext = nullptr;
			}
			else {
				streamInfo.SetFrameSize(lame_get_framesize(m_plibContext));
				ret = true;
			}
		}
	}
	return(ret);
}

void LameLib::CloseLameLib() {
	if(m_plibContext){
		lame_close(m_plibContext);
		m_plibContext = nullptr;
	}
}

void LameLib::PrintOnConsole(const char* format, va_list args)
{
	(void)vfprintf(stdout, format, args);
}

void LameLib::SetDebugMesgHandler() {
	lame_set_debugf(m_plibContext, &LameLib::PrintOnConsole);
}

void LameLib::SetErrorMesgHandler() {
	lame_set_errorf(m_plibContext, &LameLib::PrintOnConsole);
}

void LameLib::SetInfoMesgHandler() {
	lame_set_msgf(m_plibContext, &LameLib::PrintOnConsole);
}

bool LameLib::UpdateStreamSetting(const StreamInfo& streamInfo) {
	int replayGainAnalysis = streamInfo.GetReplayGainAnalysis();
	if (-1 == lame_set_findReplayGain(m_plibContext, replayGainAnalysis)) {
		Log::pLog.LogError(__FILE__,
						   __FUNCTION__,
						   __LINE__,
						   "Failed to enable Replay Gain Analysis in Lame Library\n");
		return(false);
	}

	unsigned int numChannels = streamInfo.GetNumberOfChannels();
	if (-1 == lame_set_num_channels(m_plibContext, numChannels)) {
		std::ostringstream os;
		os << "Failed to set number of channels in Lame Library." <<" "
				<< "Number of channels: "<< numChannels << '\n';

		Log::pLog.LogError(__FILE__,
						   __FUNCTION__,
						   __LINE__,
						   os);
		return(false);
	}
	
	unsigned int sampleRate = streamInfo.GetSampleRate();
	if(-1 == lame_set_in_samplerate(m_plibContext, sampleRate)) {
		std::ostringstream os;
		os << "Failed to set sampleRate in Lame Library." << " "
			<< "sampleRate: " << sampleRate << '\n';

		Log::pLog.LogError(__FILE__,
						   __FUNCTION__,
						   __LINE__,
						   os);
		return(false);
	}

	unsigned long int numSamples = streamInfo.GetNumberOfSamples();
	if (-1 == lame_set_num_samples(m_plibContext, numSamples)) {
		std::ostringstream os;
		os << "Failed to set number of samples in Lame Library." << " "
					<< "Number of Samples: " << numSamples << '\n';
		Log::pLog.LogError(__FILE__,
						   __FUNCTION__,
						   __LINE__,
						   os);
		return(false);
	}

	int quality = streamInfo.GetQuality();
	if (-1 == lame_set_quality(m_plibContext, quality)) {
		std::ostringstream os;
		os << "Failed to set number quality in Lame Library." << " "
				<< "quality: " << quality << '\n';
		Log::pLog.LogError(__FILE__,
						   __FUNCTION__,
						   __LINE__,
						   os);
		return(false);
	}
}

bool LameLib::EncodeBuffer(int *pcmBufferLeft,
						  int *pcmBufferRight,
						  int  numSamples,
						  unsigned char* mp3Buffer,
						  const int mp3BufferLen,
						  unsigned int& encodedDataLen){
	int ret		 = lame_encode_buffer_int(m_plibContext,
										  pcmBufferLeft,
										  pcmBufferRight,
										  numSamples,
										  mp3Buffer,
										  mp3BufferLen);
	if (0 <= ret) {
		encodedDataLen = ret;
		return(true);
	}

	switch (ret) {
	case -1:
		Log::pLog.LogError(__FILE__,
						   __FUNCTION__,
						   __LINE__,
						   "Lame Library failed to encode, mp3 buffer was too small \n");
		break;
	case -2:
		Log::pLog.LogError(__FILE__,
						   __FUNCTION__,
						   __LINE__,
						   "Lame Library failed to encode,  malloc() problem \n");
		break;
	case -3:
		Log::pLog.LogError(__FILE__,
						   __FUNCTION__,
						   __LINE__,
						   "Lame Library failed to encode, lame_init_params() was not called \n");
		break;
	case -4:
		Log::pLog.LogError(__FILE__,
						   __FUNCTION__,
						   __LINE__,
						   "Lame Library failed to encode, psycho acoustic problems \n");
		break;
	default:
		Log::pLog.LogError(__FILE__,
						   __FUNCTION__,
						   __LINE__,
						   "Lame Library failed to encode, Unknown Error  \n");
	}
	encodedDataLen = 0;
	return(false);
}

bool LameLib::FlushEncodedBuffer(unsigned char* mp3Buffer,
								 const int mp3BufferLen,
								 unsigned int& encodedDataLen) {

	int ret = lame_encode_flush(m_plibContext,
								mp3Buffer,
								mp3BufferLen);
	if ((0 <= ret) && (mp3BufferLen >= ret)) {
		encodedDataLen = ret;
		return(true);
	}

	Log::pLog.LogError(__FILE__,
					   __FUNCTION__,
					   __LINE__,
					   "Lame Library failed to flush data, Unknown Error  \n");
	encodedDataLen = 0;
	return(false);
}

bool LameLib::GetLameTagFrame(unsigned char* frameBuffer, 
							  const int   bufferLen,
							  unsigned int&  lameTagLen){

	int ret = lame_get_lametag_frame(m_plibContext,
									 frameBuffer,
									 bufferLen);

	if ((0 <= ret) && (bufferLen >= ret)) {
		lameTagLen = ret;
		return(true);
	}

	Log::pLog.LogError(__FILE__,
					   __FUNCTION__,
					   __LINE__,
					   "Lame Library failed to get lame tag (XING/VBR), Unknown Error  \n");
	lameTagLen = 0;
	return(false);
}