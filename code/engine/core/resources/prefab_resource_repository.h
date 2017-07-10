#pragma once
#include "core/resource_type_repository.h"
#include "core/resources/prefab_resource.h"

namespace engine
{
class E_API PrefabResourceRepository : public ResourceTypeRepositoryTpl<PrefabResource, ResourceType::Prefab>
{
public:
	bool load(Stream& stream, ResourceId resId);
	void unload(ResourceId resId);
};

}