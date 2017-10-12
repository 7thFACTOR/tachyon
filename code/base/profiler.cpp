// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "base/profiler.h"
#include "base/platform.h"
#include "base/assert.h"
#include <math.h>
#include "base/logger.h"

namespace base
{
const u32 maxGraphTimingsBufferSize = 500;

Profiler::Profiler()
{
	currentParent = nullptr;
}

Profiler::~Profiler()
{
	free();
}

ProfilerSection* Profiler::getSection(u64 id)
{
	return sections[id];
}

ProfilerSection* Profiler::getSection(const String& name)
{
	for (size_t i = 0; i < sections.size(); ++i)
	{
		if (sections.valueAt(i)->name == name)
		{
			return sections.valueAt(i);
		}
	}

	return nullptr;
}

ProfilerSection* Profiler::registerSection(u64 id, const String& name)
{
	size_t index = sections.findIndex(id);

	if (index == -1)
	{
		ProfilerSection* section;

		section = new ProfilerSection();
		sections[id] = section;
		B_ASSERT(section);

		if (!section)
			return nullptr;

		section->id = id;
		section->name = name;
		section->parent = currentParent;
		section->lastTime = getTimeMicroseconds();
		section->graphTimings.reserve(maxGraphTimingsBufferSize);

		if (currentParent)
		{
			currentParent->children.append(section);
		}

		begin(section);

		return section;
	}
	else
	{
		begin(sections[id]);
	}

	return sections[id];
}

void Profiler::deleteSection(u64 id)
{
	if (sections.findIndex(id) != -1)
	{
		delete sections[id];
		sections.erase(id);
	}
}

void Profiler::deleteSection(ProfilerSection* section)
{
	B_ASSERT(section);

	for (size_t i = 0, iCount = sections.size(); i < iCount; ++i)
	{
		if (sections.valueAt(i) == section)
		{
			delete section;
			sections.erase(sections.keyAt(i));
			break;
		}
	}
}

void Profiler::free()
{
	for (size_t i = 0; i < sections.size(); ++i)
	{
		delete sections.valueAt(i);
	}

	sections.clear();
}

void Profiler::begin(ProfilerSection* section)
{
	section->lastTime = getTimeMicroseconds();
	section->ended = false;
	++section->executeCount;
	//TODO:
	//pParent = profiler().m_pCurrentParent;
	//profiler().m_pCurrentParent = this;
}

void Profiler::end(ProfilerSection* section)
{
	section->deltaTime = getTimeMicroseconds() - section->lastTime;
	section->totalTime += fabsf(section->deltaTime);

	if (section->peakTime < section->deltaTime)
		section->peakTime = section->deltaTime;

	if (section->averageTime <= 0.000000001f)
		section->averageTime = section->deltaTime;
	else
		section->averageTime = (section->averageTime + section->deltaTime) / 2.0f;

	//TODO
	//timingPeekTimer += gameDeltaTime();

	// save value in graph
	if (section->hasGraph && section->timingPeekTimer > section->timingPeekTimeSec)
	{
		if (maxGraphTimingsBufferSize < section->graphTimings.size())
			section->graphTimings.erase(section->graphTimings.begin());

		section->graphTimings.append(section->deltaTime);
		section->timingPeekTimer = 0;
	}

	section->ended = true;
	//TODO: profiler().m_pCurrentParent = pParent;
}

void Profiler::reset(ProfilerSection* section)
{
	section->executeCount = 0;
	section->graphTimings.clear();
	section->timingPeekTimer = 0;
	section->peakTime = 0;
	section->averageTime = 0;
	section->lastTime = getTimeMicroseconds();
	section->totalTime = 0;
}

void computePercentTimeForSection(ProfilerSection* section)
{
	if (section->parent)
	{
		if (section->parent->totalTime < 0.0001f
			|| section->totalTime < 0.0001f)
		{
			section->percentTimeFromParent = 0.0f;
		}
		else
		{
			section->percentTimeFromParent = (f32) section->totalTime / section->parent->totalTime;
		}
	}

	for (size_t i = 0, iCount = section->children.size(); i < iCount; ++i)
	{
		computePercentTimeForSection(section->children[i]);
	}
}

void Profiler::computeSectionsPercentTime()
{
	for (size_t i = 0; i < sections.size(); ++i)
	{
		if (!sections.valueAt(i)->parent)
		{
			computePercentTimeForSection(sections.valueAt(i));
		}
	}
}

}