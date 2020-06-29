/* wave to mp3 encoding using Lame lib
   @mpathak
*/
#ifndef _MP3FILE_H_
#define _MP3FILE_H_
#include "FileWriter.h"
#include "AudioFile.h"

class Mp3File :public AudioFile{
	FileWriter*		m_pFileWriter		= nullptr;

	void ChangeExtensionOfFileName(char* pFileName);

public:
	Mp3File() :AudioFile() {

	}

	Mp3File(const char* pFileName) :Mp3File() {
		if (!pFileName) {
			ChangeExtensionOfFileName((char*)pFileName);
			m_pFileWriter = new FileWriter(pFileName, eFileType::Binary, eFileAccessMode::Write);
		}
	}

	Mp3File(const Mp3File& other)					= delete;
	Mp3File& operator =(const Mp3File& other)		= delete;
	Mp3File(Mp3File&&)								= delete;
	Mp3File& operator =(Mp3File&&)					= delete;

	~Mp3File() {
		if (m_pFileWriter) {
			delete m_pFileWriter;
		}
	}

	bool Initialize(const char* pFileName);
	bool Reset();
	bool WriteNextMp3Block(unsigned char* pData, const unsigned int dataLength);
	bool WriteLameTagFrame(unsigned char* pData, const unsigned int dataLength);
};

#endif // _MP3FILE_H_

