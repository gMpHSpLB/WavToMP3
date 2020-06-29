/* wave to mp3 encoding using Lame lib
   @mpathak
*/
#include <sstream>
#include "Mp3File.h"
#include "Log.h"

void Mp3File::ChangeExtensionOfFileName(char* pFileName) {
	auto fileNameLen		= strlen(pFileName);
	auto extensionNameLen	= strlen(m_FileExtension[0]);

	if (fileNameLen < FileWriter::m_MaxFileNameSize) {
		strncpy((char*)&pFileName[fileNameLen - extensionNameLen], m_FileExtension[0], extensionNameLen);
	}
}

bool Mp3File::Initialize(const char* pFileName) {
	if (!pFileName) {
		Log::pLog.LogError(__FILE__,
						   __FUNCTION__,
						   __LINE__,
						   "Bad parameter \n");
		return(false);
	}

	if (!m_pFileWriter) {
		ChangeExtensionOfFileName((char*)pFileName);
		m_pFileWriter = new FileWriter(pFileName, eFileType::Binary, eFileAccessMode::Write);
		return(m_pFileWriter->Initialize());
	}
	else {
		ChangeExtensionOfFileName((char*)pFileName);
		return(m_pFileWriter->Initialize(pFileName, eFileType::Binary, eFileAccessMode::Write));
	}
}

bool Mp3File::Reset() {
	if (m_pFileWriter) {
		return(m_pFileWriter->Reset());
	}
	return(false);
}

bool Mp3File::WriteNextMp3Block(unsigned char* pData, const unsigned int dataLength) {
	return(m_pFileWriter->WriteNextDataBlock(pData, dataLength));
}

bool Mp3File::WriteLameTagFrame(unsigned char* pData, const unsigned int dataLength) {

	if (!m_pFileWriter->WriteRemainingDataIntoFile()) {
		return(false);
	}

	if (!m_pFileWriter->MoveToBeginingOfFile()) {
		return(false);
	}

	return(m_pFileWriter->WriteNextDataBlockIntoFile(pData, dataLength));
}