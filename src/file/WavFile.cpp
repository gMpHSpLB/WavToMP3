/* wave to mp3 encoding using Lame lib
   @mpathak
*/
#include <sstream>
#include <exception>
#include "WavFile.h"
#include <iterator>
#include "Log.h"


bool WavFile::Initialize(const char* pFileName, StreamInfo* pStreamObj) {
	m_pStreamObj = pStreamObj;

	if (!pFileName) {
		Log::pLog.LogError(__FILE__,
						   __FUNCTION__,
						   __LINE__,
						   "Bad parameter \n");
		return(false);
	}

	if (!m_pFileReader) {
		m_pFileReader = new FileReader(pFileName, eFileType::Binary, eFileAccessMode::Read);
		return(m_pFileReader->Initialize());
	}
	else{
		return(m_pFileReader->Initialize(pFileName, eFileType::Binary, eFileAccessMode::Read));
	}
}

bool WavFile::Reset() {
	if (m_pFileReader) {
		return(m_pFileReader->Reset());
	}
	memset(&m_sRiffChnk, 0, sizeof(m_sRiffChnk));
	memset(&m_sFmtChnk, 0, sizeof(m_sFmtChnk));
	memset(&m_sDataChnk, 0, sizeof(m_sDataChnk));

	m_TotalSamples				= 0;
	m_ConsumedSamples			= 0;
	m_TotalBytesPerSample		= 0;
	return(true);
}

bool WavFile::IsFileSizeValid() {
	return(m_DefaultHeaderSize < m_pFileReader->GetFileSize());
}

void WavFile::UpdateStreamInfo() {
	if (m_pStreamObj) {
		m_pStreamObj->SetNumberOfChannels(m_sFmtChnk.m_NumChannels);
		m_pStreamObj->SetBitsPerSample(m_sFmtChnk.m_BitsPerSample);
		m_pStreamObj->SetSampleRate(m_sFmtChnk.m_SampleRate);
		m_TotalSamples = (m_sDataChnk.m_DataSize) / (m_sFmtChnk.m_NumChannels * ((m_sFmtChnk.m_BitsPerSample + 7) / 8));
		m_pStreamObj->SetNumberOfSamples(m_TotalSamples);
	}
}
bool WavFile::ProcessRiffChunk() {
	unsigned char riffBuffer[m_RiffChunkSize]; /* Never Read more than that */

	memset(riffBuffer, 0, m_RiffChunkSize);

	if (m_RiffChunkSize != m_pFileReader->ReadNextBlockData(riffBuffer, m_RiffChunkSize)) {
		std::ostringstream os;
		os << "Failed to read RIFF chunk data of length: " << m_RiffChunkSize 
					<< " of file: "<< m_pFileReader->m_pFileName << '\n';
		Log::pLog.LogError(__FILE__,
						   __FUNCTION__,
						   __LINE__,
						   os);
		return(false);
	}

	m_sRiffChnk.ParseRiffChunkData(riffBuffer);
	if (!m_sRiffChnk.ValidateRiffChunkData(m_pFileReader->GetFileSize())) {
		std::ostringstream os;
		os << "Failed to validate RIFF chunk data of file: " 
					<< m_pFileReader->m_pFileName << '\n';

		Log::pLog.LogError(__FILE__,
						   __FUNCTION__,
						   __LINE__,
						   os);
		return(false);
	}

	return(true);
}

bool WavFile::ProcessExtendedData() {

	unsigned char extendedBuffer[m_ExtendedChunkSize]; /* Never Read more than that */
	memset(extendedBuffer, 0, m_ExtendedChunkSize);

	if (((m_sFmtChnk.m_SizeOfChunk - m_FmtChunkSize) > 9)
	&&	(m_sFmtChnk.m_ExtendedWave == m_sFmtChnk.m_Format)
	) {
		if (m_ExtendedChunkSize != m_pFileReader->ReadNextBlockData(extendedBuffer, m_ExtendedChunkSize)) {
			std::ostringstream os;
			os << "Failed to read Extended chunk data of length: " << m_ExtendedChunkSize
						<< " of file: " << m_pFileReader->m_pFileName  << '\n';
			Log::pLog.LogError(__FILE__,
							   __FUNCTION__,
							   __LINE__,
							   os);
			return(false);
		}

		//ignore Initial 8 bytes
		m_sFmtChnk.m_Format = GET_UINT16((extendedBuffer + (m_ExtendedChunkSize - 2)));

		if (m_sFmtChnk.m_SizeOfChunk > (m_FmtChunkSize + m_ExtendedChunkSize)) {
			unsigned int remainingBytes = (m_sFmtChnk.m_SizeOfChunk - (m_FmtChunkSize + m_ExtendedChunkSize));
			//ignore Rest of Data
			m_pFileReader->SkipBytes(remainingBytes);
		}
	}
	return(true);
}

bool WavFile::ProcessFmtChunk() {
	unsigned char fmtBuffer[m_FmtChunkSize]; /* Never Read more than that */
	memset(fmtBuffer, 0, m_FmtChunkSize);

	if (m_FmtChunkSize != m_pFileReader->ReadNextBlockData(fmtBuffer, m_FmtChunkSize)) {
		std::ostringstream os;
		os << "Failed to read fmt chunk data of length: " << m_FmtChunkSize
					<< " of file: " << m_pFileReader->m_pFileName << '\n';
		Log::pLog.LogError(__FILE__,
						   __FUNCTION__,
						   __LINE__,
						   os);
		return(false);
	}

	m_sFmtChnk.ParseFmtChunkData(fmtBuffer);
	/*Check Extended Data */
	if (!ProcessExtendedData()) {
		return(false);
	}

	if (!m_sFmtChnk.ValidateFmtChunkData(m_pFileReader->GetFileSize())) {
		std::ostringstream os;
		os << "Failed to validate fmt chunk data of file: "
					<< m_pFileReader->m_pFileName << '\n';

		Log::pLog.LogError(__FILE__,
						   __FUNCTION__,
						   __LINE__,
						   os);
		return(false);
	}
	return(true);
}

bool WavFile::ProcessHeader() {
	bool isHeaderProcessed = false;

	if (!m_pFileReader) {
		Log::pLog.LogError(__FILE__,
						   __FUNCTION__,
						   __LINE__,
						   "FileReader object is not initialized \n");
		return(false);
	}

	unsigned char subChunkHeader[8]; /* Never Read more than that */
	memset(subChunkHeader, 0, 8);

	if(!IsFileSizeValid())
	{
		std::ostringstream os;
		os << "Not a valid wave file, Invalid size of file: "
			<< m_pFileReader->m_pFileName << '\n';

		Log::pLog.LogError(__FILE__,
						   __FUNCTION__,
						   __LINE__,
						   os);
		return(false);
	}

	if (!ProcessRiffChunk()) {
		return(false);
	}

	do {
		if (8 != m_pFileReader->ReadNextBlockData(subChunkHeader, 8)) { // Format + length = 8 bytes
			std::ostringstream os;
			os << "Failed to read sub chunk header of file: "
				<< m_pFileReader->m_pFileName << '\n';

			Log::pLog.LogError(__FILE__,
							   __FUNCTION__,
							   __LINE__,
							   os);
			break;
		}

		if (m_sFmtChnk.IsItFmtChunk(subChunkHeader)) {
			if (!ProcessFmtChunk()) {
				break;
			}
		}
		else if (m_sDataChnk.IsItDataChunk(subChunkHeader)) {
			if (!m_sDataChnk.ValidateDataChunkData(m_pFileReader->GetFileSize())) {
				std::ostringstream os;
				os << "Failed to validate data chunk data of file: "
							<< m_pFileReader->m_pFileName << '\n';

				Log::pLog.LogError(__FILE__,
								   __FUNCTION__,
								   __LINE__,
								   os);
				break;
			}
			isHeaderProcessed = true;
			break;
		}
		else { /* ignore all other Format*/
			unsigned int chunkSize = GET_UINT32((subChunkHeader + 4));
			m_pFileReader->SkipBytes(chunkSize);
		}
	} while (!isHeaderProcessed);

	if (isHeaderProcessed) {
		UpdateStreamInfo();
		return(true);
	}

	{
		std::ostringstream os;
		os << "Failed to read header of wave file: "
					<< m_pFileReader->m_pFileName << '\n';
		Log::pLog.LogError(__FILE__,
			__FUNCTION__,
			__LINE__,
			os);
	}

	return(false);
}

void WavFile::SetBytesPerSampleToRead() {
	m_TotalBytesPerSample = ((m_pStreamObj->GetNumberOfChannels()) * (m_pStreamObj->GetBitsPerSample() / 8));
}

void WavFile::TransformDataForBothChannels(unsigned char* pBuffer,
										   unsigned int   numSamples,
										   unsigned int*  pLeftChannelPCMData,
										   unsigned int*  pRightChannelPCMData)
{
	unsigned int numChannels = m_pStreamObj->GetNumberOfChannels();
	/*Only Little endianess supported */

	switch (m_pStreamObj->GetBitsPerSample()) {
	case 8:
		if (2 == numChannels) {
			for (int i = 0, j = 0; i < numSamples; i++) {
				unsigned char* ptr = pBuffer + j;
				pLeftChannelPCMData[i] = (unsigned int)(((ptr[0] ^ 0x80) << 24) | (0x7F << 16));
				pRightChannelPCMData[i] = (unsigned int)(((ptr[1] ^ 0x80) << 24) | (0x7F << 16));
				j += 2;
			}
		}
		else if (1 == numChannels) {
			memset(pRightChannelPCMData, 0, numSamples);
			for (int i = 0, j = 0; i < numSamples; i++) {
				unsigned char* ptr = pBuffer + j;
				pLeftChannelPCMData[i] = (unsigned int)(((ptr[0] ^ 0x80) << 24) | (0x7F << 16));
				j += 1;
			}
		}
		break;
	case 16:
		if (2 == numChannels) {
			for (int i = 0, j = 0; i < numSamples; i++) {
				unsigned char* ptr = pBuffer + j;
				pLeftChannelPCMData[i] = (unsigned int)((ptr[0] << 16) | (ptr[1] << 24));
				pRightChannelPCMData[i] = (unsigned int)((ptr[2] << 16) | (ptr[3] << 24));
				j += 4;
			}
		}
		else if (1 == numChannels) {
			memset(pRightChannelPCMData, 0, numSamples);
			for (int i = 0, j = 0; i < numSamples; i++) {
				unsigned char* ptr = pBuffer + j;
				pLeftChannelPCMData[i] = (unsigned int)((ptr[0] << 16) | (ptr[1] << 24));
				j += 2;
			}
		}
		break;
	case 24:
		if (2 == numChannels) {
			for (int i = 0, j = 0; i < numSamples; i++) {
				unsigned char* ptr = pBuffer + j;
				pLeftChannelPCMData[i] = GET_UINT24(ptr); ptr += 3;
				pRightChannelPCMData[i] = GET_UINT24(ptr);
				j += 6;
			}
		}
		else if (1 == numChannels) {
			memset(pRightChannelPCMData, 0, numSamples);
			for (int i = 0, j = 0; i < numSamples; i++) {
				unsigned char* ptr = pBuffer + j;
				pLeftChannelPCMData[i] = GET_UINT24(ptr); ptr += 3;
				j += 3;
			}
		}
		break;
	case 32: /// IEEE Float  not supported
		/*
		if (2 == numChannels) {
			for (int i = 0, j = 0; i < numSamples; i++) {
				unsigned char* ptr = pBuffer + j;
				pLeftChannelPCMData[i] = GET_UINT32(ptr); ptr += 4;
				pRightChannelPCMData[i] = GET_UINT32(ptr);
				j += 8;
			}
		}
		else if (1 == numChannels) {
			memset(pRightChannelPCMData, 0, numSamples);
			for (int i = 0, j = 0; i < numSamples; i++) {
				unsigned char* ptr = pBuffer + j;
				pLeftChannelPCMData[i] = GET_UINT32(ptr); ptr += 4;
				j += 4;
			}
		}
		//TODO: IEEE float Transformation
		*/
		break;
	}
}

void WavFile::GetNextPCMBlockForChannels(int* pLeftChannelPCMData, 
										 int* pRightChannelPCMData, 
										 int& numSamplesRead) {
	unsigned int pBuffer[m_MaxSamples];
	unsigned int bytesToRead		= m_TotalBytesPerSample * std::min((unsigned long int)m_pStreamObj->GetFrameSize(), (m_TotalSamples - m_ConsumedSamples));
	unsigned int actualBytesRead	= 0;
	
	memset(pBuffer, 0, m_MaxSamples);
	actualBytesRead = m_pFileReader->ReadNextBlockData((unsigned char*)pBuffer, bytesToRead);

	if (m_TotalBytesPerSample) {
		numSamplesRead = (actualBytesRead / m_TotalBytesPerSample);
	}
	else {
		actualBytesRead = 0;
	}

	if (0 != actualBytesRead) {
		TransformDataForBothChannels((unsigned char*)pBuffer, 
								     (unsigned int) numSamplesRead,
								     (unsigned int*)pLeftChannelPCMData,
								     (unsigned int*)pRightChannelPCMData);
	}

	m_ConsumedSamples += numSamplesRead;
}
