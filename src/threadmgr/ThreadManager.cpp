/* wave to mp3 encoding using Lame lib
   @mpathak
*/
#include <sstream>
#include <algorithm>
#include "ThreadManager.h"
#include "Log.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/sysinfo.h>
#endif

void ThreadManager::GetNumberOfCores() {
#ifdef _WIN32
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	m_NumOfCores = sysinfo.dwNumberOfProcessors/2;

#else
	m_NumOfCores = get_nprocs();
#endif
	{
		std::ostringstream os;
		os << "Number of cores: " << " "
			<< m_NumOfCores << '\n';
		Log::pLog.LogInfo(os);
	}
}

bool ThreadManager::CreateThread(unsigned int threadID) {
	auto pThread = std::make_shared<pthread_t>();
	m_ThreadQueue.emplace_back(pThread);
}

void ThreadManager::Initialize() {
	GetNumberOfCores();
	m_NumberOfThreads = 2 * m_NumOfCores;
	
	for (int i = 0; i < m_NumberOfThreads; ++i) {
		CreateThread(i + 1);
	}
	m_NumberOfThreads = m_ThreadQueue.size();

	{
		std::ostringstream os;
		os << "Number of threads: " << " "
				<< m_NumberOfThreads << '\n';
		Log::pLog.LogInfo(os);
	}
}

void ThreadManager::RemoveAllThreadsFromQueue() {
	while (!m_ThreadQueue.empty()) {
		auto pThread = m_ThreadQueue.back();
		pThread.reset();
		pThread = nullptr;
		m_ThreadQueue.pop_back();
		m_NumberOfThreads = m_ThreadQueue.size();
	}
}

void ThreadManager::ExecuteTaskInSeperateThreads(threadTask task, void* taskArgs, eWaitThread waitForExit)
{
	do {
		if (m_ThreadQueue.empty()) {
			Log::pLog.LogError(__FILE__,
							   __FUNCTION__,
							   __LINE__,
							   "No Thread is available \n");
			break;
		}

		auto executeThread = [&task, &taskArgs](auto& threadPtr){
			int ret = pthread_create(threadPtr.get(), NULL, task, taskArgs);
			if (0 != ret) {
				Log::pLog.LogError(__FILE__,
								   __FUNCTION__,
								   __LINE__,
								   "Failed to create thread and execute Task in seperate Thread \n");
				return(false);
			}
			return(true);
		};

		if (!std::all_of(m_ThreadQueue.begin(), m_ThreadQueue.end(), executeThread)) {
			RemoveAllThreadsFromQueue();
			break;
		}

		auto joinThread = [](auto& threadPtr) {
			int ret = pthread_join(*threadPtr, NULL);
			if (0 != ret) {
				Log::pLog.LogError(__FILE__,
								   __FUNCTION__,
								   __LINE__,
								   "Failed to join the thread \n");
				return(false);
			}
			return(true);
		};

		auto detachThread = [](auto& threadPtr) {
			int ret = pthread_detach(*threadPtr);
			if (0 != ret) {
				Log::pLog.LogError(__FILE__,
								   __FUNCTION__,
								   __LINE__,
								   "Failed to detach the thread \n");
				return(false);
			}
			return(true);
		};

		switch (waitForExit) {
		case eWaitThread::join:
			if (!std::all_of(m_ThreadQueue.begin(), m_ThreadQueue.end(), joinThread)) {
				RemoveAllThreadsFromQueue();
			}
			break;
		case eWaitThread::detach:
			if (!std::all_of(m_ThreadQueue.begin(), m_ThreadQueue.end(), detachThread)) {
				RemoveAllThreadsFromQueue();
			}
			break;
		}
	} while (0);
}
