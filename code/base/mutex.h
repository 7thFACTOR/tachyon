#pragma once
#include "base/defines.h"
#include "base/dictionary.h"
#include "base/string.h"
#include "base/platform.h"

namespace base
{
//! Reentrant mutex
class B_API Mutex
{
public:
	Mutex();
	virtual ~Mutex();

	void acquire();
	void release();
	bool isAcquired() const;
	bool tryAcquire(u32 maxTimeMsec = ~0 /*infinite*/);

protected:
	CriticalSectionHandle criticalSection;
	CriticalSectionHandle criticalSectionRead;
	volatile bool locked;
	volatile ThreadId threadId;
	volatile u64 usageCount;
};

template<class Type>
class AutoLock
{
public:
	AutoLock(Type& mutex)
		: lock(mutex)
	{
		lock.acquire();
	}

	~AutoLock()
	{
		lock.release();
	}

	Type& lock;
};

}