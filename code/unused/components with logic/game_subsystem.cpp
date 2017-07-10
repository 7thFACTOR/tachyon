#include "game/game_system.h"
#include "game/component.h"

namespace engine
{
void GameSystem::onEntityCreated(EntityId entity)
{}

void GameSystem::onEntityRemoved(EntityId entity)
{}

void GameSystem::onEntityParentChange(EntityId entity, EntityId oldParent, EntityId newParent)
{}

void GameSystem::onComponentAdded(EntityId entity, Component* component)
{}

void GameSystem::onComponentRemoved(EntityId entity, Component* component)
{}

}