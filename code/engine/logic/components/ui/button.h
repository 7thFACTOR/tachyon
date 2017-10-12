// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "core/defines.h"
#include "core/types.h"
#include "logic/components/ui/font.h"
#include "core/types.h"
#include "logic/component.h"

namespace engine
{
using namespace base;

struct ButtonComponent : Component
{
	static const ComponentTypeId typeId = (ComponentTypeId)StdComponentTypeId::Button;

	//Sprite
	Color color = Color::white;
	ResourceId material = nullResourceId;
}

}