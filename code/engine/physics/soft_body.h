// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/class_registry.h"
#include "logic/components/render/transform.h"

namespace engine
{
//TODO: make it
class E_API PhysicsSoftBody
{
public:
	virtual ~PhysicsSoftBody();

protected:
	PhysicsSoftBody();
};

}