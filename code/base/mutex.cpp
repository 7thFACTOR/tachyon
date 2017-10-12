// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "base/mutex.h"

namespace base
{
const ThreadId noThreadId = 0;

Mutex::Mutex()
{
	criticalSection = createCriticalSection();
	criticalSectionRead = createCriticalSection();
	locked = false;
	usageCount = 0;
	threadId = noThreadId;
}

Mutex::~Mutex()
{
	deleteCriticalSection(criticalSection);
	deleteCriticalSection(criticalSectionRead);
}

void Mutex::acquire()
{
	enterCriticalSection(criticalSectionRead);
	ThreadId crtTID = getCurrentThreadId();

	if (crtTID == threadId && usageCount)
	{
		usageCount++;
		leaveCriticalSection(criticalSectionRead);
		return;
	}

	leaveCriticalSection(criticalSectionRead);
	enterCriticalSection(criticalSection);
	threadId = getCurrentThreadId();
	locked = true;
	usageCount++;
}

void Mutex::release()
{
	enterCriticalSection(criticalSectionRead);
	ThreadId crtTID = getCurrentThreadId();

	if (crtTID == threadId && usageCount)
	{
		usageCount--;
	}

	if (usageCount > 0)
	{
		leaveCriticalSection(criticalSectionRead);
		return;
	}

	leaveCriticalSection(criticalSectionRead);
	locked = false;
	threadId = noThreadId;
	leaveCriticalSection(criticalSection);
}

bool Mutex::isAcquired() const
{
	return locked;
}

bool Mutex::tryAcquire(u32 maxTimeMsec)
{
	u32 time = getTimeMilliseconds();

	while (!tryEnterCriticalSection(criticalSection))
	{
		if (maxTimeMsec != -1)
		{
			if (getTimeMilliseconds() - time > maxTimeMsec)
			{
				if (locked)
				{
					return false;
				}

				break;
			}
		}

		yield();
	}

	locked = true;

	return true;
}

}