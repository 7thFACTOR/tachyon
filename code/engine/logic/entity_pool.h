// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/string.h"
#include "base/array.h"
#include "base/assert.h"
#include "logic/types.h"
#include "logic/entity.h"

namespace engine
{
using namespace base;

struct Component;
class Entity;

//! The component pool holds contiguous arrays of Component derived structures
class EntityPool
{
public:
	~EntityPool(){}
    void setArchetype(const EntityArchetype& arch);
	Entity* createInstance();
	bool deleteInstance(Component* component);
	size_t getInstanceCount() const;
	Entity* getInstanceAt(size_t index);
	void deleteAllInstances();
	void clearDirtyFlag();
};

}