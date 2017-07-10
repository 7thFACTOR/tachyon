#pragma once
#include "base/string.h"
#include "base/array.h"
#include "base/map.h"
#include "base/defines.h"
#include "base/types.h"
#include "game/component.h"
#include "game/game_system.h"
#include "game/components/ambient_music.h"
#include "game/components/sound_emitter.h"
#include "game/components/sound_listener.h"
#include "core/types.h"
#include "game/types.h"
#include "game/component_type_manager.h"

namespace engine
{
class SoundSource;
struct AmbientMusicComponent;

class E_API AudioSystem : public GameSystem
{
public:
	B_DECLARE_DERIVED_CLASS(AudioSystem, GameSystem);
	AudioSystem();
	~AudioSystem();
	void update();
	void onEntityCreated(EntityId entity);
	void onEntityRemoved(EntityId entity);
	void onComponentAdded(EntityId entity, Component* pComponent);
	void onComponentRemoved(EntityId entity, Component* pComponent);

protected:
	Array<Component*> m_audioComponents;
	ComponentTypeManager<SoundEmitterComponent, ComponentType::SoundEmitter>
		m_soundEmitterComponentMgr;
	ComponentTypeManager<SoundListenerComponent, ComponentType::SoundListener>
		m_soundListenerComponentMgr;
	ComponentTypeManager<AmbientMusicComponent, ComponentType::AmbientMusic>
		m_ambientMusicComponentMgr;
};

}