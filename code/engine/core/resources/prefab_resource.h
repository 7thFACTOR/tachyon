// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/string.h"
#include "base/defines.h"
#include "core/resource.h"
#include "logic/types.h"
#include "core/resources/world_resource.h"

namespace engine
{
using namespace base;

struct PrefabResource : Resource
{
	Array<String> tagNames;
	EntityData prefabRoot;
};

}