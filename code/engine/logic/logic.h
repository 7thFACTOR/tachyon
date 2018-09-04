// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/array.h"
#include "base/observer.h"
#include "logic/types.h"
#include "logic/world.h"
#include "logic/component.h"
#include "logic/component_system.h"
#include "input/input.h"
#include "input/input_map.h"

namespace engine
{
using namespace base;

struct Component;
class ComponentPool;
class ComponentSystem;
class World;

struct LogicObserver
{
	virtual void onEntityCreated(Entity* entity) {}
	virtual void onEntityModified(Entity* entity) {}
	virtual void onEntityRemoved(Entity* entity) {}
	virtual void onComponentCreated(Component* component) {}
	virtual void onComponentModified(Component* component) {}
	virtual void onComponentRemoved(Component* component) {}
	virtual void onSetActiveWorld(World* world) {}
	virtual void onWorldCreated(World* world) {}
	virtual void onWorldModified(World* world) {}
	virtual void onWorldRemoved(World* world) {}
};

class E_API Logic : public Observable<LogicObserver>
{
public:
	friend class Entity;

	Logic() {}
	virtual ~Logic() {}
	bool initialize();
	void shutdown();
	void addComponentPool(ComponentPool* pool);
	void removeComponentPool(ComponentPool* pool);
	inline ComponentPool* getComponentPool(ComponentTypeId type) { return componentPools[type]; }
	template<typename ComponentClassType>
	inline ComponentPool* getComponentPool() { return componentPools[(ComponentTypeId)ComponentClassType::typeId]; }
	Component* createComponent(ComponentTypeId type);
	void deleteComponent(Component* component);
	void addComponentSystem(ComponentSystem* csystem, u32 priority = ComponentSystem::UpdatePriority::Normal);
	void removeComponentSystem(ComponentSystem* csystem);
	const Array<ComponentSystem*>& getComponentSystems() const { return componentSystems; }
	ComponentSystem* getComponentSystem(ComponentSystemId id);
	Component* addComponentToEntity(ComponentTypeId type, Entity* entity);
	template<typename ComponentClassType>
	inline ComponentClassType* addComponentToEntity(Entity* entity) { return (ComponentClassType*)addComponentToEntity((ComponentTypeId)ComponentClassType::typeId, entity); }
	f32 getDeltaTime() const;
	f32 getFrameRate() const;
	inline World& getWorld() const { return *currentWorld; }
	inline void setActiveWorld(World* world) { currentWorld = world; }
	inline virtual World* createWorld() { World* wld = new World(); worlds.append(wld); return wld; }
	inline void removeWorld(World* world) { worlds.erase(world); }
	virtual void update();

public:
//TODO: move to input component
	InputMap inputMap;

protected:
	void computeFrameTiming();
	
	f32 framesPerSecond = 0;
	f32 frameCount = 0;
	f64 fpsTime = 0;
	Dictionary<ComponentTypeId, ComponentPool*> componentPools;
	Array<ComponentSystem*> componentSystems;
	u32 lastComponentSystemPriority = 0;
	Array<World*> worlds;
	World* currentWorld = nullptr;
};

}