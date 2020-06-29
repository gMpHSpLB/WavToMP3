/* wave to mp3 encoding using Lame lib
   @mpathak
*/
#ifndef _WAVFILE_H_
#define _WAVFILE_H_
#include "FileReader.h"
#include "AudioFile.h"
#include "WavFormat.h"
#include "StreamInfo.h"



class WavFile :public AudioFile{
	static const unsigned int m_DefaultHeaderSize		= 44 + 10; /* Including Extended format */
	static const unsigned int m_RiffChunkSize			= 12;
	static const unsigned int m_FmtChunkSize			= 16;
	static const unsigned int m_ExtendedChunkSize		= 10;
	static const unsigned int m_MaxSamples				= 2304;
	unsigned long int		  m_TotalSamples			= 0;
	unsigned long int		  m_ConsumedSamples			= 0;
	unsigned int			  m_TotalBytesPerSample		= 0;


	sRiffChunk_t m_sRiffChnk;
	sFmtChunk_t  m_sFmtChnk;
	sDataChunk_t m_sDataChnk;

	StreamInfo* m_pStreamObj	= nullptr;
	FileReader* m_pFileReader	= nullptr;

	bool IsFileSizeValid();
	bool ProcessRiffChunk();
	bool ProcessExtendedData();
	bool ProcessFmtChunk();
	void UpdateStreamInfo();
	void TransformDataForBothChannels(unsigned char* pBuffer,
									  unsigned int   numSamples,
									  unsigned int* pLeftChannelPCMData,
									  unsigned int* pRightChannelPCMData);

public:
	WavFile() :AudioFile() {
		memset(&m_sRiffChnk, 0, sizeof(m_sRiffChnk));
		memset(&m_sFmtChnk, 0, sizeof(m_sFmtChnk));
		memset(&m_sDataChnk, 0, sizeof(m_sDataChnk));
	}

	WavFile(const WavFile& other)				= delete;
	WavFile& operator =(const WavFile& other)	= delete;
	WavFile(WavFile&&)							= delete;
	WavFile& operator =(WavFile&&)				= delete;

	~WavFile() {
		if (m_pFileReader) {
			delete m_pFileReader;
		}
	}

	bool Initialize(const char* pFileName, StreamInfo* pStreamObj);
	bool Reset();
	bool ProcessHeader();
	void SetBytesPerSampleToRead();
	void GetNextPCMBlockForChannels(int* pLeftChannelPCMData,
									int* pRightChannelPCMData,
									int& numSamplesRead);
};

#endif //_WAVFILE_H_

