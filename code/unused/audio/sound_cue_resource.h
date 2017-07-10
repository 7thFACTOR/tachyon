#pragma once
#include "base/defines.h"
#include "core/defines.h"
#include "core/resource.h"

namespace engine
{
enum class SoundCueModifierType
{
	Loop,
	Delay,
	Attenuate,
	Doppler,
	Envelope,
	Join,
	Mixer,
	Modulate,
	
	Count
};

struct SoundCueModifier
{
	SoundCueModifierType type;
	u32 id;
};

struct SoundCueSlotLink
{
	u32 outModifierId;
	u32 inModifierId;
	u32 slotIndexOut;
	u32 slotIndexIn;
};

//! Holds the actual sound data
struct SoundCueResource
{
	size_t modifierCount;
	SoundCueModifier* modifiers;
};

}