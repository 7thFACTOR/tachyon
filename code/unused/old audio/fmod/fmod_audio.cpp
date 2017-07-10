#include "fmod_common.h"
#include "audio/fmod/fmod_audio.h"
#include "audio/fmod/fmod_sound_source.h"
#include "game/components/rendering/camera.h"
#include "base/logger.h"
#include "core/globals.h"

namespace engine
{
B_DEFINE_CLASS(FmodAudio);

const u32 softwareChannelCount = 32; //TODO: too fixed ?
const u32 maxAudioChannels = 128; //TODO: ehm? why?

FMOD::System* FmodAudio::fmodSystem = nullptr;

FmodAudio::FmodAudio()
	: Audio()
{
	FMOD_RESULT res;

	fmodSystem = nullptr;
	res = FMOD::System_Create(&fmodSystem);
	checkFmodError(res);

	B_ASSERT(fmodSystem);

	if (!fmodSystem)
		return;

	unsigned int version;
	res = fmodSystem->getVersion(&version);
	checkFmodError(res);

	B_ASSERT(version != FMOD_VERSION);

	if (version != FMOD_VERSION)
	{
		B_LOG_ERROR("Incorrect FMOD dll version! Fmod module: " << FMOD_VERSION << ", FMOD DLL's version: " << version);

		return;
	}

	res = fmodSystem->setSoftwareChannels(softwareChannelCount);
	checkFmodError(res);

	int ndrv;
	fmodSystem->getDriver(&ndrv);
	checkFmodError(res);

	res = fmodSystem->init(maxAudioChannels, FMOD_INIT_NORMAL, 0);
	checkFmodError(res);
	res = fmodSystem->set3DSettings(1, 1, 1);

	setSpectatorParameters(AudioSpectatorParameters());

	B_LOG_SUCCESS("Done initializing FMOD");
}

FmodAudio::~FmodAudio()
{
	FMOD_RESULT res;

	if (fmodSystem)
	{
		res = fmodSystem->close();
		checkFmodError(res);
		res = fmodSystem->release();
		checkFmodError(res);
	}
}

SoundSource* FmodAudio::createSoundSource()
{
	FmodSoundSource* sndSrc = new FmodSoundSource();
	
	B_ASSERT(sndSrc);
	
	return sndSrc;
}

void FmodAudio::setSpectatorParameters(const AudioSpectatorParameters& params)
{
	if (fmodSystem)
	{
		FMOD_VECTOR vecPos, vecVel, vecForward, vecTop;

		vecPos.x = params.position.x;
		vecPos.y = params.position.y;
		vecPos.z = params.position.z;
		vecVel.x = params.velocity.x;
		vecVel.y = params.velocity.y;
		vecVel.z = params.velocity.z;
		vecForward.x = params.forwardDirection.x;
		vecForward.y = params.forwardDirection.y;
		vecForward.z = params.forwardDirection.z;
		vecTop.x = params.upDirection.x;
		vecTop.y = params.upDirection.y;
		vecTop.z = params.upDirection.z;
		fmodSystem->set3DListenerAttributes(0, &vecPos, &vecVel, &vecForward, &vecTop);
	}
}

void FmodAudio::setAudioSettings(const AudioSettings& settings)
{
	fmodSystem->setDriver(settings.currentDriver);
	fmodSystem->setOutput(FMOD_OUTPUTTYPE_AUTODETECT);
}

void FmodAudio::update()
{
	if (!fmodSystem)
	{
		return;
	}
	
	fmodSystem->update();
}

}