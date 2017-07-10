#include "game/game_manager.h"
#include "base/defines.h"
#include "core/core.h"
#include "base/assert.h"
#include "base/logger.h"
#include "base/variable.h"
#include "game/types.h"
#include "input/input_manager.h"
#include "render/resources/texture_resource.h"
#include "core/resource_manager.h"
#include "render/renderer.h"
#include "base/util.h"
#include "game/entity_group.h"
#include "game/component.h"
#include "base/qsort.h"
#include "game/component_type_manager.h"
#include "game/entity_factory.h"
#include "game/std_entities_factory.h"
#include "game/components/info.h"
#include "base/job_manager.h"
#include "base/application.h"

#include "systems/render/render_system.h"
#include "systems/audio/audio_system.h"
#include "systems/raycast/raycast_system.h"
//#include "systems/flowgraph/flowgraph_system.h"
#include "systems/input/input_system.h"

#include "render/aux_renderer.h"

namespace engine
{
const String saveGameFolder = "savedgames";

static StdEntitiesFactory stdEntsFactory;

B_REGISTER_CLASS(GameManager);

bool sortTasks(JobTask** t1, JobTask** t2)
{
	return (*t1)->startTime < (*t2)->startTime;
}

struct TaskVisualizer : StatsVisualizer
{
	void render(AuxRenderer& aux)
	{
		Array<JobTask*> tasks;
		Color colors[] = { { Color::red }, { Color::green }, { Color::white }, { Color::black }, { Color::yellow } };

		f32 topPos = 0.9f;
		f32 height = 0.03f;
		f32 spacing = 0.002f;
		RectF rc;

		//B_LOG_INFO("%d Job tasks took %f sec", (int)jobs().activeTaskCount(), jobs().profilerTotalTime());

		aux.set2dProjection();
		rc.x = 0;
		rc.y = 0.5f;
		rc.width = 1;
		rc.height = 0.5f;
		aux.beginPrimitives(RenderPrimitive::Triangles);
		aux.setBlendMode(BlendMode::Alpha);
		aux.setColor({ 1, 1, 1, 0.5f });
		aux.disableTexturing();
		aux.add2dQuad(rc.x, rc.y, rc.width, rc.height);
		aux.endPrimitives();

		f64 allTime = getJobs().getProfilerTotalTime();

		for (size_t i = 0; i < getJobs().getWorkerCount(); i++)
		{
			f32 startPos = 0.0f;
			f32 endPos = 1.0f - height;

			getJobs().fillTasksOfWorker(i, tasks);

			//B_LOG_DEBUG("Worker %d has %d tasks", i, tasks.size());

			if (tasks.notEmpty())
			{
				B_QSORT(JobTask*, tasks.data(), tasks.size(), sortTasks);
			}

			for (auto task : tasks)
			{
				//B_LOG_DEBUG("T s %f e %f time %f", task->startTime, task->endTime, task->endTime - task->startTime);
				rc.x = startPos;
				rc.y = topPos;
				rc.width = (task->endTime - task->startTime) / allTime;
				rc.height = height;

				aux.beginPrimitives(RenderPrimitive::Triangles);
				aux.setColor(colors[task->job->subsystemIndex]);
				aux.disableTexturing();
				aux.add2dQuad(rc.x, rc.y, rc.width, rc.height);
				aux.endPrimitives();

				startPos += rc.width + spacing;
			}

			topPos -= height + spacing;
		}
	}
};

GameManager::GameManager()
{
	gameState = GameState::InGame;
	previousGameState = gameState;
	currentEntityId = 1;
}

GameManager::~GameManager()
{
	//TODO
}

bool GameManager::initialize()
{
	registerEntityFactory(&stdEntsFactory);
	registerGameSystem(new RenderSystem);
	registerGameSystem(new AudioSystem);
	registerGameSystem(new RaycastSystem);
	registerGameSystem(new InputSystem);

	E_REGISTER_COMPONENT_POOL(InfoComponent);

	String engineToc = getVariableValue<String>("sys_engine_bundle_toc");

	getResources().addBundle(engineToc);
	getAuxRenderer()->registerStatsVisualizer(new TaskVisualizer);

	getRenderer().getOsWindow()->addObserver(this);

	return true;
}

bool GameManager::shutdown()
{
	getRenderer().getOsWindow()->removeObserver(this);
	unregisterEntityFactory(&stdEntsFactory);
	free();

	return true;
}

void GameManager::free()
{
	for (auto grp : groups)
	{
		delete grp;
	}

	groups.clear();

	for (auto sys : systems)
	{
		delete sys;
	}

	systems.clear();
}

WorldSectionResource* GameManager::loadSection(ResourceId resId, bool async)
{
	return nullptr;
}

bool GameManager::unloadSection(WorldSectionResource* section)
{
	//TODO
	return true;
}


GameState GameManager::getGameState() const
{
	return gameState;
}

void GameManager::setGameState(GameState state)
{
	onExitGameState();
	previousGameState = gameState;
	gameState = state;
	onEnterGameState();
}

void GameManager::setGameMode(bool enable)
{
	if (enable)
	{
		setEngineMode(EngineMode::Playing);
		B_LOG_DEBUG("Enter game mode...");

		//TODO: check for TransformComponent of the entity
		/*Entity* pStartPos = m_pWorld->findEntityByName("PlayerStart");

		if (pStartPos
			&& getGraphicsProvider()->getCamera())
		{
			pStartPos->computeTransform();
			getGraphicsProvider()->getCamera()->setTranslation(pStartPos->getWorldTranslation());
			getGraphicsProvider()->getCamera()->setRotation(pStartPos->getWorldMatrix().getInverted().toQuaternion());
			getGraphicsProvider()->getCamera()->computeTransform();
		}
		else
		{
			N_LOG_WARNING("Cannot find player start entity, skipping repositioning...");
		}*/

		B_LOG_DEBUG("Enter in-game state...");
		setGameState(GameState::InGame);

		//for (size_t i = 0, iCount = m_entities.size(); i < iCount; ++i)
		//{
		//	m_entities[i]->onStartGamePlay();
		//}

		//enableSimulation(true);
	}
	else
	{
		B_LOG_INFO("Exit in-game state...");
		
		//for (size_t i = 0, iCount = m_entities.size(); i < iCount; ++i)
		//{
		//	m_entities[i]->onStopGamePlay();
		//}

		//enableSimulation(false);
		onExitGameState();
		B_LOG_INFO("Exit game mode...");
		//setEngineMode((EEngineMode)m_oldEngineMode);
	}
}

bool GameManager::isGameMode()
{
	return gameState == GameState::InGame;
}

bool GameManager::saveGame(
	const String& filename, Serializable* customDataWriter)
{
	//String filename = aToFolder + "/" + aGameName + ".save";

	//createFullPath(aToFolder);
	//filename = beautifyPath(filename);
	//File* pFile = files().open(
	//	filename,
	//	File::eMode_Create
	//	| File::eMode_BinaryReadWrite);
	//B_ASSERT(pFile);

	//if (!pFile)
	//{
		return false;
	//}

	//pFile->writeInt32(aCustomLevelDataSize);

	//if (pCustomLevelData && aCustomLevelDataSize)
	//{
	//	pFile->write(pCustomLevelData, aCustomLevelDataSize);
	//}

	//bool res = false;//saveToFile(pFile);

	//pFile->close();

	//return res;
}

bool GameManager::loadGame(
	const String& filename, Serializable* customDataReader)
{
	//String filename = String(pFromFolder) + "/" + String(pGameName) + ".save";
	//File* pFile = files().open(filename, File::eMode_BinaryRead);

	//B_ASSERT(pFile);

	//if (!pFile)
	//{
		return false;
	//}

	//i32 customDataSize;
	//pFile->readInt32(customDataSize);
	//u64 pos = pFile->position();

	//if (ppCustomLevelData && pCustomLevelDataSize)
	//{
	//	*ppCustomLevelData = new char[customDataSize];
	//	B_ASSERT(*ppCustomLevelData);
	//	pFile->read(*ppCustomLevelData, customDataSize);
	//	*pCustomLevelDataSize = customDataSize;
	//}
	//else
	//{
	//	// skip the data block
	//	pFile->seek(File::eSeek_Set, pos + customDataSize);
	//}

	//bool res = false;//world().loadFromFile(pFile);
	//pFile->close();

	//return res;
}

void GameManager::loadSavedGames(const String& path)
{
	FoundFileInfo info;
	String filename;
	FindFileHandle hFind;
	savedGames.clear();

	for (auto thumb : savedGamesThumbs)
	{
		getResources().unload(thumb);
	}

	savedGamesThumbs.clear();

	if (hFind = findFirstFile(path, "*.save", info))
	{
		do
		{
			String file;

			file = extractFilenameBasename(filename);
			//ResourceId tex = resources().load(resources().id(file + ".png"));

			savedGames.append(file);
			B_LOG_DEBUG("Found saved game: '%s'", file.c_str());

			//TODO: need a png loader
			//pTex->setFileName(aFromFolder + "/" + file + ".png");
			//pTex->load();

			//m_savedGamesThumbs.append(tex);

		}
		while (findNextFile(hFind, info));
	}
}

void GameManager::addComponentToEntity(
	Component* component,
	EntityId entity)
{
	B_ASSERT(component);

	if (!component)
	{
		return;
	}

	component->entity = entity;

	for (auto sys : systems)
	{
		sys->onComponentAdded(entity, component);
	}
}

Component* GameManager::addComponent(const String& typeName, EntityId entity)
{
	for (size_t i = 0; i < componentTypeManagers.size(); ++i)
	{
		if (typeName == componentTypeManagers[i]->getComponentTypeName())
		{
			Component* component =
				componentTypeManagers[i]->createComponent(entity);
			
			addComponentToEntity(component, entity);
			
			return component;
		}
	}

	return nullptr;
}

Component* GameManager::addComponent(
	ComponentType componentType, EntityId entity)
{
	for (auto compTypeMgr : componentTypeManagers)
	{
		if (compTypeMgr->getComponentType() == componentType)
		{
			Component* component = compTypeMgr->createComponent(entity);
			
			addComponentToEntity(component, entity);
			
			return component;
		}
	}

	return nullptr;
}

void GameManager::removeComponent(EntityId entity, Component* component)
{
	B_ASSERT(component);

	if (!component)
	{
		return;
	}

	for (auto sys : systems)
	{
		sys->onComponentRemoved(entity, component);
	}

	component->entity = 0;
}

void GameManager::removeAllComponents(EntityId entity)
{
	Array<Component*> comps;

	if (!findEntityComponents(entity, comps))
		return;

	for (auto sys : systems)
	{
		for (auto comp : comps)
		{
			sys->onComponentRemoved(entity, comp);
			comp->entity = 0;
		}
	}
}

void GameManager::registerComponentTypeManager(ComponentPool* mgr, size_t maxComponentCount)
{
	mgr->resizeComponentArray(maxComponentCount, getJobs().getWorkerCount());
	componentTypeManagers.appendUnique(mgr);
}

void GameManager::unregisterComponentTypeManager(ComponentPool* mgr)
{
	auto iter = componentTypeManagers.find(mgr);

	if (iter != componentTypeManagers.end())
	{
		componentTypeManagers.erase(iter);
	}
}

ComponentPool* GameManager::getComponentTypeManager(ComponentType type)
{
	for (auto mgr : componentTypeManagers)
	{
		if (mgr->getComponentType() == type)
		{
			return mgr;
		}
	}

	return nullptr;
}

void GameManager::registerEntityFactory(EntityFactory* factory)
{
	factories.appendUnique(factory);
}

void GameManager::unregisterEntityFactory(EntityFactory* factory)
{
	auto iter = factories.find(factory);

	if (iter != factories.end())
	{
		factories.erase(iter);
	}
}

bool GameManager::registerGameSystem(GameSystem* system)
{
	bool ok = systems.appendUnique(system);
	
	sortGameSystems();

	return ok;
}

void GameManager::unregisterGameSystem(GameSystem* system)
{
	auto iter = systems.find(system);

	if (iter != systems.end())
	{
		systems.erase(iter);
		sortGameSystems();
	}
}

GameSystem* GameManager::getGameSystem(ClassDescriptor* classDesc)
{
	for (size_t i = 0; i < systems.size(); ++i)
	{
		if (systems[i]->getClassDescriptor() == classDesc)
		{
			return systems[i];
		}
	}

	return nullptr;
}

GameSystem* GameManager::getGameSystem(GameSystemType type)
{
	for (auto gsys : systems)
	{
		if (gsys->getType() == type)
		{
			return gsys;
		}
	}

	return nullptr;
}

void GameManager::update()
{
	f32 deltaTime = getGameDeltaTime();
	auto rootJob = getJobs().addEmptyTask();

	rootJob->done = true;

	for (auto gsys : systems)
	{
		gsys->prepareJobTasks(rootJob);
	}

	getJobs().setProfilerStartTimeNow();
	getJobs().waitToFinish(rootJob);
	getJobs().setProfilerEndTimeNow();

	for (auto gsys : systems)
	{
		gsys->update(deltaTime);
	}

	getJobs().reset();
}

static bool sortGameSystemsByProcessingOrder(GameSystem** a, GameSystem** b)
{
	return (*a)->getUpdateOrder() > (*b)->getUpdateOrder();
}

void GameManager::sortGameSystems()
{
	B_QSORT(GameSystem*, &systems[0], systems.size(), sortGameSystemsByProcessingOrder);
}

void GameManager::addEntityGroup(EntityLayer* grp)
{
	groups.appendUnique(grp);
}

void GameManager::removeEntityGroup(EntityLayer* grp)
{
	auto iter = groups.find(grp);

	if (iter != groups.end())
	{
		groups.erase(iter);
	}
}

EntityLayer* GameManager::findEntityGroup(const String& name) const
{
	for (size_t i = 0; i < groups.size(); ++i)
	{
		if (groups[i]->getName() == name)
		{
			return groups[i];
		}
	}

	return nullptr;
}

const Array<EntityLayer*>& GameManager::getGroups() const
{
	return groups;
}

EntityId GameManager::createEntity(const String& templateName)
{
	EntityId id = currentEntityId;

	for (auto factory : factories)
	{
		if (factory->composeEntity(currentEntityId, templateName))
		{
			// if template is supported, return this new entity
			++currentEntityId;
			onEntityAdded(id);
			entityComponentsCache.addEntity(id);
			entities.append(id);

			return id;
		}
	}

	return invalidEntityId;
}

EntityId GameManager::createEmptyEntity()
{
	EntityId id = currentEntityId;

	onEntityAdded(id);
	entityComponentsCache.addEntity(id);
	entities.append(id);
	++currentEntityId;
	
	return id;
}

bool GameManager::addEntity(EntityId entity, EntityId parent)
{
	onEntityAdded(entity);
	entityComponentsCache.addEntity(entity);
	entities.appendUnique(entity);

	if (parent != invalidEntityId)
	{
		entityChildParentCache[entity] = parent;
		entityChildrenCache[parent].appendUnique(entity);
	}

	return true;
}

void GameManager::setEntityParent(EntityId entity, EntityId parent)
{
	EntityId oldParent = getEntityParent(entity);

	if (parent != 0)
	{
		entityChildParentCache[entity] = parent;
		entityChildrenCache[parent].appendUnique(entity);
	}
	else
	{
		auto iter = entityChildParentCache.find(entity);

		if (iter != entityChildParentCache.end())
		{
			entityChildParentCache.erase(iter);
		}

		auto iterParent = entityChildrenCache.find(parent);

		if (iterParent != entityChildrenCache.end())
		{
			iterParent->value.erase(entity);
		}
	}

	for (auto gsys : systems)
	{
		gsys->onEntityParentChange(entity, oldParent, parent);
	}
}

EntityId GameManager::getEntityParent(EntityId entity)
{
	auto iter = entityChildParentCache.find(entity);

	if (iter != entityChildParentCache.end())
	{
		return iter->value;
	}

	return 0;
}

EntityId GameManager::getEntityByName(const String& name)
{
	for (size_t i = 0; i < entities.size(); ++i)
	{
		InfoComponent* infoComponent = getComponent<InfoComponent>(entities[i]);

		if (!infoComponent)
		{
			continue;
		}

		if (infoComponent->name != name)
		{
			continue;
		}

		return infoComponent->entity;
	}

	return invalidEntityId;
}

bool GameManager::removeEntity(EntityId entity)
{
	auto iter = entities.find(entity);

	if (iter != entities.end())
	{
		entities.erase(iter);
		onEntityRemoved(entity);
		removeAllComponents(entity);
		entityComponentsCache.removeEntity(entity);
		entityChildParentCache.erase(entity);
		entityChildrenCache.erase(entity);
	}

	return true;
}

Component* GameManager::getEntityComponent(EntityId entity, ComponentType componentType)
{
	// first, try to find it in the cache
	auto comps = entityComponentsCache.getEntityComponents(entity);

	for (auto component : comps)
	{
		if (component->type == componentType)
		{
			return component;
		}
	}

	Component* comp = nullptr;

	// then try to find it the slow way
	for (auto compTypeMgr : componentTypeManagers)
	{
		if (compTypeMgr->getComponentType() != componentType)
		{
			continue;
		}

		comp = compTypeMgr->getComponentOfEntity(entity);

		if (comp)
		{
			return comp;
		}
	}

	return nullptr;
}

bool GameManager::findEntityComponents(EntityId entity, Array<Component*>& outComps)
{
	Component* comp = nullptr;

	// then try to find it the slow way
	for (auto compTypeMgr : componentTypeManagers)
	{
		comp = compTypeMgr->getComponentOfEntity(entity);

		if (comp)
		{
			outComps.append(comp);
		}
	}

	return outComps.notEmpty();
}

const Array<EntityId>& GameManager::getChildEntities(EntityId entity)
{
	auto iter = entityChildrenCache.find(entity);

	if (iter != entityChildrenCache.end())
	{
		return iter->value;
	}

	return Array<EntityId>();
}

void GameManager::onStartSimulation()
{}

void GameManager::onStopSimulation()
{}

void GameManager::onEnterGameMode()
{}

void GameManager::onExitGameMode()
{}

void GameManager::onEnterGameState()
{}

void GameManager::onExitGameState()
{}

void GameManager::onEntityAdded(EntityId entity)
{
	for (auto gsys : systems)
	{
		gsys->onEntityCreated(entity);
	}
}

void GameManager::onEntityRemoved(EntityId entity)
{
	for (auto gsys : systems)
	{
		gsys->onEntityRemoved(entity);
	}

	for (auto grp : groups)
	{
		grp->removeEntity(entity);
	}

	for (auto compTypeMgr : componentTypeManagers)
	{
		compTypeMgr->onEntityRemoved(entity);
	}
}

void GameManager::onWindowEvent(OsWindow* wnd, const WindowEvent& ev)
{
	if (ev.type == WindowEventType::WindowClosed)
	{
		getApplication().setStopped(true);
	}
}

}