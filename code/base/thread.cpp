#include "base/thread.h"
#include "base/assert.h"
#include "base/logger.h"

namespace base
{
const u32 maxWaitTimeMsecForCloseThread = 5000;

Thread::Thread()
{
	threadHandle = 0;
	threadUserData = nullptr;
	threadPriority = ThreadPriority::Normal;
	threadStackSize = 0;
	running = false;
	threadName = "Thread";
}

Thread::~Thread()
{
	if (threadHandle || running.read())
	{
		B_LOG_ERROR("Thread '" << threadName << "' is running away, it was not stopped and the handle is still not zero.");
	}

	close();
}

bool Thread::create(void* userData, bool startThreadNow, const String& name, ThreadPriority priority, size_t stackSize)
{
	threadUserData = userData;
	threadPriority = priority;
	threadStackSize = stackSize;
	running = false;
	threadName = name;
	threadHandle = createThread(
						startThread,
						this,
						threadPriority,
						threadStackSize,
						threadName);

	if (startThreadNow)
	{
		start();
	}

	return true;
}

void Thread::setPriority(ThreadPriority value)
{
	setThreadPriority(threadHandle, value);
}

ThreadHandle Thread::getThreadHandle() const
{
	return threadHandle;
}

ThreadPriority Thread::getPriority() const
{
	return threadPriority;
}

void* Thread::getUserData() const
{
	return threadUserData;
}

u64 Thread::getStackSize() const
{
	return threadStackSize;
}

bool Thread::isRunning()
{
	return running.read() != 0;
}

bool Thread::isSuspended()
{
	return suspended.read() != 0;
}

bool Thread::start()
{
	if (running.read())
	{
		B_LOG_WARNING("Trying to run a thread which is already running, thread name: '" << threadName << "'");
	}

	if (!threadHandle)
	{
		B_LOG_DEBUG("Cannot run a thread which was closed, handle is zero.");
	}

	resume();

	return (threadHandle != 0);
}

void Thread::suspend()
{
	suspendThread(threadHandle);
}

void Thread::resume()
{
	if (!threadHandle)
	{
		B_LOG_DEBUG("Cannot resume a thread that was closed.");
		return;
	}

	if (running.read())
	{
		B_LOG_DEBUG("Cannot resume a thread that is already running");
		return;
	}

	threadHandle = createThread(startThread, this, threadPriority, threadStackSize, threadName);
	resumeThread(threadHandle);
}

void Thread::onRun()
{}

ThreadReturnType Thread::startThread(void* data)
{
	Thread* thread = (Thread*)data;

	if (thread)
	{
		setThreadName(thread->threadHandle, thread->threadName);
		thread->running = true;
		thread->onRun();
		thread->running = false;
	}

	return 0;
}

bool Thread::stop(bool waitThreadToExit, u32 maxTimeMsec)
{
	running = false;
	
	bool ok = closeThread(threadHandle, waitThreadToExit ? maxTimeMsec : 1);
	
	threadHandle = 0;
	
	return ok;
}

void Thread::close()
{
	running = false;

	if (threadHandle)
	{
		closeThread(threadHandle);
		threadHandle = 0;
	}
}

}