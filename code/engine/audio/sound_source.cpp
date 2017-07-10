#include "sound_source.h"
#include "sound_buffer.h"
#include "SFML/Audio/SoundSource.hpp"
#include "SFML/Audio/SoundBuffer.hpp"
#include "SFML/Audio/Sound.hpp"
#include "base/assert.h"

namespace engine
{
SoundSource::SoundSource()
{
	apiSoundSource = new sf::Sound();
}

SoundSource::~SoundSource()
{
	delete (sf::Sound*)apiSoundSource;
}

SoundSource::SoundSource(SoundBuffer* buffer)
{
	soundBuffer = buffer;
}

void SoundSource::play()
{
	B_ASSERT(apiSoundSource);
	((sf::Sound*)apiSoundSource)->play();
}

void SoundSource::pause()
{
	B_ASSERT(apiSoundSource);
	((sf::Sound*)apiSoundSource)->pause();
}

void SoundSource::stop()
{
	B_ASSERT(apiSoundSource);
	((sf::Sound*)apiSoundSource)->stop();
}

void SoundSource::setBuffer(SoundBuffer* buffer)
{
	B_ASSERT(apiSoundSource);
	B_ASSERT(buffer);
	B_ASSERT(buffer->getApiSoundBuffer());
	soundBuffer = buffer;
	((sf::Sound*)apiSoundSource)->setBuffer(*(sf::SoundBuffer*)buffer->getApiSoundBuffer());
}

void SoundSource::setLoop(bool loop)
{
	B_ASSERT(apiSoundSource);
	((sf::Sound*)apiSoundSource)->setLoop(loop);
}

void SoundSource::setPlayingOffset(u32 timeOffset)
{
	B_ASSERT(apiSoundSource);
	((sf::Sound*)apiSoundSource)->setPlayingOffset(sf::milliseconds(timeOffset));
}

bool SoundSource::getLoop() const
{
	B_ASSERT(apiSoundSource);
	return ((sf::Sound*)apiSoundSource)->getLoop();
}

u32 SoundSource::getPlayingOffset() const
{
	B_ASSERT(apiSoundSource);
	return ((sf::Sound*)apiSoundSource)->getPlayingOffset().asMilliseconds();
}

SoundStatus SoundSource::getStatus() const
{
	B_ASSERT(apiSoundSource);
	return (SoundStatus)((sf::Sound*)apiSoundSource)->getStatus();
}

void SoundSource::setPitch(f32 pitch)
{
	B_ASSERT(apiSoundSource);
	((sf::Sound*)apiSoundSource)->setPitch(pitch);
}

void SoundSource::setVolume(f32 volume)
{
	B_ASSERT(apiSoundSource);
	((sf::Sound*)apiSoundSource)->setVolume(volume);
}

void SoundSource::setPosition(const Vec3& position)
{
	B_ASSERT(apiSoundSource);
	((sf::Sound*)apiSoundSource)->setPosition(position.x, position.y, position.z);
}

void SoundSource::setRelativeToListener(bool relative)
{
	B_ASSERT(apiSoundSource);
	((sf::Sound*)apiSoundSource)->setRelativeToListener(relative);
}

void SoundSource::setMinDistance(f32 distance)
{
	B_ASSERT(apiSoundSource);
	((sf::Sound*)apiSoundSource)->setMinDistance(distance);
}

void SoundSource::setAttenuation(f32 attenuation)
{
	B_ASSERT(apiSoundSource);
	((sf::Sound*)apiSoundSource)->setAttenuation(attenuation);
}

f32 SoundSource::getPitch() const
{
	B_ASSERT(apiSoundSource);
	return ((sf::Sound*)apiSoundSource)->getPitch();
}

f32 SoundSource::getVolume() const
{
	B_ASSERT(apiSoundSource);
	return ((sf::Sound*)apiSoundSource)->getVolume();
}

Vec3 SoundSource::getPosition() const
{
	B_ASSERT(apiSoundSource);
	auto pos = ((sf::Sound*)apiSoundSource)->getPosition();

	return { pos.x, pos.y, pos.z };
}

bool SoundSource::isRelativeToListener() const
{
	B_ASSERT(apiSoundSource);
	return ((sf::Sound*)apiSoundSource)->isRelativeToListener();
}

f32 SoundSource::getMinDistance() const
{
	B_ASSERT(apiSoundSource);
	return ((sf::Sound*)apiSoundSource)->getMinDistance();
}

f32 SoundSource::getAttenuation() const
{
	B_ASSERT(apiSoundSource);
	return ((sf::Sound*)apiSoundSource)->getAttenuation();
}

}