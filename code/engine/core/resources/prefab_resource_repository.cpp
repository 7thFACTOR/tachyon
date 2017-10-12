// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "core/resources/prefab_resource_repository.h"
#include "base/defines.h"
#include "base/assert.h"
#include "base/logger.h"

namespace engine
{
bool PrefabResourceRepository::load(Stream& stream, ResourceId resId)
{
	PrefabResource& prefabRes = *resources[resId];

	stream >> prefabRes.tagNames;
	prefabRes.prefabRoot.streamData(stream);

	return true;
}

void PrefabResourceRepository::unload(ResourceId resId)
{
	PrefabResource& prefabRes = *resources[resId];

	prefabRes.prefabRoot.deleteComponentData();
	prefabRes.prefabRoot.children.clear();
	prefabRes.prefabRoot.tags.clear();
}

}