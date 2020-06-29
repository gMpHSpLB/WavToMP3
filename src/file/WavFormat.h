/* wave to mp3 encoding using Lame lib
   @mpathak
*/
#ifndef _WAVFORMAT_H_
#define _WAVFORMAT_H
#include <sstream>
#include "ParseMacro.h"
#include "Log.h"

#define WAV_RIFF_STR    ("RIFF")
#define WAV_WAVE_STR    ("WAVE")
#define WAV_FMT_STR     ("fmt ")
#define WAV_DATA_STR    ("data")

#define MOVE_NEXT(p, n)  {p +=n;}

typedef struct sRiffChunk
{
    char            m_ChunkIdentifier[4];      //"RIFF" 
    unsigned int    m_SizeRestData = 0;                   
    char            m_WaveFileIdentifier[4];   //"WAVE"

    bool ParseRiffChunkData(unsigned char* ptr) {
        memcpy(&m_ChunkIdentifier, ptr, sizeof(m_ChunkIdentifier));     MOVE_NEXT(ptr, 4) 
        m_SizeRestData = GET_UINT32(ptr);                               MOVE_NEXT(ptr, 4)   
        memcpy(&m_WaveFileIdentifier, ptr, sizeof(m_ChunkIdentifier));
    }

    bool ValidateRiffChunkData(unsigned int fileSize) {
        if(strncmp(m_ChunkIdentifier, WAV_RIFF_STR, strlen(WAV_RIFF_STR)))
	    {         
            Log::pLog.LogDebug(__FILE__,
                               __FUNCTION__,
                               __LINE__,
                               "RIFF identifier is not present in file \n");
            return(false);
 	    }	

        if (m_SizeRestData != (fileSize - 8)) { 
            std::ostringstream os;
            os << "RIFF chunk has invalid Data value, not matching with size of file" <<" "
                    << "Expected size is: "<< (fileSize - 8)
                    << "Actual size in file is: "<< m_SizeRestData <<'\n';

            Log::pLog.LogDebug(__FILE__,
                               __FUNCTION__,
                               __LINE__,
                               os);
            return(false);
        }
        
        if(strncmp(m_WaveFileIdentifier, WAV_WAVE_STR, strlen(WAV_WAVE_STR)))
	    {
            Log::pLog.LogDebug(__FILE__,
                               __FUNCTION__,
                               __LINE__,
                               "WAVE identifier is not present in file \n");
            return(false);
 	    }	

        return(true);
    }
}sRiffChunk_t;

typedef struct sFmtChunk
{
    static const short int      m_PCMFormat                 = 1;
    static const short int      m_IEEEFloatingPointFormat   = 3;
    static const short int      m_ExtendedWave              = 65534; //0xFFFE
    unsigned int                m_SizeOfChunk               = 0;             
    short int                   m_Format                    = 0;       
    short int                   m_NumChannels               = 0;
    unsigned  int               m_SampleRate                = 0;
    unsigned  int               m_ByteRate                  = 0;
    short int                   m_BlockAlign                = 0;
    short int                   m_BitsPerSample             = 0;
    char                        m_ChunkIdentifier[4];             //"fmt"

    void ParseFmtChunkData(unsigned char* ptr) {
        m_Format                = GET_UINT16(ptr); MOVE_NEXT(ptr, 2)
        m_NumChannels           = GET_UINT16(ptr); MOVE_NEXT(ptr, 2) 
        m_SampleRate            = GET_UINT32(ptr); MOVE_NEXT(ptr, 4) 
        m_ByteRate              = GET_UINT32(ptr); MOVE_NEXT(ptr, 4)
        m_BlockAlign            = GET_UINT16(ptr); MOVE_NEXT(ptr, 2) 
        m_BitsPerSample         = GET_UINT16(ptr);
        {
            std::ostringstream os;
            os << "m_SizeOfChunk: " << m_SizeOfChunk << '\n' 
                    << "m_Format: " << m_Format << '\n' 
                    << "m_NumChannels: " << m_NumChannels << '\n'
                    << "m_SampleRate: " << m_SampleRate << '\n'
                    << "m_ByteRate: " << m_ByteRate << '\n'
                    << "m_BlockAlign: " << m_BlockAlign << '\n' 
                    << "m_BitsPerSample: " << m_BitsPerSample << '\n';

            Log::pLog.LogDebug(__FILE__,
                               __FUNCTION__,
                               __LINE__,
                               os);
        }
    }

    bool IsItFmtChunk(const unsigned char* ptr) {
        memcpy(&m_ChunkIdentifier, ptr, sizeof(m_ChunkIdentifier)); MOVE_NEXT(ptr, 4)
        m_SizeOfChunk = GET_UINT32(ptr);
        
        if (strncmp(m_ChunkIdentifier, WAV_FMT_STR, strlen(WAV_FMT_STR))
        || (m_SizeOfChunk < 16)
        ) {
            std::ostringstream os;
            os << "Not fmt subchunk , no identifier or invalid subchunk size in file." <<" "
                        << "subchunk size: "<< m_SizeOfChunk << '\n';

            Log::pLog.LogDebug(__FILE__,
                               __FUNCTION__,
                               __LINE__,
                               os);
            return(false);
        }
        return(true);
    }

    bool ValidateFmtChunkData(unsigned int fileSize) {
        if (((m_PCMFormat != m_Format)) /* Floating point is not supported */
        ||  (0 == m_NumChannels)
        ||  (2 < m_NumChannels)
        ||  (m_SizeOfChunk >= fileSize)
        ){
            std::ostringstream os;
            os << "Not supported format or channels in file. Or invalid subchunk size" <<" "
                    << "format: "<< m_Format <<" "
                    << "channels: "<< m_NumChannels  <<" "
                    << "subchunk size: "<< m_SizeOfChunk << " "
                    << "File Size: " << fileSize << " " << '\n';

            Log::pLog.LogDebug(__FILE__,
                               __FUNCTION__,
                               __LINE__,
                               os);
            return(false);
        }

        if ((m_BitsPerSample < 1)
        || !((m_BitsPerSample == 8) || (m_BitsPerSample == 16) || (m_BitsPerSample == 24))  // || (m_BitsPerSample == 32) is not support for now
        ) {       
            std::ostringstream os;
            os << "Not valid bits per samples in file." << " "
                    << "Bits per sample: " << m_BitsPerSample << '\n';

            Log::pLog.LogDebug(__FILE__,
                               __FUNCTION__,
                               __LINE__,
                               os);
            return(false);
        }
        return(true);
    }

}sFmtChunk_t;

typedef struct sDataChunk
{
    char            m_ChunkIdentifier[4];  //"data"
    unsigned  int   m_DataSize = 0; 

    bool IsItDataChunk(const unsigned char* ptr) {
        memcpy(&m_ChunkIdentifier, ptr, sizeof(m_ChunkIdentifier)); MOVE_NEXT(ptr, 4)
        m_DataSize = GET_UINT32(ptr);

        if (strncmp(m_ChunkIdentifier, WAV_DATA_STR, strlen(WAV_DATA_STR)))
        {            
            Log::pLog.LogDebug(__FILE__,
                               __FUNCTION__,
                               __LINE__,
                                "Not Data subchunk \n");
            return(false);
        }

        if (0 == m_DataSize) {
            Log::pLog.LogDebug(__FILE__,
                               __FUNCTION__,
                               __LINE__,
                               "Data subchunk size is zero \n");
            return(false);
        }
        return(true);
    }

    bool ValidateDataChunkData(unsigned int fileSize) {
	    if(m_DataSize > fileSize){
            std::ostringstream os;
            os << "Not valid data size, it is more than file size." << " "
                    << "Data Size: " << m_DataSize <<" "
                    << "file Size: " << fileSize << '\n';

            Log::pLog.LogDebug(__FILE__,
                               __FUNCTION__,
                               __LINE__,
                               os);
            return(false);
	    }
        return(true);
    }

}sDataChunk_t;

#endif //_WAVFORMAT_H_
