// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "core/resource_type_repository.h"
#include "core/resources/world_resource.h"

namespace engine
{
class E_API WorldResourceRepository : public ResourceTypeRepositoryTpl<WorldResource, ResourceType::World>
{
public:
	bool load(Stream& stream, ResourceId resId);
	void unload(ResourceId resId);
};

}