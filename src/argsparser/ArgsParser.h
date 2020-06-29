/* wave to mp3 encoding using Lame lib
   @mpathak
*/
#ifndef _ARGSPARSER_H_
#define _ARGSPARSER_H_

class ArgsParser{
	void Usage(const char* programName);

public:
	ArgsParser()								= default;
	ArgsParser(const ArgsParser&)				= default;
	ArgsParser& operator =(const ArgsParser&)	= default;
	ArgsParser(ArgsParser&&)					= default;
	ArgsParser& operator =(ArgsParser&&)		= default;


	bool ValidateInputArguments(const int argc, const char* argv[]);
};

#endif //_ARGSPARSER_H_