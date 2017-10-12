// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/string.h"
#include "base/defines.h"
#include "base/array.h"
#include "logic/types.h"

namespace engine
{
using namespace base;

class Entity;

struct EntityGroup
{
	String name;
	bool visible = true;
	Array<Entity*> entities;

	void addEntity(Entity* entity);
	void removeEntity(Entity* entity);
};

}