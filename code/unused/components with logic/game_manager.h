#pragma once
#include "base/string.h"
#include "base/array.h"
#include "base/map.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/observer.h"
#include "resources/flowgraph_resource.h"
#include "resources/world_resource.h"
#include "game/game_system.h"
#include "game/types.h"
#include "game/component_cache.h"
#include "game/components/info.h"
#include "game/component_type_manager.h"
#include "base/os_window.h"

namespace base
{
class File;
}

namespace engine
{
class EntityFactory;
class EntityLayer;
struct ComponentPool;
struct Component;
class WorldSectionResource;

enum class GameState
{
	Intro,
	MainMenu,
	LevelLoading,
	LevelCutscene,
	InGame,
	InGameMenu,
	Outro,
	User,
	_Count
};

class ENGINE_API GameManager : public ClassInfo, WindowObserver
{
public:
	B_DECLARE_CLASS(GameManager);

	GameManager();
	virtual ~GameManager();

	//! called by the engine when the game must be initialized
	bool initialize();
	//! called by the engine when the game must be released
	bool shutdown();
	void free();


	//////////////////////////////////////////////////////////////////////////
	// Components
	//////////////////////////////////////////////////////////////////////////

	Component* addComponent(ComponentType component, EntityId entity);
	Component* addComponent(const String& typeName, EntityId entity);
	template <typename Type>
	Type* addComponent(EntityId entity)
	{
		return (Type*)addComponent(Type::Type, entity);
	}

	void removeComponent(EntityId entity, Component* component);
	void removeAllComponents(EntityId entity);

private:
	void addComponentToEntity(Component* component, EntityId entity);

public:
	//////////////////////////////////////////////////////////////////////////
	// Component type managers
	//////////////////////////////////////////////////////////////////////////

	void registerComponentTypeManager(ComponentPool* mgr, size_t maxComponentCount);
	void unregisterComponentTypeManager(ComponentPool* mgr);
	ComponentPool* getComponentTypeManager(ComponentType type);

	//////////////////////////////////////////////////////////////////////////
	// Entity factories
	//////////////////////////////////////////////////////////////////////////

	void registerEntityFactory(EntityFactory* factory);
	void unregisterEntityFactory(EntityFactory* factory);

	//////////////////////////////////////////////////////////////////////////
	// Game systems
	//////////////////////////////////////////////////////////////////////////

	bool registerGameSystem(GameSystem* system);
	void unregisterGameSystem(GameSystem* system);
	GameSystem* getGameSystem(ClassDescriptor* classDesc);
	GameSystem* getGameSystem(GameSystemType type);
	template<class Type>
	Type* getGameSystem() { return (Type*)getGameSystem(Type::GameSystemType); }
	virtual void update();
	void sortGameSystems();

	//////////////////////////////////////////////////////////////////////////
	// Entity groups
	//////////////////////////////////////////////////////////////////////////
	//TODO: layers!

	void addEntityGroup(EntityLayer* grp);
	void removeEntityGroup(EntityLayer* grp);
	EntityLayer* findEntityGroup(const String& name) const;
	const Array<EntityLayer*>& getGroups() const;

	//////////////////////////////////////////////////////////////////////////
	// Entities
	//////////////////////////////////////////////////////////////////////////

	EntityId createEntity(const String& templateName);
	EntityId createEmptyEntity();
	bool addEntity(EntityId entity, EntityId parent = 0);
	bool removeEntity(EntityId entity);
	void setEntityParent(EntityId entity, EntityId parent);
	EntityId getEntityParent(EntityId entity);
	EntityId getEntityByName(const String& name);
	Component* getEntityComponent(EntityId entity, ComponentType type);

	template<typename Type>
	Type* getComponent(EntityId entity)
	{
		return (Type*)(getEntityComponent(entity, (ComponentType)Type::Type));
	}

	bool findEntityComponents(EntityId entity, Array<Component*>& outComps);
	const Array<EntityId>& getEntities() const { return entities; }
	const Array<EntityId>& getChildEntities(EntityId entity);

protected:
	virtual void onEntityAdded(EntityId entity);
	virtual void onEntityRemoved(EntityId entity);
	void onWindowEvent(OsWindow* wnd, const WindowEvent& ev);

	E_DECLARE_COMPONENT_POOL(InfoComponent, ComponentType::Info);


};

}