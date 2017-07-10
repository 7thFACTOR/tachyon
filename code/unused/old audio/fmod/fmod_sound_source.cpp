#include "fmod_common.h"
#include <string.h>
#include "base/file.h"
#include "core/globals.h"
#include "base/logger.h"
#include "base/assert.h"
#include "audio/fmod/fmod_audio.h"
#include "audio/fmod/fmod_sound_source.h"
#include "core/resource_repository.h"

namespace engine
{
FmodSoundSource::FmodSoundSource()
{
	fmodSound = nullptr;
	fmodChannel = nullptr;
}

FmodSoundSource::~FmodSoundSource()
{
	if (fmodSound)
	{
		stop();
		fmodSound->release();
	}
}

void FmodSoundSource::loadFromResource(ResourceId resId)
{
	B_ASSERT(fmodChannel);
	SoundResource* soundData = getResources().map<SoundResource>(resId);

	B_ASSERT(soundData);

	FMOD_RESULT res;
	FMOD_CREATESOUNDEXINFO exinfo;

	memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
	exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
	exinfo.length = soundData->dataSize;

	res = FmodAudio::fmodSystem->createSound(
		(const char*)soundData->data,
		FMOD_HARDWARE
		| FMOD_OPENMEMORY
		| (soundData->mode == SoundMode::Mode2D
		? FMOD_2D : (FMOD_3D | FMOD_3D_WORLDRELATIVE)),
		&exinfo,
		&fmodSound);
	checkFmodError(res);
}

void FmodSoundSource::play()
{
	if (fmodSound)
	{
		checkFmodError(FmodAudio::fmodSystem->playSound(
			FMOD_CHANNEL_FREE, fmodSound, true, &fmodChannel));
	}
}

f32 FmodSoundSource::getCurrentTime() const
{
	if (!fmodChannel)
		return 0;

	u32 pos = 0;

	fmodChannel->getPosition(&pos, FMOD_TIMEUNIT_MS);

	return (f32)pos / 1000.0f;
}

void FmodSoundSource::pause()
{
	B_ASSERT(fmodChannel);
	
	if (!fmodChannel)
		return;

	fmodChannel->setPaused(true);
}

void FmodSoundSource::resume()
{
	B_ASSERT(fmodChannel);

	if (!fmodChannel)
		return;

	fmodChannel->setPaused(false);
}

bool FmodSoundSource::isPlaying() const
{
	B_ASSERT(fmodChannel);

	if (!fmodChannel)
		return false;

	bool playing;
	
	fmodChannel->isPlaying(&playing);

	return playing;
}

void FmodSoundSource::setVolume(f32 value)
{
	B_ASSERT(fmodChannel);

	if (!fmodChannel)
		return;

	fmodChannel->setVolume(value);
}

void FmodSoundSource::setCurrentTime(f32 value)
{
	B_ASSERT(fmodChannel);
	
	if (!fmodChannel)
		return;

	fmodChannel->setPosition(value * 1000.0f, FMOD_TIMEUNIT_MS);
}

void FmodSoundSource::setFrequency(f32 value)
{
	B_ASSERT(fmodChannel);

	if (!fmodChannel)
		return;

	fmodChannel->setFrequency(value);
}
void FmodSoundSource::setPan(f32 value)
{
	B_ASSERT(fmodChannel);

	if (!fmodChannel)
		return;

	fmodChannel->setPan(value);
}

void FmodSoundSource::setLoopMode(SoundLoopMode value)
{
	B_ASSERT(fmodChannel);

	if (!fmodChannel)
		return;

	fmodChannel->setMode(value == SoundLoopMode::Repeat ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
}

void FmodSoundSource::stop()
{
	B_ASSERT(fmodChannel);

	if (!fmodChannel)
		return;

	fmodChannel->stop();
}

void FmodSoundSource::setVelocity(const Vec3& newVelocity)
{
	velocity = newVelocity;

	B_ASSERT(fmodChannel);

	if (!fmodChannel)
		return;

	FMOD_VECTOR vel, pos;

	vel = { velocity.x, velocity.y, velocity.z };
	pos = { position.x, position.y, position.z };

	fmodChannel->set3DAttributes(&pos, &vel);
}


void FmodSoundSource::setTranslation(const Vec3& newPosition)
{
	position = newPosition;

	B_ASSERT(fmodChannel);

	if (!fmodChannel)
		return;

	FMOD_VECTOR vel, pos;

	vel = { velocity.x, velocity.y, velocity.z };
	pos = { position.x, position.y, position.z };

	fmodChannel->set3DAttributes(&pos, &vel);
}

void FmodSoundSource::setPitch(f32 value)
{
	B_ASSERT(fmodChannel);

	if (!fmodChannel)
		return;

	fmodChannel->setFrequency(value);
}

void FmodSoundSource::setMode(SoundMode mode)
{
	B_ASSERT(fmodChannel);

	if (fmodSound && fmodChannel)
	{
		fmodSound->setMode(
						mode == SoundMode::Mode2D
						? FMOD_3D_HEADRELATIVE : FMOD_3D | FMOD_3D_WORLDRELATIVE);
		fmodChannel->setMode(
						mode == SoundMode::Mode2D
						? FMOD_3D_HEADRELATIVE : FMOD_3D | FMOD_3D_WORLDRELATIVE);
	}
}

void FmodSoundSource::setPriority(i32 value)
{
	B_ASSERT(fmodChannel);

	if (fmodChannel)
	{
		fmodChannel->setPriority(value);
	}
}

void FmodSoundSource::setRange(f32 minDistance, f32 maxDistance)
{
	B_ASSERT(fmodChannel);
	
	if (fmodChannel)
	{
		fmodChannel->set3DMinMaxDistance(minDistance, maxDistance);
	}
}

}