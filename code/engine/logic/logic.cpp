#include "logic/logic.h"
#include "logic/component_pool.h"
#include "base/timer.h"
#include "logic/component_updater.h"
#include "base/logger.h"
#include "base/task_scheduler.h"
#include "logic/entity.h"
#include "graphics/graphics.h"
#include "input/window.h"
#include "base/profiler.h"
#include "core/globals.h"
#include "core/module.h"

#include "logic/component_updaters/audio/audio_component_updater.h"
#include "logic/component_updaters/input/input_component_updater.h"
#include "logic/component_updaters/render/render_component_updater.h"

#include "logic/components/audio/ambient_music.h"
#include "logic/components/render/camera.h"
#include "logic/components/render/mesh.h"
#include "logic/components/render/mesh_renderer.h"
#include "logic/components/audio/audio_source.h"
#include "logic/components/audio/audio_spectator.h"
#include "logic/components/render/transform.h"
#include "logic/components/input/flyby.h"

namespace engine
{
bool Logic::initialize()
{
	// audio
	addComponentPool(new ComponentPoolTpl<AmbientMusicComponent>());
	addComponentPool(new ComponentPoolTpl<AudioSpectatorComponent>());
	addComponentPool(new ComponentPoolTpl<AudioSourceComponent>());
	
	// render
	addComponentPool(new ComponentPoolTpl<CameraComponent>());
	addComponentPool(new ComponentPoolTpl<MeshComponent>());
	addComponentPool(new ComponentPoolTpl<MeshRendererComponent>());
	addComponentPool(new ComponentPoolTpl<TransformComponent>());

	// input
	addComponentPool(new ComponentPoolTpl<FlybyComponent>());

	addComponentUpdater(new InputComponentUpdater(), ComponentUpdater::UpdatePriority::First);
	addComponentUpdater(new AudioComponentUpdater(), ComponentUpdater::UpdatePriority::Normal);
	addComponentUpdater(new RenderComponentUpdater(), ComponentUpdater::UpdatePriority::Last);

	return true;
}

void Logic::shutdown()
{
	for (auto& pool : componentPools)
	{
		delete pool.value;
	}
	componentPools.clear();

	for (auto& updater : componentUpdaters)
	{
		delete updater;
	}
	componentUpdaters.clear();

	for (auto world : worlds)
	{
		delete world;
	}

	worlds.clear();
	currentWorld = nullptr;
}

void Logic::addComponentPool(ComponentPool* pool)
{
	componentPools.add(pool->getComponentTypeId(), pool);
}

void Logic::removeComponentPool(ComponentPool* pool)
{
	componentPools.erase(pool->getComponentTypeId());
}

Component* Logic::createComponent(ComponentTypeId type)
{
	auto iter = componentPools.find(type);

	if (iter != componentPools.end())
	{
		return iter->value->createComponent();
	}

	return nullptr;
}

void Logic::deleteComponent(Component* component)
{
	auto iter = componentPools.find(component->type);

	if (iter != componentPools.end())
	{
		iter->value->deleteComponent(component);
	}
}

void Logic::addComponentUpdater(ComponentUpdater* updater, u32 priority)
{
	// insert before the last highest priority updater
	if (lastUpdaterPriority > priority)
	{
		if (componentUpdaters.isEmpty())
		{
			componentUpdaters.append(updater);
		}
		else
		{
			// insert before the last updater
			componentUpdaters.insert(componentUpdaters.size() - 1, updater);
		}
	}
	else
	{
		componentUpdaters.append(updater);
	}

	lastUpdaterPriority = priority;
}

void Logic::removeComponentUpdater(ComponentUpdater* updater)
{
	componentUpdaters.erase(updater);
}

ComponentUpdater* Logic::getComponentUpdater(ComponentUpdaterId id)
{
	for (auto updater : componentUpdaters)
	{
		if (updater->getId() == id)
			return updater;
	}

	return nullptr;
}

Component* Logic::addComponentToEntity(ComponentTypeId type, Entity* entity)
{
	auto pool = componentPools[type];

	B_ASSERT(pool);
	B_ASSERT(entity);

	Component* newComponent = pool->createComponent();
	
	B_ASSERT(newComponent);

	if (!newComponent)
	{
		B_RELEASE_ASSERT(newComponent);
		return nullptr;
	}

	entity->addComponent(newComponent);

	return newComponent;
}

void Logic::update()
{
	computeFrameTiming();
	f32 deltaTime = getDeltaTime();

	inputMap.update(deltaTime);

	// update components and their systems
	for (auto updater : componentUpdaters)
	{
		updater->update(deltaTime);
	}

	// update modules
	for (auto& module : getModuleLoader().getModules())
	{
		module.value->onUpdate(deltaTime);
	}
}

f32 Logic::getDeltaTime() const
{
	B_ASSERT(getClock().getTimer(SystemTimer::logic));
	return getClock().getTimer(SystemTimer::logic)->getDelta();
}

f32 Logic::getFrameRate() const
{
	return framesPerSecond;
}

void Logic::computeFrameTiming()
{
	getClock().update();

	f32 delta = getDeltaTime();

	if (fpsTime >= 1.0f)
	{
		framesPerSecond = frameCount;
		frameCount = 0.0f;
		fpsTime = 0.0f;
	}

	++frameCount;
	fpsTime += delta;
}

}