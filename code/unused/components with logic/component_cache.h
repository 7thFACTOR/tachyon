#pragma once
#include "base/string.h"
#include "base/defines.h"
#include "game/types.h"

namespace engine
{
class ENGINE_API EntityComponentsCache
{
public:
	EntityComponentsCache();
	virtual ~EntityComponentsCache();
	void addEntity(EntityId entity);
	void removeEntity(EntityId entity);
	void removeAllEntities();
	Array<Component*>& getEntityComponents(EntityId entity);

protected:
	Map<EntityId, Array<Component*> > entityComponents;
};

}