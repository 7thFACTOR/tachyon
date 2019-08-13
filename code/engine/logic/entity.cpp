// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "logic/entity.h"
#include "core/globals.h"
#include "logic/logic.h"

namespace engine
{
Component* Entity::addComponent(ComponentTypeId type)
{


	return (Component*)getLogic().addComponentToEntity(type, this);
}

void Entity::removeComponent(ComponentTypeId type)
{

}

}