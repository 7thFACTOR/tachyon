// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/dictionary.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/math/smoother.h"

namespace base
{
class B_API Timer
{
public:
	Timer();
	Timer(const Timer& other);
	virtual ~Timer();

	//! returns the normalized time span between 0 and 1
	f32 getNormalized() const;
	//! \return delta time as seconds
	f32 getDelta() const;
	//! \return segment start time in msec
	u32 getStartTime() const;
	//! \return segment end time in msec
	u32 getEndTime() const;
	//! \return current time in msec
	u32 getTime() const;
	//! \return segment length in msec
	u32 getLength() const;
	//! \return last time value in msec
	u32 getLastTime() const;
	f32 getSpeedMultiplier() const;

	void setNormalized(f32 value);
	void setDelta(f32 value);
	void setStart(u32 value);
	void setEnd(u32 value);
	void setTime(u32 value);
	void setLength(u32 value);
	void setLast(u32 value);
	void setSpeedMultiplier(f32 speedMul);
	void setDeltaCapping(bool enable, f32 maxDelta = 0.0f);

	void start(u32 lenMSec);
	u32 update(Timer* parentTimer = nullptr);

private:
	Smoother<f32> deltaSmoother;
	bool smoothDelta = true, capDelta = false;
	f32 normalized = 0, delta = 0, speed = 1.0f, maxDelta = 1.0f;
	u32 startTime = 0, endTime = 0, time = 0, length = 0, lastTime = 0;
};

class B_API Clock
{
public:
	Clock();
	virtual ~Clock();

	TimerId createTimer();
	//! delete a timer by its id (system timers cannot be deleted)
	bool deleteTimer(TimerId id);
	Timer* getTimer(TimerId id) const;
	//! \return the main timer, which holds the real time, unmodified by speeds or anything
	Timer& getMainTimer();
	//! update main timer and the other timers
	void update();

protected:
	void deleteAllTimers();

	TimerId lastId = 0;
	Timer mainTimer;
	Dictionary<TimerId, Timer*> timers;
};

}