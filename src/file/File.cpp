/* wave to mp3 encoding using Lame lib
   @mpathak
*/

#include <sstream>
#include "File.h"
#include "Log.h"

bool File::Initialize(const char* pFileName,
					  eFileType fileType,
					  eFileAccessMode accessMode,
					  unsigned long int fileSize){
	if (pFileName) {
		unsigned int len = strlen(pFileName);
		if ((len + 1) < m_MaxFileNameSize) {
			m_FileNameLen = len + 1;
			strcpy(m_pFileName, pFileName);

			m_FileType	 = fileType;
			m_AccessMode = accessMode;
			return(OpenFile());
		}
		else {
			std::ostringstream os;
			os << "File Name: " << pFileName << " is bigger than Max Limit: " 
						<< m_MaxFileNameSize << '\n';
			Log::pLog.LogError(__FILE__,
							   __FUNCTION__,
							   __LINE__,
								os);
			return(false);
		}
	}
	return(false);
}

bool File::Reset() {
	m_FileSize		= 0;
	m_FileNameLen	= 0;
	m_FileType		= eFileType::Unknown;
	m_AccessMode	= eFileAccessMode::Unknown;
	return(CloseFile());
}

std::ios::openmode File::GetFileAccessMode() {
	switch (m_AccessMode) {
	case eFileAccessMode::Read:
		return(std::ios::in);
	case eFileAccessMode::Write:
		return(std::ios::out);
	case eFileAccessMode::Read_Write:
		return(std::ios::out | std::ios::in);
	default:
		return(std::ios::in);
	}
}

bool File::IsBinaryFile() {
	switch (m_FileType) {
	case eFileType::Binary:
		return(true);
	case eFileType::Text:
		return(false);
	default:
		return(true);
	}
}

bool File::OpenFile() {
	bool ret = false;

	if (m_FileObj.is_open()) {
		ret = true;
	}
	else {
		try {
			std::ios::openmode	accessMode = GetFileAccessMode();
			if (IsBinaryFile()) {
				m_FileObj.open(m_pFileName, accessMode | std::ios::binary);
			}
			else {
				m_FileObj.open(m_pFileName, accessMode);
			}
			ret = true;
		}
		catch (std::exception & e) {
			std::ostringstream os;
			os<< "Exception while opening file : " << m_pFileName
					<< " " << e.what() << '\n';
			Log::pLog.LogError(__FILE__,
							   __FUNCTION__,
							   __LINE__,
							   os);
			ret = false;
		}
	}
	return(ret);
}

bool File::CloseFile() {
	bool ret = false;
	if (m_FileObj.is_open()) {
		try {
			m_FileObj.close();
			return(ret);
		}
		catch (std::exception & e) {
			std::ostringstream os;
			os << "Exception while closing file : "<< m_pFileName
						<< " " << e.what() << '\n';
			Log::pLog.LogError(__FILE__,
							   __FUNCTION__,
							   __LINE__,
							   os);
			ret = false;
		}
	}
	return(ret);
}

