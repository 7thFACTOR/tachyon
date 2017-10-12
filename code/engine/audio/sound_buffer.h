// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "core/defines.h"
#include "core/types.h"

namespace base
{
class String;
}

namespace engine
{
using namespace base;

class E_API SoundBuffer
{
public:
	SoundBuffer();
	~SoundBuffer();
	bool loadFromFile(const String& filename);
	bool loadFromMemory(const void* data, size_t sizeInBytes);
	bool loadFromSamples(const i16* samples, u64 sampleCount, u32 channelCount, u32 sampleRate);
	bool saveToFile(const String& filename) const;
	const i16* getSamples() const;
	u64 getSampleCount() const;
	u32 getSampleRate() const;
	u32 getChannelCount() const;
	u32 getDuration() const;
	void* getApiSoundBuffer() const { return apiSoundBuffer; }

private:
	void* apiSoundBuffer = nullptr;
};

}