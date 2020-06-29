/* wave to mp3 encoding using Lame lib
   @mpathak
*/
#ifndef _ENCODEMGR_H_
#define _ENCODEMGR_H_
#include "FileManager.h"
#include "StreamInfo.h"
#include "WavFile.h"

class EncodeManager {
private:
	static bool GetWaveFile(FileManager* pObjFM, const char** pFileName);
	static bool ProcessFileHeader(FileManager* pObjFM,
								  StreamInfo* pStreamInfo,
								  const char* pFileName,
								  WavFile* pWavFile);
public:
	static const unsigned int m_MaxChannelSamples = 1152;
	static const unsigned int m_MaxMp3BufferSize;

	EncodeManager()											= default;
	EncodeManager(const EncodeManager&)						= default;
	EncodeManager& operator = (const EncodeManager&)		= default;
	EncodeManager(EncodeManager&&)							= default;
	EncodeManager& operator = (EncodeManager&&)				= default;

	static void* EncodeToMP3(void* args);
	void WavToMp3(FileManager* pObjFM, StreamInfo* pStreamObj);

	~EncodeManager(){
	}
};
#endif // _ENCODEMGR_H_