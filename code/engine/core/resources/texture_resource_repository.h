#pragma once
#include "core/resource_type_repository.h"
#include "core/resources/texture_resource.h"

namespace engine
{
class E_API TextureResourceRepository : public ResourceTypeRepositoryTpl<TextureResource, ResourceType::Texture>
{
public:
	bool load(Stream& stream, ResourceId resId);
	void unload(ResourceId resId);
};

}