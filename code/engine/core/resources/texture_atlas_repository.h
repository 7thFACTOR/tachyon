// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "core/resource_type_repository.h"
#include "core/resources/texture_atlas_resource.h"

namespace engine
{
class E_API TextureAtlasRepository : public ResourceTypeRepositoryTpl<TextureAtlasResource, ResourceType::TextureAtlas>
{
public:
	bool load(Stream& stream, ResourceId resId);
	void unload(ResourceId resId);
};

}