/* wave to mp3 encoding using Lame lib
   @mpathak
*/
#ifndef _LOGGER_H_
#define _LOGGER_H_
#include <array>
#include <time.h>

enum class eErrorLevel {
	Error,
	Info,
	Debug
};

class Logger {
	static const unsigned int	m_MaxTimeBuffer = 80;
	char       m_TimeBuffer[m_MaxTimeBuffer];
protected:
	static const unsigned int	m_NumberErrorType   = 3;
	eErrorLevel					m_ErrorLevel		= eErrorLevel::Error;
	const std::array<const char*, m_NumberErrorType> m_ErrorType{ "ERROR", "INFO", "DEBUG" };

	Logger()							= default;
	Logger(const Logger&)				= default;
	Logger& operator = (const Logger&)	= delete;
	Logger(Logger&&)					= delete;
	Logger& operator = (Logger&&)		= delete;
	virtual ~Logger() {
	}

	const char* GetCurrentDateTime() {
		time_t     currentTime = time(0);
		struct tm  tStruct;
		tStruct = *localtime(&currentTime);
		strftime(m_TimeBuffer, m_MaxTimeBuffer, "%Y-%m-%d.%X", &tStruct);
		return(m_TimeBuffer);
	}

public:
	virtual void LogInfo(const char* logString)			= 0;
	virtual void LogInfo(std::ostringstream& logStream) = 0;
	virtual void LogError(const char* fileName,
						  const char* functionName,
						  const int   lineNo, 
						  const char* logString)		= 0;
	virtual void LogError(const char* fileName,
						  const char* functionName,
						  const int   lineNo, 
						  std::ostringstream& logStream)= 0;
	virtual void LogDebug(const char* fileName,
						  const char* functionName,
						  const int   lineNo, 
						  const char* logString)		= 0;
	virtual void LogDebug(const char* fileName,
						  const char* functionName,
						  const int   lineNo, 
						  std::ostringstream& logStream)= 0;


	void SetErrorLevel(eErrorLevel errorLevel) {
		m_ErrorLevel = errorLevel;
	}
};
#endif //_LOGGER_H_ 