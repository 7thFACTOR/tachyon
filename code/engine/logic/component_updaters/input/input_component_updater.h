// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/array.h"
#include "base/dictionary.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/observer.h"
#include "base/math/ray.h"
#include "logic/component.h"
#include "logic/component_updater.h"
#include "graphics/graphics.h"
#include "logic/component_pool.h"
#include "base/math/smoother.h"

namespace engine
{
class Logic;

class InputComponentUpdater : public ComponentUpdater
{
public:
	InputComponentUpdater();
	virtual ~InputComponentUpdater();
	ComponentUpdaterId getId() const override { return StdComponentUpdaterId_Input; }
	void update(f32 deltaTime) override;

protected:
	void updateFlyBy(f32 deltaTime);

	bool mouseWasCentered = false;
	Smoother<Vec2> rotationSmoother;
};

}