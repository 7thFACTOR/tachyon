#pragma once
#include "core/resource_type_repository.h"
#include "core/resources/material_resource.h"

namespace engine
{
class E_API MaterialResourceRepository : public ResourceTypeRepositoryTpl<MaterialResource, ResourceType::Material>
{
public:
	bool load(Stream& stream, ResourceId resId);
	void unload(ResourceId resId);
};

}