/* wave to mp3 encoding using Lame lib
   @mpathak
*/
#include <sstream>
#include "FileWriter.h"
#include "Log.h"

bool FileWriter::Initialize() {
	m_Array.Reset();
	return(OpenFile());
}

bool FileWriter::Initialize(const char* pFileName,
							eFileType fileType,
							eFileAccessMode accessMode,
							unsigned long int fileSize) {
	m_Array.Reset();
	return(File::Initialize(pFileName, fileType, accessMode, fileSize));
}

bool FileWriter::Reset() {
	bool   ret		= false;

	ret = WriteRemainingDataIntoFile();
	m_Array.Reset();
	return(ret & File::Reset());
}

bool FileWriter::OpenFile() {
	return(File::OpenFile());
}

bool FileWriter::CloseFile() {
	return(File::CloseFile());
}

void FileWriter::CalculateFileSize() {
	//TODO
}

bool FileWriter::WriteNextDataBlockIntoFile(unsigned char* pData, const unsigned int dataLength) {
	bool  ret = false;

	if (!OpenFile()) {
		return(ret);
	}

	try {
		m_FileObj.write((char*)pData, dataLength);
		m_FileObj.flush();
		ret = true;
	}
	catch (std::exception & e) {
		std::ostringstream os;
		os << "Exception while writing file : " << m_pFileName << " " << e.what() << '\n';
		Log::pLog.LogError(__FILE__,
						   __FUNCTION__,
						   __LINE__,
						   os);
		ret = false;
	}
	return(ret);
}

bool FileWriter::WriteNextDataBlock(unsigned char* pData, const unsigned int dataLength) {
	unsigned int	remainingElements	= 0;
	bool			ret					= false;
	unsigned int availableSize			= m_Array.AvailableSize();

	if (availableSize >= dataLength) {
		return(m_Array.InsertElements(pData, dataLength));
	}

	if (0 != availableSize) {

		if (!m_Array.InsertElements(pData, availableSize)) {
			std::ostringstream os;
			os << "Failed to insert data of length : " << availableSize << "into Data Array" << '\n';
			Log::pLog.LogError(__FILE__,
							   __FUNCTION__,
							   __LINE__,
							   os);
			return(false);
		}
		remainingElements = dataLength - availableSize;
		pData += availableSize;
	}

	m_Array.ResetCurrentPosition();
	if (!WriteNextDataBlockIntoFile(m_Array.GetCurrentPosition(), m_Array.GetSize())) {
		return(false);
	}

	m_Array.Reset();

	if (m_MaxBufferSize < remainingElements) {
		do {
			if (!WriteNextDataBlockIntoFile(pData, m_MaxBufferSize)) {
				break;
				ret = false;
			}
			remainingElements -= m_MaxBufferSize;
			pData += m_MaxBufferSize;
		} while (remainingElements > m_MaxBufferSize);

		if (0 != remainingElements) {
			ret = m_Array.InsertElements(pData, remainingElements);
		}
		else {
			ret = true;
		}
	}
	else if (0 != remainingElements) {
		ret = m_Array.InsertElements(pData, remainingElements);
	}
	else {
		ret = true;
	}

	if (!ret) {
		Log::pLog.LogError(__FILE__,
						   __FUNCTION__,
						   __LINE__,
						   "Failed to insert data into Data Array \n");
		m_Array.Reset();
	}
	return(ret);
}

bool FileWriter::WriteRemainingDataIntoFile() {
	bool	ret				=	true;

	if (0 != m_Array.GetSize()) {
		m_Array.ResetCurrentPosition();
		ret = WriteNextDataBlockIntoFile(m_Array.GetCurrentPosition(), m_Array.GetSize());
		m_Array.SetSize(0);
	}
	return(ret);
}

bool FileWriter::MoveToBeginingOfFile() {
	if ((0 == m_FileSize) && (m_FileObj.is_open())) {
		try {
			m_FileObj.seekp(0, std::ios::beg);
			return(true);
		}
		catch (std::exception & e) {
			std::ostringstream os;
			os << "Exception while seeking file pointer of file : " 
						<< m_pFileName << " " << e.what() << '\n';
			Log::pLog.LogError(__FILE__,
							   __FUNCTION__,
							   __LINE__,
							   os);
		}
	}
	return(false);
}