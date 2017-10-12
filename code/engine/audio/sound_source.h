// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "core/types.h"
#include "core/defines.h"
#include "audio/types.h"

namespace engine
{
class SoundBuffer;

class E_API SoundSource
{
public:
	SoundSource();
	SoundSource(SoundBuffer* buffer);
	~SoundSource();
	void play();
	void pause();
	void stop();
	void setBuffer(SoundBuffer* buffer);
	void setLoop(bool loop);
	void setPlayingOffset(u32 timeOffset);
	const SoundBuffer* getBuffer() const { return soundBuffer; }
	bool getLoop() const;
	u32 getPlayingOffset() const;
	SoundStatus getStatus() const;
	void setPitch(f32 pitch);
	void setVolume(f32 volume);
	void setPosition(const Vec3& position);
	void setRelativeToListener(bool relative);
	void setMinDistance(f32 distance);
	void setAttenuation(f32 attenuation);
	f32 getPitch() const;
	f32 getVolume() const;
	Vec3 getPosition() const;
	bool isRelativeToListener() const;
	f32 getMinDistance() const;
	f32 getAttenuation() const;

private:
	class SoundBuffer* soundBuffer = nullptr; //!< sound buffer bound to the source
	void* apiSoundSource = nullptr;
};

}