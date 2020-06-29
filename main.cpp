/* wave to mp3 encoding using Lame lib
   @mpathak
*/
#include <iostream>
#include <unistd.h>
#include "ArgsParser.h"
#include "FileManager.h"
#include "StreamInfo.h"
#include "EncodeManager.h"
#include "Log.h"

int main(int argc, char* argv[]) {
	int		ret		= 0;
	ArgsParser objParser;
	Log::pLog.SetErrorLevel(eErrorLevel::Info);
	if (!objParser.ValidateInputArguments(argc, (const char**)argv)) {
		Log::pLog.LogError(__FILE__,
						   __FUNCTION__,
						   __LINE__,
						   "Invalid Arguments!!! \n");
		return(0);
	}

	FileManager objFM;
	if (!objFM.GetFilesFromDirectory(argv[1])) {
		Log::pLog.LogError(__FILE__,
						   __FUNCTION__,
						   __LINE__,
						   "Failed to get files from directory \n");
		return(0);
	}
	
	/*Set Stream Configuration, in-case user explicitly want to set*/
	StreamInfo streamObj;
	
	/*Encode to MP3*/
	EncodeManager objEncode;
	objEncode.WavToMp3(&objFM, &streamObj);

	objFM.PrintNumberOfFilesProcessed();
	return(ret);
}
