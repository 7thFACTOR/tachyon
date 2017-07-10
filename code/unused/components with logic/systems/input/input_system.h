#pragma once
#include "base/string.h"
#include "base/array.h"
#include "base/map.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/observer.h"
#include "base/math/ray.h"
#include "game/component.h"
#include "game/game_system.h"
#include "render/renderer.h"
#include "game/component_type_manager.h"
#include "game/components/free_camera_controller.h"

namespace engine
{
struct FreeCameraControllerComponent;

class E_API InputSystem : public GameSystem
{
public:
	enum { Type = GameSystemType::Input };

	B_DECLARE_DERIVED_CLASS(InputSystem, GameSystem);
	InputSystem();
	virtual ~InputSystem();
	void update();
	void onComponentAdded(EntityId aId, Component* pComp);
	void onComponentRemoved(EntityId aId, Component* pComp);
	void update(FreeCameraControllerComponent* pComp);

protected:
	Array<FreeCameraControllerComponent*> m_freeCamComponents;
	E_DECLARE_COMPONENT_MANAGER(FreeCameraControllerComponent, ComponentType::FreeCameraController);
};
}