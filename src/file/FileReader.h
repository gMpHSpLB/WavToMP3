/* wave to mp3 encoding using Lame lib
   @mpathak
*/
#ifndef _FILEREADER_H_
#define _FILEREADER_H_
#include "File.h"
#include "Array.h"

class FileReader :public File {
private:
	static const unsigned int m_MaxBufferSize = 3*4096;
	Array<unsigned char, m_MaxBufferSize> m_Array;  // Store Data into Array to minimize file read operation
protected:
	void CalculateFileSize() override;
	bool OpenFile() override;
	bool CloseFile() override;
public:
	FileReader(const char* fileName, 
			   eFileType fileType, 
			   eFileAccessMode accessMode = eFileAccessMode::Read):File(fileName, fileType, accessMode) {
	}

	FileReader(const FileReader& other):File(other) {
	}

	FileReader& operator =(const FileReader& other) {
		if (this != &other) {
			File::operator=(other);
		}
		return(*this);
	}

	FileReader(FileReader&&)				= delete;
	FileReader& operator =(FileReader&&)	= delete;

	bool Initialize();
	bool Initialize(const char* pFileName,
					eFileType fileType,
					eFileAccessMode accessMode,
					unsigned long int fileSize = 0) override;
	bool Reset() override;

	unsigned long int GetFileSize();
	void SkipBytes(unsigned int nBytes);
	unsigned int ReadNextBlockDataFromFile(unsigned char* pBuffer, const unsigned int dataLength);
	unsigned int ReadNextBlockData(unsigned char* pBuffer, const unsigned int dataLength);
	
	~FileReader() {
		CloseFile();
	}
};
#endif //_FILEREADER_H_

