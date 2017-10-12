// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "sound_buffer.h"
#include "base/string.h"
#include "base/assert.h"
#include "SFML/Audio/SoundBuffer.hpp"

namespace engine
{
SoundBuffer::SoundBuffer()
{
	apiSoundBuffer = new sf::SoundBuffer();
}

SoundBuffer::~SoundBuffer()
{
	delete (sf::SoundBuffer*)apiSoundBuffer;
}

bool SoundBuffer::loadFromFile(const String& filename)
{
	B_ASSERT(apiSoundBuffer);
	return ((sf::SoundBuffer*)apiSoundBuffer)->loadFromFile(filename.c_str());
}

bool SoundBuffer::loadFromMemory(const void* data, size_t sizeInBytes)
{
	B_ASSERT(apiSoundBuffer);
	return ((sf::SoundBuffer*)apiSoundBuffer)->loadFromMemory(data, sizeInBytes);
}

bool SoundBuffer::loadFromSamples(
	const i16* samples,
	u64 sampleCount,
	u32 channelCount,
	u32 sampleRate)
{
	B_ASSERT(apiSoundBuffer);
	return ((sf::SoundBuffer*)apiSoundBuffer)->loadFromSamples(samples, sampleCount, channelCount, sampleRate);
}

bool SoundBuffer::saveToFile(const String& filename) const
{
	B_ASSERT(apiSoundBuffer);
	return ((sf::SoundBuffer*)apiSoundBuffer)->saveToFile(filename.c_str());
}

const i16* SoundBuffer::getSamples() const
{
	B_ASSERT(apiSoundBuffer);
	return ((sf::SoundBuffer*)apiSoundBuffer)->getSamples();
}

u64 SoundBuffer::getSampleCount() const
{
	B_ASSERT(apiSoundBuffer);
	return ((sf::SoundBuffer*)apiSoundBuffer)->getSampleCount();
}

u32 SoundBuffer::getSampleRate() const
{
	B_ASSERT(apiSoundBuffer);
	return ((sf::SoundBuffer*)apiSoundBuffer)->getSampleRate();
}

u32 SoundBuffer::getChannelCount() const
{
	B_ASSERT(apiSoundBuffer);
	return ((sf::SoundBuffer*)apiSoundBuffer)->getChannelCount();
}

u32 SoundBuffer::getDuration() const
{
	B_ASSERT(apiSoundBuffer);
	return ((sf::SoundBuffer*)apiSoundBuffer)->getDuration().asMilliseconds();
}

}