/* wave to mp3 encoding using Lame lib
   @mpathak
*/
#ifndef _FILEWRITER_H_
#define _FILEWRITER_H_
#include "File.h"
#include "Array.h"

class FileWriter :public File {
private:
	static const unsigned int m_MaxBufferSize = 3 * 4096;
	Array<unsigned char, m_MaxBufferSize> m_Array; // Store Data into Array to minimize file write operation
protected:
	bool OpenFile() override;
	bool CloseFile() override;
	void CalculateFileSize() override;

public:
	FileWriter(const char* fileName,
			   eFileType fileType,
			   eFileAccessMode accessMode = eFileAccessMode::Write):File(fileName, fileType, accessMode) {
	}

	FileWriter(const FileWriter& other):File(other) {
	}

	FileWriter& operator =(const FileWriter& other) {
		if (this != &other) {
			File::operator=(other);
		}
		return(*this);
	}

	FileWriter(FileWriter&&)				= delete;
	FileWriter& operator =(FileWriter&&)	= delete;

	bool Initialize();
	bool Initialize(const char* pFileName,
					eFileType fileType,
					eFileAccessMode accessMode,
					unsigned long int fileSize = 0) override;
	bool Reset() override;
	bool WriteNextDataBlockIntoFile(unsigned char* pData, const unsigned int dataLength);
	bool WriteNextDataBlock(unsigned char* pData, const unsigned int dataLength);
	bool WriteRemainingDataIntoFile();
	bool MoveToBeginingOfFile();

	~FileWriter() {
		CloseFile();
	}
};

#endif //_FILEWRITER_H_

