#pragma once

namespace engine
{
struct Entity;

struct GameObserver
{
	virtual void onEntityCreated(Entity* entity) {}
	virtual void onEntityRemoved(Entity* entity) {}
	virtual void onEntityParentChange(Entity* entity, Entity* oldParent, Entity* newParent) {}
	virtual void onComponentAdded(Entity* entity, Component* component) {}
	virtual void onComponentRemoved(Entity* entity, Component* component) {}
}

}