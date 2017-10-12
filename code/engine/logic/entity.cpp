// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "logic/entity.h"
#include "core/globals.h"
#include "logic/logic.h"

namespace engine
{
Component* Entity::addComponent(Component* component)
{
	components[component->type] = component;
	component->entity = this;

	return component;
}

Component* Entity::addComponent(ComponentTypeId type)
{
	return (Component*)getLogic().addComponentToEntity(type, this);
}

}