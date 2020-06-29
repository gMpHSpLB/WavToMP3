/* wave to mp3 encoding using Lame lib
   @mpathak
*/
#ifndef _ARRAY_H_
#define _ARRAY_H_
#include <iostream>

template<typename T, unsigned int MAX_SIZE = 4096>
class Array {
	T m_Buffer[MAX_SIZE + 1];
	unsigned int m_Size = 0;
	T* m_CurPosition = nullptr;

public:
	Array() :m_Size(0) {
		m_CurPosition = &m_Buffer[0];
		memset(m_Buffer, 0, sizeof(m_Buffer));
	}

	void Reset() {
		m_CurPosition = &m_Buffer[0];
		memset(m_Buffer, 0, sizeof(m_Buffer));
		m_Size = 0;
	}
	void ResetCurrentPosition() {
		m_CurPosition = &m_Buffer[0];
	}

	unsigned int GetSize() {
		return(m_Size);
	}

	void SetSize(unsigned int newSize) {
		if (MAX_SIZE >= newSize) {
			m_Size = newSize;
		}
	}

	T* GetCurrentPosition() {
		return(m_CurPosition);
	}

	void SetConsumedElements(unsigned int consumedElements) {
		if ((m_Size >= consumedElements) && ((m_CurPosition + consumedElements) < &m_Buffer[MAX_SIZE])) {
			m_Size -= consumedElements;
			m_CurPosition += consumedElements;
		}
		else {
			m_Size = MAX_SIZE;
		}
	}

	bool InsertElements(T* data, unsigned int length) {
		if (((m_Size + length) <= MAX_SIZE) && ((m_CurPosition + length) <= &m_Buffer[MAX_SIZE])) {

			memcpy(m_CurPosition, data, length);
			m_Size += length;
			m_CurPosition += length;
			return(true);
		}
		Reset(); /* Should not reach here */
		return(false);
	}

	unsigned int AvailableSize() {
		if (m_Size <= MAX_SIZE) {
			return(MAX_SIZE - m_Size);
		}
		return(0);
	}

	~Array() {
		Reset();
	}
};

#endif // _ARRAY_H_