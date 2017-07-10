#include "base/semaphore.h"

namespace base
{
Semaphore::Semaphore(u32 newInitialCount, u32 newMaxCount)
{
	initialCount = newInitialCount;
	maxCount = newMaxCount;
	count.set(newInitialCount);
}

Semaphore::~Semaphore()
{}

void Semaphore::setCount(u32 value)
{
	count.set(value);
}

void Semaphore::setInitialCount(u32 value)
{
	initialCount = value;
}

void Semaphore::setMaxCount(u32 value)
{
	maxCount = value;
}

u32 Semaphore::getCount()
{
	return count.read();
}

u32 Semaphore::getInitialCount() const
{
	return initialCount;
}

u32 Semaphore::getMaxCount() const
{
	return maxCount;
}

bool Semaphore::use()
{
	if (count.read() == 0)
		return false;

	if (count.read() > 0)
		count.decrement();

	return true;
}

void Semaphore::release()
{
	if (count.read() < maxCount)
	{
		count.increment();
	}
}

bool Semaphore::wait(u32 maxTimeMsec)
{
	u32 time = getTimeMilliseconds();

	while (count.read() > 0)
	{
		if (maxTimeMsec != -1)
		{
			if (getTimeMilliseconds() - time > maxTimeMsec)
			{
				if (count.read() > 0)
				{
					return false;
				}
				break;
			}
		}

		yield();
	}

	return true;
}

}