// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/class_registry.h"
#include "logic/components/render/transform.h"

namespace engine
{
class E_API PhysicsCloth
{
public:
	virtual ~PhysicsCloth();

	virtual bool load(const String& filename);

protected:
	// create this only through the PhysicsSpace
	PhysicsCloth();

	u8* clothData = nullptr;
};

}