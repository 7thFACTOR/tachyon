#pragma once
#include "base/defines.h"
#include "base/observer.h"
#include "base/math/smoother.h"
#include "base/math/vec3.h"
#include "base/math/quat.h"
#include "game/component.h"

namespace engine
{
struct CameraComponent;
struct FirstPersonViewComponent;
class PhysicsCharacter;

struct FirstPersonViewObserver
{
	virtual void onJumpStart(FirstPersonViewComponent*) {};
	virtual void onJumpEnd(FirstPersonViewComponent*) {};
	virtual void onCrouchStart(FirstPersonViewComponent*) {};
	virtual void onCrouchEnd(FirstPersonViewComponent*) {};
	virtual void onStandUpStart(FirstPersonViewComponent*) {};
	virtual void onStandUpEnd(FirstPersonViewComponent*) {};
	virtual void onLeanLeftStart(FirstPersonViewComponent*) {};
	virtual void onLeanLeftEnd(FirstPersonViewComponent*) {};
	virtual void onLeanRightStart(FirstPersonViewComponent*) {};
	virtual void onLeanRightEnd(FirstPersonViewComponent*) {};
	virtual void onLeanUpStart(FirstPersonViewComponent*) {};
	virtual void onLeanUpEnd(FirstPersonViewComponent*) {};
	virtual void onLeanDownStart(FirstPersonViewComponent*) {};
	virtual void onLeanDownEnd(FirstPersonViewComponent*) {};
	virtual void onFootStep(FirstPersonViewComponent*) {};
};

enum class StrafeDirection
{
	Left = -1,
	Right = 1
};

enum class CameraEffect
{
	Shake,
	FovWarp
};

enum class LeaningDirection
{
	None,
	Left,
	Right,
	Up,
	Down,
	_Count
};

//! This class controls a camera, modifying the angles to simulate walking for FPS type games\n
//! It also includes some special camera effects, like a quake, and FieldOfView speed warping
struct FirstPersonViewComponent : Component, Observable<FirstPersonViewObserver>
{
	enum
	{
		Type = ComponentType::FirstPersonView,
		DefaultCount = kMaxComponentCountPerType
	};

	FirstPersonViewComponent()
	{
		// 0..1
		leaningSpeed[(int)LeaningDirection::Left] = 1.0f;
		leaningSpeed[(int)LeaningDirection::Right] = 1.0f;
		leaningSpeed[(int)LeaningDirection::Up] = 1.0f;
		leaningSpeed[(int)LeaningDirection::Down] = 1.0f;

		// degrees
		leaningAngle[(int)LeaningDirection::Left] = 30.0f;
		leaningAngle[(int)LeaningDirection::Right] = 30.0f;
		leaningAngle[(int)LeaningDirection::Up] = 30.0f;
		leaningAngle[(int)LeaningDirection::Down] = 30.0f;

		// meters
		leaningDistance[(int)LeaningDirection::Left] = 0.5f;
		leaningDistance[(int)LeaningDirection::Right] = 0.5f;
		leaningDistance[(int)LeaningDirection::Up] = 0.5f;
		leaningDistance[(int)LeaningDirection::Down] = 0.5f;

		m_yawSmoother.setSampleCount(3);
		m_pitchSmoother.setSampleCount(3);
	}

	PhysicsCharacter* pCharacter = nullptr;
	Smoother<f32> m_pitchSmoother, m_yawSmoother;
	bool bIsPlayerMoving = false;
	bool bIsPlayerRunning = false;
	bool bDisableWalkBounce = false;
	f32 walkBounceAngle = 0.0f;
	f32 walkBounceDir = 1.0f;
	f32 walkBounceSpeed = 6.0f;
	f32 walkAngleAperture = 100.0f;
	f32	strafeRoll = 0.0f;
	f32 strafeRollDir = 0.0f;
	f32 strafeRollSpeed = 5.0f;
	f32 strafeRollAngle = 2.0f;
	f32 runningMultiplier = 1.5f;
	f32 shakeStrength = 1.0f;
	f32 shakeSpeed = 35.0f;
	f32 fovWarpToAngle = 90.0f;
	bool bShakeEffect = false;
	bool bFovWarpEffect = false;
	f32 shakeEffectTimer = 0.0f;
	f32 fovWarpTimer = 0.0f;
	f32 fovWarpCurrentAngle = 0.0f;
	f32 fovWarpDir = -1.0f;
	f32 fovWarpNormal = 75.0f;
	f32 fovWarpSpeed = 130.0f;
	Quat m_shakeFinalRot, m_shakeCurrentRot;
	f32 lookAroundSpeedMultiplier = 0.1f;
	f32 jumpInitialForce = 10.0f;
	f32 jumpCurrentForce = 0.0f;
	f32 crouchHeight = 1.0f;
	f32 standupHeight = 2.0f;
	f32 avatarCurrentHeight = 2.0f;
	f32 crouchSpeed = 2.0f;
	f32 standUpSpeed = 2.0f;
	bool bJumping = false;
	bool bCrouching = false;
	bool bStandingUp = false;
	bool bCrouched = false;
	f32 leaningSpeed[(int)LeaningDirection::_Count];
	f32 leaningAngle[(int)LeaningDirection::_Count];
	f32 leaningDistance[(int)LeaningDirection::_Count];
	f32 leaningCurrentAngle = 0.0f;
	f32 leaningCurrentDistance = 0.0f;
	f32 leaningDirection = (f32)LeaningDirection::None;
	Vec3 avatarMoveDirection = Vec3(0, 0, 1);
	f32 moveMinDistance = 0.01f;
	f32 moveAutoStepSharpness = 1.0f;
};

}