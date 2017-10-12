// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/string.h"
#include "base/array.h"
#include "base/dictionary.h"
#include "base/defines.h"
#include "base/types.h"
#include "logic/component.h"
#include "logic/component_updater.h"
#include "core/types.h"
#include "logic/types.h"

namespace engine
{
class Logic;
class World;

class AudioComponentUpdater : public ComponentUpdater
{
public:
	ComponentUpdaterId getId() const override { return StdComponentUpdaterId_Audio; }
	void update(f32 deltaTime) override;
};

}