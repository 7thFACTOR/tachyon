// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/string.h"
#include "base/array.h"
#include "base/dictionary.h"
#include "base/defines.h"
#include "base/types.h"
#include "logic/component.h"
#include "logic/component_system.h"
#include "core/types.h"
#include "logic/types.h"

namespace engine
{
class Logic;
class World;

class AudioComponentSystem : public ComponentSystem
{
public:
	ComponentSystemId getId() const override { return StdComponentSystemId_Audio; }
	void update(f32 deltaTime) override;
};

}