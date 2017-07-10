#include "base/simple_mutex.h"

namespace base
{
SimpleMutex::SimpleMutex()
{
	criticalSection = createCriticalSection();
	locked = false;
}

SimpleMutex::~SimpleMutex()
{
	deleteCriticalSection(criticalSection);
}

void SimpleMutex::acquire()
{
	enterCriticalSection(criticalSection);
	locked = true;
}

void SimpleMutex::release()
{
	leaveCriticalSection(criticalSection);
	locked = false;
}

bool SimpleMutex::isAcquired() const
{
	return locked;
}

bool SimpleMutex::tryAcquire(u32 maxTimeMsec)
{
	u32 time = getTickCount();

	while (!tryEnterCriticalSection(criticalSection))
	{
		if (maxTimeMsec != -1)
		{
			if (getTickCount() - time > maxTimeMsec)
			{
				if (locked)
				{
					return false;
				}

				break;
			}
		}

		sleep(1);
	}

	locked = true;

	return true;
}

}