// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/dictionary.h"
#include "base/string.h"
#include "base/platform.h"
#include "base/atomic.h"

namespace base
{
class B_API Thread
{
public:
	Thread();
	virtual ~Thread();

	virtual bool create(
		void* userData,
		bool startThreadNow,
		const String& name,
		ThreadPriority priority = ThreadPriority::Normal,
		size_t stackSize = 0);

	void setPriority(ThreadPriority value);
	ThreadHandle getThreadHandle() const;
	ThreadPriority getPriority() const;
	void* getUserData() const;
	u64 getStackSize() const;
	bool isRunning();
	bool isSuspended();
	bool start();
	void suspend();
	void resume();
	//! tell the thread to exit, and optionally wait for the thread to end,
	//! max aMaxTimeMsec milliseconds, 0 - infinite
	bool stop(bool waitThreadToExit = true, u32 maxTimeMsec = 0);
	void close();
	virtual void onRun() = 0;

protected:
	static ThreadReturnType startThread(void* data);

	ThreadHandle threadHandle = 0;
	void* threadUserData = 0;
	ThreadPriority threadPriority = ThreadPriority::Normal;
	size_t threadStackSize = 0;
	AtomicInteger running = 0, suspended = 1;
	String threadName;
};

}