// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "core/resource_type_repository.h"
#include "core/resources/animation_resource.h"

namespace engine
{
class E_API AnimationResourceRepository : public ResourceTypeRepositoryTpl<AnimationResource, ResourceType::Animation>
{
public:
	bool load(Stream& stream, ResourceId resId);
	void unload(ResourceId resId);
};

}