// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/array.h"
#include "base/dictionary.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/observer.h"
#include "base/math/ray.h"
#include "logic/component.h"
#include "logic/component_system.h"
#include "graphics/graphics.h"
#include "logic/component_pool.h"
#include "base/math/smoother.h"

namespace engine
{
class Logic;

class InputComponentSystem : public ComponentSystem
{
public:
	InputComponentSystem();
	virtual ~InputComponentSystem();
	ComponentSystemId getId() const override { return StdComponentSystemId_Input; }
	void update(f32 deltaTime) override;

protected:
	void updateFlyBy(f32 deltaTime);

	bool mouseWasCentered = false;
	Smoother<Vec2> rotationSmoother;
};

}