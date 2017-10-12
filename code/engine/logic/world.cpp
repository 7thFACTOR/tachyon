// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "base/memory_stream.h"
#include "base/assert.h"
#include "core/globals.h"
#include "logic/world.h"
#include "logic/entity.h"
#include "core/resource_repository.h"
#include "core/resources/prefab_resource.h"
#include "core/resources/world_resource.h"
#include "logic/logic.h"

namespace engine
{
static int initialEntityCountReserve = 5000;

World::World()
{
	entities.reserve(initialEntityCountReserve);
}

World::~World()
{}

Entity* World::createEntity(const String& name)
{
	Entity* entity = new Entity();

	B_ASSERT(entity);

	if (name.isEmpty())
	{
		String numberedName = String("Entity") << lastNewEntityCounter++;
		entity->name = numberedName;
	}

	entities.append(entity);

	return entity;
}

Entity* World::createEntityRecursive(Entity* parent, EntityData& entityData)
{
	Entity* entity = createEntity(entityData.name);
	
	entity->active = entityData.active;
	entity->name = entityData.name;
	entity->prefabId = entityData.prefabId;
	entity->parent = parent;

	//TODO: apply the properties after
	// expose the exposable properties

	// add components
	for (size_t i = 0; i < entityData.components.size(); i++)
	{
		auto& comp = entityData.components[i];
		auto component = getLogic().addComponentToEntity((ComponentTypeId)comp.typeId, entity);

		if (component)
		{
			MemoryStream memStream(comp.data, comp.dataSize);
			component->streamData(memStream);
		}
		else
		{
			B_LOG_DEBUG("Cannot create entity of type " << comp.typeId);
		}
	}

	// add children
	for (size_t i = 0; i < entityData.children.size(); i++)
	{
		auto& child = entityData.children[i];
		auto childEntity = createEntityRecursive(entity, child);
	}

	return entity;
}

Entity* World::createEntityFromPrefab(ResourceId prefabId, const String& name)
{
	ScopedResourceMapping<PrefabResource> prefabRes(prefabId);
	Entity* entity = nullptr;

	if (prefabRes)
	{
		entity = createEntityRecursive(nullptr, prefabRes->prefabRoot);
		
		if (name.notEmpty())
			entity->name = name;
	}

	return entity;
}

bool World::createWorldFromResource(ResourceId worldId, bool additive)
{
	if (!additive)
	{
		deleteEntities();
		deleteGroups();
	}

	ScopedResourceMapping<WorldResource> worldRes(worldId);

	if (!worldRes)
	{
		return false;
	}

	auto ent = createEntityRecursive(nullptr, worldRes->rootEntityData);

	return ent != nullptr;
}

void World::deleteEntity(Entity* entity)
{
	B_ASSERT(entity);
	delete entity;
	entities.erase(entity);
}

void World::deleteEntities()
{
	for (auto entity : entities)
	{
		deleteEntity(entity);
		delete entity;
	}

	entities.clear();
}

void World::addEntityGroup(EntityGroup* group)
{
	B_ASSERT(group);
	entityGroups.append(group);
}

void World::removeEntityGroup(EntityGroup* group)
{
	B_ASSERT(group);
	entityGroups.erase(group);
}

void World::deleteGroups()
{
	for (auto group : entityGroups)
	{
		removeEntityGroup(group);
		delete group;
	}

	entityGroups.clear();
}

}