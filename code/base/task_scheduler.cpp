// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "base/task_scheduler.h"
#include "base/job.h"
#include "base/util.h"
#include "base/logger.h"

namespace base
{
const size_t maxTasksPerWorker = 100;

Worker::Worker(u32 newIndex, size_t newMaxTaskCount)
{
	index = newIndex;
	tasks.reserve(newMaxTaskCount);
}

Worker::~Worker()
{}

void Worker::addTask(Task* task)
{
	B_ASSERT(task);
	AutoLock<Mutex> autoLock(tasksLock);
	task->workerThreadId = threadHandle;
	task->workerIndex = index;
	task->done = 0;
	task->stop = 0;
	tasks.appendUnique(task);
}

void Worker::onRun()
{
	while (isRunning())
	{
		sortTasks();

		bool hasTasks = false;
		Task* task = nullptr;

		while (true)
		{
			{
				AutoLock<Mutex> autoLock(tasksLock);

				hasTasks = tasks.notEmpty();
				task = nullptr;

				// search for a task that can be execute, that is, not having children that did not yet finished
				for (size_t i = 0; i < tasks.size(); i++)
				{
					if ((int)tasks[i]->workingChildrenCount == 0)
					{
						task = tasks[i];
						tasks.eraseAt(i);
						break;
					}
				}

				// no task is ready, maybe waiting for children tasks to finish
				if (!task)
					break;
			}

			if (!hasTasks)
			{
				break;
			}
				
			B_ASSERT(task);

			if (!isRunning())
			{
				break;
			}

			if (task->done)
			{
				continue;
			}

			task->stop = 0;
#ifdef B_USE_JOB_PROFILING
			task->startTime = getTimeMicroseconds();
#endif
			B_ASSERT(task->job);
			if (task->job)
			{
				task->job->execute(*task);
			}
#ifdef B_USE_JOB_PROFILING
			task->endTime = getTimeMicroseconds();
#endif
			task->stop = 1;

			if (task->parent)
			{
				if ((int)task->parent->workingChildrenCount != 0)
				{
					task->parent->workingChildrenCount--;
				}
			}

			task->done = 1;
		}

		yield();
	}
}

bool Worker::isBusy()
{
	return busy.read() != 0;
}

size_t Worker::taskCount() const
{
	return tasks.size();
}

void Worker::sortTasks()
{
	//TODO
}

//////////////////////////////////////////////////////////////////////////

TaskScheduler::TaskScheduler()
{
	workerThreadPriority = ThreadPriority::Normal;
	useHalfCores = true;
}

TaskScheduler::~TaskScheduler()
{}

bool TaskScheduler::initialize()
{
	SystemInfo si;

	B_LOG_INFO("Get system info");
	si = getSystemInfo();
	int hwCores = si.physicalProcessorCount / (useHalfCores ? 2 : 1);
	// keep a hw thread for main thread
	hwCores--;
	
	if (hwCores <= 0)
		hwCores = 1;

	B_LOG_INFO("Worker thread count " << hwCores);
	setWorkerCount(hwCores);
	taskPool.resize(maxTasksPerWorker * hwCores);

	return true;
}

bool TaskScheduler::shutdown()
{
	B_LOG_INFO("Shutdown workers...");
	
	// lets stop all first
	for (auto worker : workers)
	{
		worker->stop();
		delete worker;
	}

	workers.clear();

	return true;
}

void TaskScheduler::setWorkerCount(u32 count)
{
	// lets stop all first
	for (auto worker : workers)
	{
		worker->stop();
		delete worker;
	}

	workers.resize(count);

	for (size_t i = 0; i < workers.size(); ++i)
	{
		B_LOG_INFO("Creating worker thread " << (int)i);
		workers[i] = new Worker((u32)i, maxTasksPerWorker);
		workers[i]->create(nullptr, true, String("Worker") << (int)i, workerThreadPriority, 0);
		setThreadAffinity(workers[i]->getThreadHandle(), 1 << i);
	}
}

Task* TaskScheduler::addEmptyTask()
{
	Task* task = findFreeTaskSlot();
	
	if (!task)
	{
		B_ASSERT(!"No more free slots for tasks");
		return nullptr;
	}

	task->done = false;
	task->stop = false;
	task->used = true;
	task->parent = nullptr;
	task->job = nullptr;

	return task;
}

Task* TaskScheduler::addTask(Job* job, u32 dataType, void* data, u64 dataSize)
{
	return addTask(job, nullptr, dataType, data, dataSize);
}

Task* TaskScheduler::addTask(Job* job, Task* parent)
{
	return addTask(job, parent, 0, nullptr, 0);
}

Task* TaskScheduler::addTask(Task* parent)
{
	return addTask(nullptr, parent, 0, nullptr, 0);
}

void TaskScheduler::addDependency(Task* task, Task* dependency)
{
	task->dependency = dependency;
}

Task* TaskScheduler::addTask(Job* job, const RangeU32& range)
{
	Task* task = addTask(job, 0, 0, 0);
	task->itemRange = range;

	return task;
}

Task* TaskScheduler::addTask(Job* job, Task* parent, const RangeU32& range)
{
	Task* task = addTask(job, parent, 0, 0, 0);

	B_ASSERT(task);
	task->itemRange = range;

	return task;
}

Task* TaskScheduler::addTask(Job* job, Task* parent, u32 dataType, void* data, u64 dataSize)
{
	Task* task = findFreeTaskSlot();

	if (!task)
	{
		B_ASSERT(!"No more free task slots");
		return nullptr;
	}

	task->job = job;
	task->dataType = dataType;
	task->done = false;
	task->stop = false;
	task->used = true;
	task->data = data;
	task->dataSize = dataSize;
	task->parent = parent;

	if (parent)
	{
		parent->workingChildrenCount++;
	}

	return task;
}

bool TaskScheduler::waitToFinish(Task* task)
{
	if (!task)
	{
		return false;
	}
	
	while (true)
	{
		if (task->done)
		{
			break;
		}

		schedule();
		yield();
	}

	task->used = false;

	return true;
}

void TaskScheduler::reset()
{
	for (auto& task : taskPool)
	{
		task.used = 0;
		task.workingChildrenCount = 0;
		task.done = 0;
		task.stop = 0;
		task.job = nullptr;
		task.parent = nullptr;
		task.workerThreadId = 0;
		task.workerIndex = ~0;
	}
}

void TaskScheduler::getWorkerTasks(size_t workerIndex, Array<Task*>& outTasks)
{
	outTasks.clear();

	for (auto& task : taskPool)
	{
		if (task.used && task.workerIndex == workerIndex)
		{
			outTasks.appendUnique(&task);
		}
	}
}

void TaskScheduler::startProfiling()
{
	profilerStartTime = getTimeMilliseconds();
}

void TaskScheduler::endProfiling()
{
	profilerEndTime = getTimeMilliseconds();
}

void TaskScheduler::schedule()
{
	B_ASSERT(workers.notEmpty());
	u32 tasksPerWorker = (u32)taskPool.size() / (u32)workers.size();
	size_t workerIndex = 0;

	for (size_t ti = 0; ti < taskPool.size(); ti++)
	{
		auto& task = taskPool[ti];

		// if already scheduled or not used, skip
		if (task.workerThreadId || !task.used)
		{
			continue;
		}

		// don't start task if its dependency is not done
		if (task.dependency)
		{
			if (!task.dependency->done)
			{
				continue;
			}
		}

		// must be a chain link task
		if (!task.job && !task.done)
		{
			if ((int)task.workingChildrenCount == 0)
			{
				task.done = 1;
				task.workingChildrenCount = 0;

				if (task.parent)
				{
					if ((int)task.parent->workingChildrenCount)
						task.parent->workingChildrenCount--;
				}

				continue;
			}
		}

		if (!task.job)
		{
			continue;
		}
			
		u32 affinity = task.job->affinity;

		if (affinity != -1 && affinity < workers.size())
		{
			// this task prefers this thread index
			//TODO: check max tasks for this thread
			workers[affinity]->addTask(&task);
			continue;
		}

		// if this worker is full, then find another free worker
		if (workers[workerIndex]->taskCount() >= tasksPerWorker)
		{
			size_t wi = 0;

			for (auto worker : workers)
			{
				if (worker->taskCount() < tasksPerWorker)
				{
					workerIndex = wi;
					break;
				}

				wi++;
			}
		}

		// move to next worker after adding task
		workers[workerIndex++]->addTask(&task);
		workerIndex %= workers.size();
	}
}

Task* TaskScheduler::findFreeTaskSlot()
{
	for (size_t i = 0; i < lastUsedTaskIndex; i++)
	{
		if (!taskPool[i].used)
		{
			return &taskPool[i];
		}
	}

	if (lastUsedTaskIndex >= taskPool.size())
	{
		return nullptr;
	}

	lastUsedTaskIndex++;
	taskPool[lastUsedTaskIndex].used = 1;
	return &taskPool[lastUsedTaskIndex];
}

size_t TaskScheduler::getActiveTaskCount()
{
	size_t counter = 0;

	for (size_t i = 0; i < taskPool.size(); ++i)
	{
		if (taskPool[i].used)
		{
			counter++;
		}
	}

	return counter;
}

}