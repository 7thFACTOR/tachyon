// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/dictionary.h"
#include "base/array.h"
#include "base/string.h"
#include "core/defines.h"
#include "core/types.h"
#include "logic/types.h"

namespace engine
{
using namespace base;

class Entity;
struct EntityGroup;

class E_API World
{
public:
	World();
	~World();

	Entity* createEntity(const String& name = "");
	Entity* createEntityFromPrefab(ResourceId prefabId, const String& name = "");
	bool createWorldFromResource(ResourceId worldId, bool additive = false);
	void deleteEntity(Entity* entity);
	void deleteEntities();
	void addEntityGroup(EntityGroup* group);
	void removeEntityGroup(EntityGroup* group);
	void deleteGroups();
	const Array<Entity*>& getEntities() const { return entities; }
	const Array<EntityGroup*>& getEntityGroups() const { return entityGroups; }

protected:
	Entity* createEntityRecursive(class Entity* parent, struct EntityData& entityData);

	Array<Entity*> entities;
	Array<EntityGroup*> entityGroups;
	u32 lastNewEntityCounter = 1;
};

}