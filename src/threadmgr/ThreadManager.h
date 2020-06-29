/* wave to mp3 encoding using Lame lib
   @mpathak
*/
#ifndef _THREADMGR_H_
#define _THREADMGR_H_
#include <vector>
#include <memory>
#include <pthread.h>

using  threadTask  = void* (*)(void*);

class ThreadManager{
	unsigned int m_NumberOfThreads	= 0;
	unsigned int m_NumOfCores		= 0;
	std::vector<std::shared_ptr<pthread_t>> m_ThreadQueue;

	void Initialize();
	bool CreateThread(unsigned int);
	void GetNumberOfCores();

public:
	enum class eWaitThread {
		join,
		detach,
	};

	ThreadManager() {
		Initialize();
	}

	ThreadManager(const ThreadManager&)				= delete;
	ThreadManager& operator=(const ThreadManager&)	= delete;
	ThreadManager(ThreadManager&&)					= delete;
	ThreadManager& operator=(ThreadManager&&)		= delete;

	void ExecuteTaskInSeperateThreads(threadTask task, void* taskArgs, eWaitThread waitForExit);
	void RemoveAllThreadsFromQueue();

	~ThreadManager() {
		RemoveAllThreadsFromQueue();
	}
};

#endif //_THREADMGR_H_

