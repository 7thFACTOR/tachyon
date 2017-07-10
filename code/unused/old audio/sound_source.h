#pragma once
#include "base/types.h"
#include "base/math/vec3.h"
#include "core/defines.h"
#include "core/types.h"
#include "audio/resources/sound_resource.h"

namespace engine
{
using namespace base;

struct SoundOutputLevels
{
	f32 left = 0.0f;
	f32 right = 0.0f;
};

class SoundSource
{
public:
	virtual ~SoundSource() {}

	virtual void loadFromResource(ResourceId resId) = 0;

	virtual void play() = 0;
	virtual void stop() = 0;
	virtual void pause() = 0;
	virtual void resume() = 0;

	virtual void setVolume(f32 value) = 0;
	virtual void setCurrentTime(f32 value) = 0;
	virtual void setFrequency(f32 value) = 0;
	virtual void setPan(f32 value) = 0;
	virtual void setLoopMode(SoundLoopMode value) = 0;
	virtual void setPitch(f32 value) = 0;
	virtual void setVelocity(const Vec3& velocity) = 0;
	virtual void setTranslation(const Vec3& value) = 0;
	virtual void setPriority(i32 value) = 0;
	virtual void setRange(f32 minDistance, f32 maxDistance) = 0;
	virtual void setMode(SoundMode mode) = 0;

	virtual f32 getCurrentTime() const = 0;
	virtual bool isPlaying() const = 0;
};

}