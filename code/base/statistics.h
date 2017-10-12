// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/string.h"
#include "base/array.h"
#include "base/defines.h"
#include "base/types.h"

namespace base
{
enum class StatCounterType
{
	Quantity,
	Time
};

struct StatCounter
{
	f32 count = 0.0f;
	f32 peakValue = 0.0f;
	u32 startTime = 0;
	u32 lastTime = 0;
	u32 endTime = 0;
	StatCounterType type = StatCounterType::Quantity;
	String label, name, unitsName;
};

inline StatCounter& operator += (StatCounter& counter, f32 value)
{
	counter.count += value;
	return counter;
}

class B_API Statistics
{
public:
	Statistics();
	virtual ~Statistics();

	const Array<StatCounter*>& getCounters() const;
	void addCounter(StatCounter* counter);
	void removeCounter(StatCounter* counter);
	void update();
	StatCounter* findCounter(const String& name) const;
	void startCounter(StatCounter* counter);
	void computeCounter(StatCounter* counter);

private:
	Array<StatCounter*> counters;
};

}