#pragma once
#include "core/resource_type_repository.h"
#include "core/resources/mesh_resource.h"

namespace engine
{
class E_API MeshResourceRepository : public ResourceTypeRepositoryTpl<MeshResource, ResourceType::Mesh>
{
public:
	bool load(Stream& stream, ResourceId resId);
	void unload(ResourceId resId);
};

}