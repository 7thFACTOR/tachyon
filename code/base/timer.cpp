// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "base/assert.h"
#include "base/math/util.h"
#include "base/timer.h"
#include "base/platform.h"

namespace base
{
Timer::Timer()
{
	normalized = delta = 0;
	startTime = endTime = length = 0;
	time = lastTime = getTimeMilliseconds();
	speed = 1.0f;
	smoothDelta = false;
	capDelta = true;
	maxDelta = 1.0f / 5.0f; // 5 fps min
}

Timer::Timer(const Timer& other)
{
	*this = other;
}

Timer::~Timer()
{}

f32 Timer::getNormalized() const
{
	return normalized;
}

f32 Timer::getDelta() const
{
	return delta;
}

u32 Timer::getStartTime() const
{
	return startTime;
}

u32 Timer::getEndTime() const
{
	return endTime;
}

u32 Timer::getTime() const
{
	return time;
}

u32 Timer::getLength() const
{
	return length;
}

u32 Timer::getLastTime() const
{
	return lastTime;
}

f32 Timer::getSpeedMultiplier() const
{
	return speed;
}

void Timer::setNormalized(f32 value)
{
	normalized = value;
}

void Timer::setDelta(f32 value)
{
	delta = value;
}

void Timer::setStart(u32 value)
{
	startTime = value;
}

void Timer::setEnd(u32 value)
{
	endTime = value;
}

void Timer::setTime(u32 value)
{
	time = value;
}

void Timer::setLength(u32 value)
{
	length = value;
}

void Timer::setLast(u32 value)
{
	lastTime = value;
}

void Timer::setSpeedMultiplier(f32 speedMul)
{
	speed = speedMul;
}

void Timer::start(u32 lenMsec)
{
	length = lenMsec;
	startTime = time;
	endTime = startTime + length;
	lastTime = startTime;
}

u32 Timer::update(Timer* parentTimer)
{
	if (!parentTimer)
	{
		time = getTimeMilliseconds();
		normalized = (f32)(time - startTime) / length;
		delta = (f32)(time - lastTime) / 1000.0f;
		lastTime = time;
	}
	else
	{
		time = parentTimer->time;
		normalized = (f32)(time - startTime) / length;
		delta = (f32)(time - lastTime) / 1000.0f;
		lastTime = time;
	}

	if (capDelta)
	{
		if (delta > maxDelta)
		{
			delta = maxDelta;
		}
	}

	delta *= speed;

	if (smoothDelta)
	{
		deltaSmoother.addValue(delta);
		delta = deltaSmoother.compute();
	}

	return time;
}

//---

Clock::Clock()
{
	lastId = 0;
	mainTimer.start(1000);
}

Clock::~Clock()
{
	deleteAllTimers();
}

TimerId Clock::createTimer()
{
	Timer* timer = new Timer();

	B_ASSERT(timer);
	timers[lastId] = timer;
	TimerId id = lastId;
	++lastId;

	return id;
}

bool Clock::deleteTimer(TimerId id)
{
	size_t index = timers.findIndex(id);

	if (index != -1)
	{
		delete timers[id];
		timers.erase(id);

		return true;
	}

	return false;
}

Timer* Clock::getTimer(TimerId id) const
{
	size_t index = timers.findIndex(id);
	
	if (index != -1)
	{
		return timers.valueAt(index);
	}

	return nullptr;
}

Timer& Clock::getMainTimer()
{
	return mainTimer;
}

void Clock::deleteAllTimers()
{
	for (auto timer : timers)
	{
		delete timer.value;
	}

	timers.clear();
}

void Clock::update()
{
	mainTimer.update();

	for (auto timer : timers)
	{
		B_ASSERT(timer.value);
		timer.value->update(&mainTimer);
	}
}

}