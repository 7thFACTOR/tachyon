#pragma once
#include "base/defines.h"
#include "base/map.h"
#include "base/string.h"
#include "base/platform.h"

namespace base
{
//DEPRECATED
//! This class represents a simple mutex synchronization object,
//! used by threads to lock a common resource to many threads when write operations occur
class BASE_API SimpleMutex
{
public:
	SimpleMutex();
	~SimpleMutex();

	//! lock the mutex, signaled state
	void acquire();
	//! unlock the mutex, non-signaled state
	void release();
	//! query mutex state, true if signaled
	bool isAcquired() const;
	//! function that waits for a SimpleMutex object to be unlocked
	bool tryAcquire(u32 maxTimeMsec = -1 /*infinite*/);

protected:
	CriticalSectionHandle criticalSection;
	volatile bool locked;
};

}