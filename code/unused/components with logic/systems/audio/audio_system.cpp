#include "game/systems/audio/audio_system.h"
#include "game/component_type_manager.h"
#include "game/types.h"
#include "game/game_manager.h"
#include "core/core.h"
#include "base/assert.h"
#include "base/math/conversion.h"
#include "render/renderer.h"
#include "audio/sound_source.h"
#include "audio/audio_manager.h"
#include "game/components/ambient_music.h"
#include "game/components/sound_emitter.h"
#include "game/components/sound_listener.h"

namespace engine
{
B_REGISTER_CLASS(AudioSystem);

AudioSystem::AudioSystem()
{
	game().registerComponentTypeManager(&m_soundEmitterComponentMgr);
	game().registerComponentTypeManager(&m_ambientMusicComponentMgr);
	game().registerComponentTypeManager(&m_soundListenerComponentMgr);
}

AudioSystem::~AudioSystem()
{
	game().unregisterComponentTypeManager(&m_soundEmitterComponentMgr);
	game().unregisterComponentTypeManager(&m_ambientMusicComponentMgr);
	game().unregisterComponentTypeManager(&m_soundListenerComponentMgr);
}

void AudioSystem::update()
{
	f32 deltaTime = gameDeltaTime();

	for (auto comp : m_audioComponents)
	{
		switch (comp->type)
		{
		case ComponentType::AmbientMusic:
			{
				AmbientMusicComponent* pComp = (AmbientMusicComponent*)comp;
				pComp->update(deltaTime);
				break;
			}
		case ComponentType::SoundEmitter:
			{
				SoundEmitterComponent* pComp = (SoundEmitterComponent*)comp;
				//pComp->update(deltaTime);
				break;
			}
		}
	}

	if (&audio())
	{
		audio().update();
	}
}

void AudioSystem::onEntityCreated(EntityId aId)
{}

void AudioSystem::onEntityRemoved(EntityId aId)
{}

void AudioSystem::onComponentAdded(EntityId aId, Component* pComp)
{
	if (pComp->type == ComponentType::AmbientMusic)
		m_audioComponents.appendUnique(pComp);
}

void AudioSystem::onComponentRemoved(EntityId aId, Component* pComp)
{
	if (pComp->type == ComponentType::AmbientMusic)
		m_audioComponents.erase(pComp);
}

}