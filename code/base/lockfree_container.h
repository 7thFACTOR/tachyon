#pragma once
#include "base/defines.h"
#include "base/platform.h"
#include "base/mutex.h"

namespace base
{

//TODO: add Allocator support
template<typename Type>
class LockfreeContainer
{
public:
	LockfreeContainer()
	{
		criticalSection = createCriticalSection();
		consumedQueue = &containers[0];
		filledQueue = &containers[1];
	}

	virtual ~LockfreeContainer()
	{
		if (criticalSection)
		{
			deleteCriticalSection(criticalSection);
		}
	}

	Type& getConsumedQueue() const
	{
		enterCriticalSection(criticalSection);
		Type& ret = *consumedQueue;
		leaveCriticalSection(criticalSection);

		return ret;
	}

	Type& getFilledQueue() const
	{
		enterCriticalSection(criticalSection);
		Type& ret = *filledQueue;
		leaveCriticalSection(criticalSection);

		return ret;
	}

	void swapQueues()
	{
		enterCriticalSection(criticalSection);
		Type* tmp = consumedQueue;
		consumedQueue = filledQueue;
		filledQueue = tmp;
		leaveCriticalSection(criticalSection);
	}

protected:
	Type* consumedQueue;
	Type* filledQueue;
	Type containers[2];
	CriticalSectionHandle criticalSection;
};

template <typename Type>
class ThreadedArray : public LockfreeContainer<Array<Type> >
{};

template <typename KeyType, typename ValueType>
class ThreadedMap : public LockfreeContainer<Dictionary<KeyType, ValueType> >
{};

}