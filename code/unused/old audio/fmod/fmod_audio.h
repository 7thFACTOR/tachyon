#pragma once
#include "fmod_common.h"
#include "audio/audio.h"
#include "base/class_registry.h"
#include "core/defines.h"

namespace engine
{
class FmodSoundSource;

class FmodAudio : public Audio
{
public:
	B_DECLARE_DERIVED_CLASS(FmodAudio, Audio);
	FmodAudio();
	~FmodAudio();

	SoundSource* createSoundSource() override;
	void setSpectatorParameters(const AudioSpectatorParameters& params) override;
	void setAudioSettings(const AudioSettings& settings) override;
	void update() override;

	static FMOD::System* fmodSystem;
};

}