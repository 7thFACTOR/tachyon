// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/string.h"
#include "base/array.h"
#include "base/dictionary.h"
#include "base/defines.h"
#include "base/types.h"
#include "logic/component.h"
#include "logic/types.h"

namespace engine
{
using namespace base;
class SoundSource;

struct AmbientMusicComponent : Component
{
	static const ComponentTypeId typeId = StdComponentTypeId_AmbientMusic;

	Dictionary<String, SoundSource*> tracks;
	f32 trackCrossFadeTimer;
	f32 trackCrossFadeSpeed;
	bool crossFadingTracks;
	String currentTrack, nextTrack;
};

}