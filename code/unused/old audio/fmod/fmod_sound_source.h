#pragma once
#include "fmod_common.h"
#include "audio/resources/sound_resource.h"
#include "audio/sound_source.h"
#include "base/class_registry.h"

namespace engine
{
using namespace base;
class FmodAudio;

class FmodSoundSource : public SoundSource
{
public:
	FmodSoundSource();
	~FmodSoundSource();

	void loadFromResource(ResourceId resId) override;

	void play() override;
	void stop() override;
	void pause() override;
	void resume() override;

	void setVolume(f32 value) override;
	void setCurrentTime(f32 value) override;
	void setFrequency(f32 value) override;
	void setPan(f32 value) override;
	void setLoopMode(SoundLoopMode value) override;
	void setPitch(f32 value) override;
	void setVelocity(const Vec3& velocity) override;
	void setTranslation(const Vec3& value) override;
	void setPriority(i32 value) override;
	void setRange(f32 minDistance, f32 maxDistance) override;
	void setMode(SoundMode mode) override;

	f32 getCurrentTime() const override;
	bool isPlaying() const override;

protected:
	Vec3 velocity;
	Vec3 position;
	FMOD::Sound* fmodSound = nullptr;
	FMOD::Channel* fmodChannel = nullptr;
};

}