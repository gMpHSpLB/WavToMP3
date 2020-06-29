/* wave to mp3 encoding using Lame lib
   @mpathak
*/
#include <sstream>
#include "FileReader.h"
#include "Log.h"

bool FileReader::Initialize(){
	m_Array.Reset();
	return(OpenFile());
}

bool FileReader::Initialize(const char* pFileName,
							eFileType fileType,
							eFileAccessMode accessMode,
							unsigned long int fileSize) {
	m_Array.Reset();
	return(File::Initialize(pFileName, fileType, accessMode, fileSize));
}

bool FileReader::Reset() {
	m_Array.Reset();
	return(File::Reset());
}

bool FileReader::OpenFile() {
	if (File::OpenFile()) {
		CalculateFileSize();
		return(true);
	}
	return(false);
}

bool FileReader::CloseFile() {
	return(File::CloseFile());
}

void FileReader::CalculateFileSize() {
	if ((0 == m_FileSize) && (m_FileObj.is_open())) {
		std::streampos begin, end;

		m_FileObj.seekg(0, std::ios::beg);
		begin = m_FileObj.tellg();

		m_FileObj.seekg(0, std::ios::end);
		end = m_FileObj.tellg();

		m_FileSize = (unsigned long int) (end - begin);
		m_FileObj.seekg(0, std::ios::beg);
	}
}

unsigned long int FileReader::GetFileSize() {
	return(m_FileSize);
}

void FileReader::SkipBytes(unsigned int nBytes) {
	m_Array.SetConsumedElements(nBytes);
}

unsigned int FileReader::ReadNextBlockDataFromFile(unsigned char* pBuffer, const unsigned int dataLength) {
	unsigned int numBytesRead = 0;

	try {
		if ((!m_FileObj.is_open())
			|| (m_FileObj.peek() == EOF)
			) {
			Log::pLog.LogInfo("Failed to read Data from file, either file is not opened or EOF \n");
			numBytesRead = 0;
		}
		else {
			m_FileObj.read((char*)pBuffer, dataLength);
			numBytesRead = m_FileObj.gcount();
		}
	}
	catch (std::exception & e) {
		std::ostringstream os;
		os << "Exception while reading file : " << m_pFileName 
					<< " " << e.what() << '\n';
		Log::pLog.LogError(__FILE__,
						   __FUNCTION__,
						   __LINE__,
						   os);
		m_FileObj.close();
		numBytesRead = 0;
	}
	return(numBytesRead);
}

/*Store data into Buffer to avoid multiple file read */
unsigned int FileReader::ReadNextBlockData(unsigned char* pBuffer, const unsigned int dataLength) {
	unsigned int   dataReadFromFile				= 0;
	unsigned int   remainingBytes				= 0;
	unsigned int   dataRead						= 0;
	unsigned char* tmpPtr						= pBuffer;

	if (0 == m_Array.GetSize()) {
		m_Array.ResetCurrentPosition();
		dataReadFromFile = ReadNextBlockDataFromFile(m_Array.GetCurrentPosition(), FileReader::m_MaxBufferSize);
		m_Array.SetSize(dataReadFromFile);

		if (0 == dataReadFromFile) {
			return(0);
		}
	}

	if (m_Array.GetSize() >= dataLength) {
		memcpy(tmpPtr, m_Array.GetCurrentPosition(), dataLength);
		m_Array.SetConsumedElements(dataLength);
		dataRead = dataLength;
	}
	else {
		unsigned int size = m_Array.GetSize();
		memcpy(tmpPtr, m_Array.GetCurrentPosition(), size);
		remainingBytes = dataLength - size;
		dataRead = size;
		tmpPtr += size;
		m_Array.SetSize(0);
		m_Array.ResetCurrentPosition();

		dataReadFromFile = ReadNextBlockDataFromFile(m_Array.GetCurrentPosition(), FileReader::m_MaxBufferSize);
		if (dataReadFromFile >= remainingBytes) {
			m_Array.SetSize(dataReadFromFile);
			memcpy(tmpPtr, m_Array.GetCurrentPosition(), remainingBytes);
			m_Array.SetConsumedElements(remainingBytes);
			dataRead += remainingBytes;
		}
		else if (0 != dataReadFromFile) {
			memcpy(tmpPtr, m_Array.GetCurrentPosition(), dataReadFromFile);
			m_Array.SetSize(0);
			m_Array.ResetCurrentPosition();
			dataRead += dataReadFromFile;
		}
	}
	return(dataRead);
}

