/* wave to mp3 encoding using Lame lib
   @mpathak
*/
#ifndef _CONSOLELOG_H_
#define _CONSOLELOG_H_
#include <sstream>
#include "Logger.h"

class ConsoleLogger : public Logger {
private:
	ConsoleLogger()								= default;
	~ConsoleLogger(){}

public:
	ConsoleLogger(const Logger&)				= delete;
	ConsoleLogger& operator = (const Logger&)	= delete;
	ConsoleLogger(Logger&&)						= delete;
	ConsoleLogger& operator = (Logger&&)		= delete;

	static ConsoleLogger& GetInstance() {
		static ConsoleLogger m_Instance;
		return(m_Instance);
	}
	virtual void LogInfo(const char* logString)			override;
	virtual void LogInfo(std::ostringstream& logStream) override;

	virtual void LogError(const char* fileName,
						  const char* functionName,
						  const int   lineNo,
						  const char* logString)		override;

	virtual void LogError(const char* fileName,
						  const char* functionName,
						  const int   lineNo,
						  std::ostringstream& logStream) override;

	virtual void LogDebug(const char* fileName,
						  const char* functionName,
						  const int lineNo,
						  const char* logString)		 override;
	virtual void LogDebug(const char* fileName,
						  const char* functionName,
						  const int   lineNo,
						  std::ostringstream& logStream) override;
};
#endif //_CONSOLELOG_H_