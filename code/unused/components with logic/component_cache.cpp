#include "base/assert.h"
#include "core/core.h"
#include "base/logger.h"
#include "game/component.h"
#include "game/game_manager.h"
#include "game/component_cache.h"

namespace engine
{
EntityComponentsCache::EntityComponentsCache()
{}

EntityComponentsCache::~EntityComponentsCache()
{}

void EntityComponentsCache::addEntity(EntityId entity)
{
	getGame().findEntityComponents(entity, entityComponents[entity]);
}

void EntityComponentsCache::removeEntity(EntityId entity)
{
	auto iter = entityComponents.find(entity);

	if (iter == entityComponents.end())
	{
		return;
	}

	entityComponents.erase(iter);
}

void EntityComponentsCache::removeAllEntities()
{
	entityComponents.clear();
}

Array<Component*>& EntityComponentsCache::getEntityComponents(EntityId entity)
{
	return entityComponents[entity];
}

}