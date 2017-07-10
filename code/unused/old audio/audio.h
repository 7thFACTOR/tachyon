#pragma once
#include "base/defines.h"
#include "base/array.h"
#include "base/string.h"
#include "base/dictionary.h"
#include "base/types.h"
#include "base/math/vec3.h"
#include "core/defines.h"
#include "base/class_registry.h"
#include "audio/types.h"

namespace engine
{
using namespace base;

struct SoundListenerSettings;
struct SoundSource;

class Audio : public RuntimeClass
{
public:
	virtual ~Audio() {}
	virtual SoundSource* createSoundSource() = 0;
	virtual void setSpectatorParameters(const AudioSpectatorParameters& params) = 0;
	virtual void setAudioSettings(const AudioSettings& settings) = 0;
	virtual void update() = 0;
};

}