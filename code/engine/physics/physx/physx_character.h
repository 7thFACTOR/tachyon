// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "physics/character.h"
#include "PxCapsuleController.h"

namespace engine
{
using namespace physx;
class PhysXCharacter : public PhysicsCharacter
{
public:
	PhysXCharacter();
	~PhysXCharacter();

	bool create(f32 aCapsuleRadius, f32 aCapsuleHeight);
	void moveCharacter(const Vec3& rDelta, f32 aMinDistance = 0.1f, f32 aAutoStepSharpness = 0.5f);
	void enableCollisions(bool bEnable);
	Vec3 translation() const;
	void setTranslation(const Vec3& rPos);
	void setCapsuleHeight(f32 aHeight);
	void setCapsuleRadius(f32 aRadius);
	f32 capsuleHeight() const;
	f32 capsuleRadius() const;
	bool isCapsuleWorldOverlapping(f32 aRadius, f32 aHeight) const;
	bool checkGroundIntersection(
				f32* pOutDist = nullptr,
				Vec3* pOutWorldPt = nullptr,
				Vec3* pOutWorldNormal = nullptr) const;
	bool isCollidingSides() const;
	bool isCollidingGround() const;
	bool isCollidingUp() const;

	PxCapsuleController* m_pNxCtrl;
	PxScene* m_pNxScene;
	PxU32 m_lastCollFlags;
};
}