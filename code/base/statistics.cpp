#include "base/statistics.h"
#include "base/platform.h"

namespace base
{
Statistics::Statistics()
{}

Statistics::~Statistics()
{
	for (auto counter : counters)
	{
		delete counter;
	}
}

const Array<StatCounter*>& Statistics::getCounters() const
{
	return counters;
}

void Statistics::addCounter(StatCounter* counter)
{
	counters.append(counter);
}

void Statistics::removeCounter(StatCounter* counter)
{
	auto iter = counters.find(counter);

	if (iter != counters.end())
	{
		counters.erase(iter);
	}
}

void Statistics::update()
{
	for (auto counter : counters)
	{
		computeCounter(counter);
	}
}

StatCounter* Statistics::findCounter(const String& name) const
{
	for (auto counter : counters)
	{
		if (counter->name == name)
		{
			return counter;
		}
	}

	return nullptr;
}

void Statistics::startCounter(StatCounter* counter)
{
	counter->count = 0;
	counter->startTime = getTimeMilliseconds();
	counter->lastTime = counter->startTime;
	counter->endTime = 0;
}

void Statistics::computeCounter(StatCounter* counter)
{
	switch (counter->type)
	{
	case StatCounterType::Quantity:
	{
		break;
	}

	case StatCounterType::Time:
	{
		counter->endTime = counter->lastTime = getTimeMilliseconds();
		counter->count = (f32)counter->endTime - counter->startTime;
		break;
	}
	}

	if (counter->peakValue < counter->count)
	{
		counter->peakValue = counter->count;
	}
}

}