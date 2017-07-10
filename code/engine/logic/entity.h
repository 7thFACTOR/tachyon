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

struct Component;
class World;

class E_API Entity
{
public:
	friend class Logic;
	Entity() {}
	virtual ~Entity() {}
	inline Component* getComponent(ComponentTypeId componentType) { return components[componentType]; }
	template<typename ComponentClassType>
	inline ComponentClassType* getComponent() { return (ComponentClassType*)components[(ComponentTypeId)ComponentClassType::typeId]; }
	inline const Dictionary<ComponentTypeId, Component*>& getComponents() const { return components; }
	inline struct TransformComponent* getTransform() { return (struct TransformComponent*)components[StdComponentTypeId_Transform]; }
	Component* addComponent(ComponentTypeId type);
	template<typename ComponentClassType>
	inline ComponentClassType* addComponent() { return (ComponentClassType*)addComponent((ComponentTypeId)ComponentClassType::typeId); }
	
	EntityId id = invalidEntityId;
	String name;
	bool active = true;
	Entity* parent = nullptr;
	Array<EntityTagIndex> tags;
	struct EntityGroup* group = nullptr;
	ResourceId prefabId = 0;
	
protected:
	Component* addComponent(Component* component);
	inline void removeComponent(ComponentTypeId type) { components.erase(type); }

	Dictionary<ComponentTypeId, Component*> components;
};

}