#include "entity_group.h"
#include "entity.h"

namespace engine
{
using namespace base;

void EntityGroup::addEntity(Entity* entity)
{
	entity->group = this;
	entities.append(entity);
}

void EntityGroup::removeEntity(Entity* entity)
{
	entity->group = nullptr;
	entities.erase(entity);
}

}