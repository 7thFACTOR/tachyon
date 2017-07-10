#pragma once
#include "base/array.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/thread.h"
#include "base/mutex.h"
#include "base/range.h"

namespace base
{
class Job;

struct Task
{
	Job* job = nullptr;
	Task* parent = nullptr;
	Task* dependency = nullptr;
	ThreadId workerThreadId = 0;
	u32 workerIndex = -1;
	u32 dataType = 0;
	void* data = nullptr;
	u64 dataSize = 0;
	RangeU32 itemRange;
	AtomicInteger stop = 0;
	AtomicInteger done = 0;
	AtomicInteger used = 0;
	AtomicInteger workingChildrenCount = 0;
#ifdef B_USE_JOB_PROFILING
	u32 tag = 0;
	f64 startTime = 0.0f;
	f64 endTime = 0.0f;
#endif
};

class B_API Worker : public Thread
{
public:
	Worker(u32 index, size_t maxTasks);
	virtual ~Worker();

	void addTask(Task* task);
	//TODO: implement task stealing from other workers if affinity is allowing it
	void onRun();
	bool isBusy();
	size_t taskCount() const;
	
protected:
	void sortTasks();
	
	Array<Task*> tasks;
	Mutex tasksLock;
	AtomicInteger busy = 0;
	u32 index = 0;
};

class B_API TaskScheduler
{
public:
	TaskScheduler();
	~TaskScheduler();
	bool initialize();
	bool shutdown();
	void setWorkerCount(u32 count);
	inline size_t getWorkerCount() const { return workers.size(); }
	Task* addEmptyTask();
	Task* addTask(Job* job, u32 dataType, void* data, u64 dataSize);
	Task* addTask(Job* job, Task* parent, u32 dataType, void* data, u64 dataSize);
	Task* addTask(Job* job, const RangeU32& range);
	Task* addTask(Job* job, Task* parent, const RangeU32& range);
	Task* addTask(Job* job, Task* parent);
	Task* addTask(Task* parent);
	void addDependency(Task* task, Task* dependency);
	bool waitToFinish(Task* task);
	void reset();
	inline const Array<Task>& getTaskPool() const { return taskPool; }
	void getWorkerTasks(size_t workerIndex, Array<Task*>& outTasks);
	void startProfiling();
	void endProfiling();
	inline f64 getProfilerStartTime() const { return profilerStartTime; }
	inline f64 getProfilerEndTime() const { return profilerEndTime; }
	inline f64 getProfilerTotalTime() const { return profilerEndTime - profilerStartTime; }
	size_t getActiveTaskCount();

protected:
	void schedule();
	Task* findFreeTaskSlot();

	f64 profilerStartTime = 0.0f;
	f64 profilerEndTime = 0.0f;
	ThreadPriority workerThreadPriority = ThreadPriority::Normal;
	bool useHalfCores;
	size_t lastUsedTaskIndex = 0;
	Array<Worker*> workers;
	Array<Task> taskPool;
};

}