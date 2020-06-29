/* wave to mp3 encoding using Lame lib
   @mpathak
*/
#ifndef _FILE_H_
#define _FILE_H_
#include <fstream>
#include <cstring>

enum class eFileAccessMode {
	Read,
	Write,
	Read_Write,
	Unknown
};

enum class eFileType {
	Binary,
	Text,
	Unknown
};

class File {
public:
	static const unsigned int	m_MaxFileNameSize = 1024;

private:
	bool IsBinaryFile();
	std::ios::openmode GetFileAccessMode();

protected:
	unsigned long int			m_FileSize				= 0;
	unsigned int				m_FileNameLen			= 0;
	std::fstream				m_FileObj;
	eFileAccessMode				m_AccessMode			= eFileAccessMode::Unknown;
	eFileType					m_FileType				= eFileType::Unknown;

	virtual bool OpenFile()								= 0;
	virtual bool CloseFile()							= 0;
	virtual void CalculateFileSize()					= 0;
	virtual bool Initialize(const char* pFileName,
							eFileType fileType = eFileType::Unknown,
							eFileAccessMode accessMode = eFileAccessMode::Unknown,
							unsigned long int fileSize	= 0
							)							= 0;
	virtual bool Reset()								= 0;

public:
	char						m_pFileName[m_MaxFileNameSize];

	File(const char* pFileName, 
		eFileType fileType = eFileType::Unknown,
		eFileAccessMode accessMode = eFileAccessMode::Unknown,
		unsigned long int fileSize = 0
		):m_FileType(fileType), m_AccessMode(accessMode), m_FileSize(fileSize){
		if(pFileName)  {
			unsigned int len = strlen(pFileName);
			if ((len + 1) < m_MaxFileNameSize) {
				m_FileNameLen = len + 1;
				strcpy(m_pFileName, pFileName);
			}
		}
	}

	File(const File& other) {
		if (other.m_pFileName && (other.m_FileNameLen < m_MaxFileNameSize) ) {
			m_FileNameLen	= other.m_FileNameLen;
			strcpy(m_pFileName, other.m_pFileName);
			m_FileType		= other.m_FileType;
			m_AccessMode	= other.m_AccessMode;
			m_FileSize		= 0;
		}
	}

	File& operator = (const File& other) {
		if ((this != &other) && (other.m_pFileName) && (other.m_FileNameLen < m_MaxFileNameSize)) {
			this->m_FileNameLen = other.m_FileNameLen;
			strcpy(this->m_pFileName, other.m_pFileName);
			m_FileType = other.m_FileType;
			m_AccessMode = other.m_AccessMode;
			m_FileSize = 0;
		}
		return(*this);
	}

	File(File&&)					= delete;
	File& operator =(File&&)		= delete;

	virtual ~File() {
		m_FileSize = 0;
	}
};
#endif //_FILE_H_