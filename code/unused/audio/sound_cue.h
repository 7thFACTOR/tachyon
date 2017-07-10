#pragma once 
#include "base/types.h"
#include "core/defines.h"
#include "core/types.h"
#include "base/math/util.h"

namespace engine
{
using namespace base;

class ENGINE_API SoundCue
{
public:
	SoundCue()
	{
		nextSoundIndex = 0;
		randomize = false;
	}

	ResourceId pickNewSound()
	{
		u32 index = 0;

		if (randomize)
		{
			index = base::randomInteger((u32)sounds.size());
		}
		else
		{
			index = nextSoundIndex++;

			if (nextSoundIndex >= sounds.size())
			{
				nextSoundIndex = 0;
				index = 0;
			}
		}

		return sounds[index];
	}

protected:
	Array<ResourceId> sounds;
	bool randomize;
	u32 nextSoundIndex;
};

}