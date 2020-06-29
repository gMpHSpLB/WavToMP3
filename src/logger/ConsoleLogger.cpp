/* wave to mp3 encoding using Lame lib
   @mpathak
*/
#include <iostream>
#include <chrono>
#include <ctime>
#include "ConsoleLogger.h"
#include "Lock.h"

static pthread_mutex_t g_LoggerMutexObj = PTHREAD_MUTEX_INITIALIZER;

void ConsoleLogger::LogError(const char* fileName, 
							 const char* functionName,
							 const int   lineNo, 
							 const char* logString) {
	if ((logString) && (functionName) && (fileName)){
		Lock  lockObj(g_LoggerMutexObj);
		switch (m_ErrorLevel) {
		case eErrorLevel::Error:
		case eErrorLevel::Info:
			lockObj.AcquireLock();
			std::cout << GetCurrentDateTime() << ": " 
						<< "[" << m_ErrorType[0] << "]: " 
						<< logString;
			break;
		case eErrorLevel::Debug:
			lockObj.AcquireLock();
			std::cout << GetCurrentDateTime() << ": "
						<< "[" << m_ErrorType[0] << ": "
						<< fileName << ": "
						<< functionName << ": "
						<< lineNo  << "]: "
						<< logString;
			break;
		}
	}
}

void ConsoleLogger::LogError(const char* fileName, 
							 const char* functionName, 
							 const int   lineNo, 
							 std::ostringstream& logStream) {
	LogError(fileName, functionName, lineNo, logStream.str().c_str());
}

void ConsoleLogger::LogInfo(const char* logString) {
	if ((m_ErrorLevel >= eErrorLevel::Info) && (logString)) {
		Lock  lockObj(g_LoggerMutexObj);
		lockObj.AcquireLock();
		std::cout << GetCurrentDateTime() << ": " 
					<<"[" << m_ErrorType[1] << "]: " 
					<< logString;
	}
}

void ConsoleLogger::LogInfo(std::ostringstream& logStream) {
	LogInfo(logStream.str().c_str());
}

void ConsoleLogger::LogDebug(const char* fileName,
							 const char* functionName,
							 const int   lineNo, 
							 const char* logString) {
	if ((m_ErrorLevel >= eErrorLevel::Debug) && (logString) && (functionName) && (fileName)) {
		Lock  lockObj(g_LoggerMutexObj);
		lockObj.AcquireLock();
		std::cout << GetCurrentDateTime() << ": " 
					<< "[" << m_ErrorType[2] <<": "
					<< fileName << ": "
					<< functionName  << ": "
					<< lineNo << "]: "
					<< logString;
	}
}

void ConsoleLogger::LogDebug(const char* fileName, 
							 const char* functionName,
							 const int   lineNo, 
							 std::ostringstream& logStream) {
	LogDebug(fileName, functionName, lineNo, logStream.str().c_str());
}