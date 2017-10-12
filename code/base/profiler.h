// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/array.h"
#include "base/dictionary.h"
#include "base/string.h"
#include "base/defines.h"
#include "base/types.h"

namespace base
{
struct ProfilerSection
{
	u64 id = 0;
	String name;
	f64 lastTime = 0.0f;
	f64 totalTime = 0.0f;
	f64 deltaTime = 0.0f;
	f64 peakTime = 0.0f;
	f64 averageTime = 0.0f;
	f64 percentTimeFromParent = 0.0f;
	f64 maxValueShownOnGraph = 1.0f;
	ProfilerSection* parent = nullptr;
	Array<f64> graphTimings;
	Array<ProfilerSection*> children;
	u64 executeCount = 0;
	bool hasGraph = false;
	bool ended = false;
	f32 timingPeekTimeSec = 0.02f;
	f32 timingPeekTimer = 0.0f;
};

class B_API Profiler
{
public:
	Profiler();
	~Profiler();

	const Dictionary<u64, ProfilerSection*>& getSections() const { return sections; }
	ProfilerSection* getSection(u64 id);
	ProfilerSection* getSection(const String& name);
	ProfilerSection* registerSection(u64 id, const String& name);
	void deleteSection(u64 id);
	void deleteSection(ProfilerSection* section);
	void begin(ProfilerSection* section);
	void end(ProfilerSection* section);
	void reset(ProfilerSection* section);
	void computeSectionsPercentTime();

protected:
	void free();

	Dictionary<u64, ProfilerSection*> sections;
	ProfilerSection* currentParent;
};

#if defined(B_USE_PERFORMANCE_PROFILER)
	#define B_BEGIN_PROFILE_SECTION(profiler, sectionName)\
		base::ProfilerSection* profile##sectionName =\
			profiler.registerSection(\
				(u64)#sectionName, #sectionName); // use the static string address as id

	#define B_END_PROFILE_SECTION(profiler, sectionName)\
		if(profile##sectionName)\
			profiler.end(profile##sectionName);
#else
	#define B_BEGIN_PROFILE_SECTION(profiler, sectionName)
	#define B_END_PROFILE_SECTION(profiler, sectionName)
#endif

}