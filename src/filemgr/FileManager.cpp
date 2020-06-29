/* wave to mp3 encoding using Lame lib
   @mpathak
*/
#include <cstring>
#include <random>
#include <algorithm>
#include <iterator>
#include "FileManager.h"
#include "ThreadManager.h"
#include "Lock.h"
#include "Log.h"

#ifdef _WIN32
#include <windows.h>
#include <winerror.h>
#include <io.h>
#define ACCESS_MODE         (4)
inline int access(const char* path, int accessmode) {
	return(_access(path, accessmode));
}
#else
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#define ACCESS_MODE         (R_OK)
#endif

static pthread_mutex_t g_FileQueueMutexObj = PTHREAD_MUTEX_INITIALIZER;

bool FileManager::IsReadable(const char* path) {
	if (0 == access(path, ACCESS_MODE)) {
		return(true);
	}

    std::ostringstream os;
    os << "Not accessible file: " << path << '\n';

    Log::pLog.LogDebug(__FILE__,
                       __FUNCTION__,
                       __LINE__,
                       os);
	return(false);
}

bool FileManager::IsDirectory(const char* directoryName) {
#ifdef _WIN32
    unsigned int attribute = GetFileAttributes(directoryName);

    if ((attribute != INVALID_FILE_ATTRIBUTES)
    &&  (attribute & FILE_ATTRIBUTE_DIRECTORY)) {
        return(true);
    }
#else
    struct dirent* pDirent;
    DIR* pDir = opendir(directoryName);

    if (pDir) {
        closedir(pDir);
        return(true);
    }
#endif
    std::ostringstream os;
    os << "Not a valid directory: "
            << directoryName << '\n';

    Log::pLog.LogError(__FILE__,
                       __FUNCTION__,
                       __LINE__,
                       os);
    return(false);
}

bool FileManager::GetFilesFromDirectory(char* directoryName) {

    if (!IsDirectory(directoryName)) {
        return(false);
    }

#ifdef _WIN32
    HANDLE          handler = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA fdata;

    int dirLength = strlen(directoryName);

    if (directoryName[dirLength - 1] == '\\' || directoryName[dirLength - 1] == '/') {
        directoryName[dirLength - 1] = '\0';
    }

    std::string  strDir(directoryName);
    handler = FindFirstFile(strDir.append("\\*").c_str(), &fdata);
    if (handler != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                continue;
            }
            else
            {
                CheckWavFileAndAddIntoQueue(directoryName, fdata.cFileName);
            }
        } while (FindNextFile(handler, &fdata) != 0);
    }
    else {
        std::ostringstream os;
        os << "Failed to read directory: "
                << directoryName <<" "
                << "system Error: " << GetLastError() << '\n';

        Log::pLog.LogError(__FILE__,
                           __FUNCTION__,
                           __LINE__,
                           os);
        return(false);
    }

    if (GetLastError() != ERROR_NO_MORE_FILES)
    {
        std::ostringstream os;
        os << "Failed to read files in directory: "
                << directoryName << " "
                << "system Error: " << GetLastError() << '\n';

        Log::pLog.LogError(__FILE__,
                           __FUNCTION__,
                           __LINE__,
                           os);
        FindClose(handler);
        return(false);
    }

    FindClose(handler);
    handler = INVALID_HANDLE_VALUE;
    ShuffleWavFilesQueue(); // For fair distribution
    return(true);
#else
    bool     ret = false;
    struct dirent* pDirent;
    int dirLength = strlen(directoryName);

    if (directoryName[dirLength - 1] == '\\' || directoryName[dirLength - 1] == '/') {
        directoryName[dirLength - 1] = '\0';
    }

    DIR* pDir = opendir(directoryName);
    if (pDir) {
        while ((pDirent = readdir(pDir)) != NULL) {
            if (pDirent->d_type == DT_REG) {
                CheckWavFileAndAddIntoQueue(directoryName, pDirent->d_name); // Max File name length is 255
            }
        }
        closedir(pDir);
        ShuffleWavFilesQueue(); // For fair distribution
        return(true);
    }

    {
        std::ostringstream os;
        os << "Failed to read directory: "
            << directoryName << '\n';

        Log::pLog.LogError(__FILE__,
            __FUNCTION__,
            __LINE__,
            os);
    }
        return(false);
 #endif // _WIN32
}

bool FileManager::IsWAVFilePresent() {
    return(!m_queueOfWavFilesIndexes.empty());
}

const char* FileManager::PopWAVFileFromQueue(){
    Lock  lockObj(g_FileQueueMutexObj);
    lockObj.AcquireLock();
    if (!IsWAVFilePresent()) {
        return(nullptr);
    }

    auto index = m_queueOfWavFilesIndexes.back();
    m_queueOfWavFilesIndexes.pop_back();
    return(m_queueOfWAVFiles.at(index).c_str());
}

void FileManager::CheckWavFileAndAddIntoQueue(char* directoryName, const char* pFileName) {
    do {
        if((nullptr == pFileName) || (nullptr == directoryName)) {
            break;
        }

        unsigned int len         = strlen(pFileName);
        
        if (len < strlen(m_FileExtension[0]))
        {
            std::ostringstream os;
            os << "Invalid file name, file: "
                        << pFileName << '\n';

            Log::pLog.LogDebug(__FILE__,
                               __FUNCTION__,
                               __LINE__,
                               os);
            break;
        }
        
        if (std::all_of(&pFileName[len - 4],
            &pFileName[len - 1],
            [i = 0, this](char c) mutable { return (c == this->m_FileExtension[0][i++]); })
        ) {
            std::string absolutePath(directoryName);
#ifdef _WIN32
            absolutePath.append("\\").append(pFileName);
#else
            absolutePath.append("/").append(pFileName);
#endif

            if (!IsReadable(absolutePath.c_str())) {
                std::ostringstream os;
                os << "File is not accessible, file: "
                    << pFileName << '\n';

                Log::pLog.LogDebug(__FILE__,
                                   __FUNCTION__,
                                   __LINE__,
                                   os);
                break;
            }

            m_queueOfWAVFiles.emplace_back(absolutePath);
            m_queueOfWavFilesIndexes.emplace_back(m_numberOfFiles);
            m_numberOfFiles++;
        }
    } while (0);
}

void FileManager::ShuffleWavFilesQueue(){
    /*Lets shuffle for more fair distribution */
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(m_queueOfWavFilesIndexes.begin(), m_queueOfWavFilesIndexes.end(), gen);
}

void FileManager::PrintNumberOfFilesProcessed() {
    std::ostringstream os;
    os << "Total number of Wave files processed : " 
                << m_numberOfFiles << '\n';
          
    Log::pLog.LogInfo(os);
 }
