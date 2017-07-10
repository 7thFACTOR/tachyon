#include "logic/animation.h"
#include "base/logger.h"

namespace engine
{
Animation::Animation()
{}

Animation::~Animation()
{}

void Animation::update(f32 deltaTime)
{
	f32 oldTime = time;
	time += deltaTime * timeScale * direction;

	for (size_t i = 0, iCount = events.size(); i < iCount; ++i)
	{
		if (!events[i]->triggered)
		{
			if (events[i]->time >= oldTime && events[i]->time < time)
			{
				events[i]->onTrigger();
				events[i]->triggered = true;
			}
		}
	}

	if (time > duration)
	{
		for (size_t i = 0, iCount = events.size(); i < iCount; ++i)
		{
			events[i]->triggered = false;
		}

		if (loopMode == AnimationLoopMode::Once)
		{
			return;
		}

		if (loopMode == AnimationLoopMode::Repeat)
		{
			time = 0;
		}
		else if (loopMode == AnimationLoopMode::PingPong)
		{
			direction = -direction;

			if (time > duration)
			{
				time = duration;
			}
		}
	}

	if (time <= 0)
	{
		if (loopMode == AnimationLoopMode::PingPong)
		{
			for (size_t i = 0, iCount = events.size(); i < iCount; ++i)
			{
				events[i]->triggered = false;
			}

			direction = -direction;
			time = 0;
		}
	}
}

void Animation::addTrack(AnimationTrackBase* track)
{
	B_ASSERT(track);

	if (track)
	{
		tracks.append(track);
	}
}

void Animation::play()
{
	time = 0;

	for (size_t i = 0, iCount = events.size(); i < iCount; ++i)
	{
		events[i]->triggered = false;
	}

	for (size_t i = 0, iCount = tracks.size(); i < iCount; ++i)
	{
		tracks[i]->computeTangents();
	}

	if (autoDuration)
	{
		duration = 0;

		for (size_t i = 0, iCount = tracks.size(); i < iCount; ++i)
		{
			for (size_t j = 0, jCount = tracks[i]->keys.size(); j < jCount; ++j)
			{
				if (tracks[i]->keys[j]->time > duration)
				{
					duration = tracks[i]->keys[j]->time;
				}
			}
		}
	}
}

AnimationTrackBase* Animation::findTrack(const String& name) const
{
	for (auto& track : tracks)
	{
		if (track->name == name)
		{
			return track;
		}
	}

	return nullptr;
}

}