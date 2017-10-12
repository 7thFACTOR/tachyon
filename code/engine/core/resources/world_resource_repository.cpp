// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "core/resources/world_resource_repository.h"
#include "base/defines.h"
#include "base/assert.h"
#include "base/logger.h"

namespace engine
{
bool WorldResourceRepository::load(Stream& stream, ResourceId resId)
{
	WorldResource& world = *resources[resId];

	stream | world.tagNames;
	world.rootEntityData.streamData(stream);
	
	return true;
}

void WorldResourceRepository::unload(ResourceId resId)
{
	WorldResource& world = *resources[resId];

	world.rootEntityData.deleteComponentData();
	world.rootEntityData.children.clear();
}

}