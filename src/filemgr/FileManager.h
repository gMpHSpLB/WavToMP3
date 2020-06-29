/* wave to mp3 encoding using Lame lib
   @mpathak
*/
#ifndef _FILEMGR_H
#define _FILEMGR_H
#include <vector>
#include <string>

class FileManager
{
private:
	unsigned int			m_numberOfFiles		= 0;
	const char**			m_listOfFiles		= 0;
	static const int		MAX_EXTENSION		= 2;
	std::vector<std::string> m_queueOfWAVFiles;
	std::vector<int>    m_queueOfWavFilesIndexes;
	
	bool IsDirectory(const char* directoryName);
	bool IsReadable(const char* path);
	void CheckWavFileAndAddIntoQueue(char* dirName, const char* pFileName);
	void ShuffleWavFilesQueue();
	
public:
	const char* m_FileExtension[MAX_EXTENSION] = { ".wav", ".mp3" }; // mp3 in-case later want to do decode

	FileManager():m_numberOfFiles(0),m_listOfFiles(nullptr){
	}

	FileManager(const FileManager&)					= delete;
	FileManager& operator =(const FileManager&)		= delete;
	FileManager(FileManager&&)						= delete;
	FileManager& operator =(FileManager&&)			= delete;

	~FileManager() {}

	bool GetFilesFromDirectory(char* directoryName);
	bool IsWAVFilePresent();
	const char* PopWAVFileFromQueue();
	void PrintNumberOfFilesProcessed();
};

#endif //_FILEMGR_H

