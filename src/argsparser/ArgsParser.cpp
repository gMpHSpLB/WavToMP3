/* wave to mp3 encoding using Lame lib
   @mpathak
*/
#include <sstream>
#include "ArgsParser.h"
#include "Log.h"

void ArgsParser::Usage(const char* programName) {
	std::ostringstream os;
	os<<"Usage: " << programName << " " << "<Path Of Wave files Directory>" << '\n';
	Log::pLog.LogInfo(os);
}

bool ArgsParser::ValidateInputArguments(const int argc, const char* argv[]) {

	if ( argc != 2 ) {
		Usage(argv[0]);
		return(false);
	}
	/* Other Argument can be added later if required */
	return(true);
}
