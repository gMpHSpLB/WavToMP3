/* wave to mp3 encoding using Lame lib
   @mpathak
*/
#ifndef _LAMELIB_H_
#define _LAMELIB_H_

#include "lame.h"
#include "FileManager.h"
#include "StreamInfo.h"
#include "WavFile.h"

class LameLib
{
private:
	lame_t  m_plibContext				= nullptr;

public:
	LameLib()									= default;
	LameLib(const LameLib& other)				= delete;
	LameLib& operator = (const LameLib& other)	= delete;

	LameLib (LameLib&& other) {
		this->m_plibContext	= other.m_plibContext;
		other.m_plibContext	= nullptr;
	}

	LameLib& operator = (LameLib&& other) {
		if (this != &other) {
			this->CloseLameLib();
			this->m_plibContext = other.m_plibContext;
			other.m_plibContext = nullptr;
		}
		return(*this);
	}

	~LameLib() {
		CloseLameLib();
		m_plibContext = nullptr;
	}

	bool InitializeLameLib(StreamInfo& streamInfo);
	void CloseLameLib();
	static void PrintOnConsole(const char* format, va_list args);
	/*
		Other Additional Functions
		More methods can be added if Application needs them.
	*/
	void SetErrorMesgHandler();
	void SetDebugMesgHandler();
	void SetInfoMesgHandler();
	bool UpdateStreamSetting(const StreamInfo& streamInfo);
	bool EncodeBuffer(int* pcmBufferLeft,
					  int* pcmBufferRight,
					  int  numSamples,
					  unsigned char* mp3Buffer,
					  const int mp3BufferLen,
					  unsigned int& encodedDataLen);

	bool FlushEncodedBuffer(unsigned char* mp3Buffer,
							const int mp3BufferLen,
							unsigned int& encodedDataLen);

	bool GetLameTagFrame(unsigned char* frameBuffer,
						 const int   bufferLen,
						 unsigned int& lameTagLen);
};
#endif //_LAMELIB_H_

