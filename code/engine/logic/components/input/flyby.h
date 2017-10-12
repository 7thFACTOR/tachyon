// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/math/vec3.h"
#include "base/math/matrix.h"
#include "graphics/frustum.h"
#include "logic/component.h"
#include "core/resources/mesh_resource.h"

namespace engine
{
struct FlybyComponent : Component
{
	static const ComponentTypeId typeId = StdComponentTypeId_Flyby;

	FlybyComponent() {}

	f32 speed = 5.0f;
	f32 speedWarpScale = 2.0f;
	f32 acceleration = 0.8f;
	f32 currentSpeed = 0;
};

}