#pragma once
#include "base/defines.h"
#include "base/types.h"

namespace base
{
struct Task;

class Job
{
public:
	virtual void execute(Task& task) = 0;

	static const u32 noAffinity = ~0;

	u32 priority = 0;
	u32 affinity = noAffinity;
#ifdef B_USE_JOB_PROFILING
	u32 tag = 0;
	f64 averageTime = 0;
#endif
};
}