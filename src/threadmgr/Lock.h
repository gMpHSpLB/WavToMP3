/* wave to mp3 encoding using Lame lib
   @mpathak
*/
#ifndef _LOCK_H_
#define _LOCKT_H_
#include <pthread.h>

class Lock {
	pthread_mutex_t* m_pMutexObj	= nullptr;

	void ReleaseLock() {
		if (!m_pMutexObj) {
			pthread_mutex_unlock(m_pMutexObj);
		}
	}
public:
	Lock(pthread_mutex_t& mutexObj) :m_pMutexObj(&mutexObj) {

	}

	Lock(const Lock&)					= delete;
	Lock& operator = (const Lock&)		= delete;
	Lock(Lock&&)						= delete;
	Lock& operator = (Lock&&)			= delete;

	void AcquireLock() {
		if (!m_pMutexObj) {
			pthread_mutex_lock(m_pMutexObj);
		}
	}
	
	void DestroyMutex() {
		if (!m_pMutexObj) {
			pthread_mutex_destroy(m_pMutexObj);
		}
	}

	~Lock() {
		ReleaseLock();
	}
};
#endif // _LOCK_H_
