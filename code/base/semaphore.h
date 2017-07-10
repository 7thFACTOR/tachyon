#pragma once
#include "base/dictionary.h"
#include "base/string.h"
#include "base/defines.h"
#include "base/platform.h"
#include "base/atomic.h"

namespace base
{
//! This class represents a semaphore object, used to limit the number of created
//! resources (for example there may be maximum 10 threads for computing a data stream)\n
//! When a thread which uses this semaphore is started, it will call use() method 
//! (which will decrement the count), and when thread will exit, it will call release()
//! (which will increment the count of the semaphore)\n
//! When the count is zero, the thread cannot create another resource of that type and
//! it will have to wait for that semaphore counter to be greater than zero,
//! using the waitForSemaphore(...) global function.
class B_API Semaphore
{
public:
	Semaphore(u32 initialCount = 1, u32 maxCount = 1);
	~Semaphore();

	void setCount(u32 value);
	void setInitialCount(u32 value);
	void setMaxCount(u32 value);

	u32 getCount();
	u32 getInitialCount() const;
	u32 getMaxCount() const;

	//! use the semaphore's counter, decrementing it
	//! \return false if the counter is zero and the semaphore is signaled, you cannot create the resource!
	bool use();
	//! release the semaphore, incrementing it
	void release();
	//! function that waits for a Semaphore object for its count to be > 0
	bool wait(u32 maxTimeMsec = -1 /*infinite*/);

protected:
	u32 initialCount, maxCount;
	AtomicInteger count;
};

}