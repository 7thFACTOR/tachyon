// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/string.h"
#include "base/dictionary.h"
#include "core/types.h"
#include "core/defines.h"
#include "logic/types.h"
#include "logic/component.h"
#include "logic/logic.h"
#include "core/resources/world_resource.h"

namespace engine
{
using namespace base;

const u32 maxEntityComponentCount = 128;
const u32 maxEntityTagCount = 128;

struct Component;
class World;

struct EntityArchetype
{
    String name;
    Array<ComponentTypeId> componentTypes;
};

class E_API Entity
{
public:
    struct ComponentInfo
    {
        ComponentTypeId typeId = 0;
        Component* component = nullptr;
    };
    
    friend class Logic;
	
    Entity() {}
	~Entity() {}
    Component* getComponent(ComponentTypeId componentType);
	template<typename ComponentClassType>
	inline ComponentClassType* getComponent()
    {
        for (u32 i = 0; i < componentCount; i++)
        {
            if (componentInfo[i].typeId == (ComponentTypeId)ComponentClassType::typeId)
                return (ComponentClassType*)componentInfo[i].component;
        }

        return nullptr;
    }

	inline ComponentInfo* getComponentInfos() { return componentInfo; }
	inline struct TransformComponent* getTransform() { return (struct TransformComponent*)getComponent<struct TransformComponent>(); }
	Component* addComponent(ComponentTypeId type);
	template<typename ComponentClassType>
	inline ComponentClassType* addComponent() { return (ComponentClassType*)addComponent((ComponentTypeId)ComponentClassType::typeId); }
	
	EntityId id = invalidEntityId;
	String name;
	bool active = true;
	Entity* parent = nullptr;
    EntityTagIndex tags[maxEntityTagCount] = { 0 };
	struct EntityGroup* group = nullptr;
	ResourceId prefabId = 0;
	
protected:
    void removeComponent(ComponentTypeId type);

    u32 componentCount = 0;
    ComponentInfo componentInfo[maxEntityComponentCount] = { 0 };
};

}