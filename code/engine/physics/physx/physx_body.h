// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "common.h"
#include "physics/body.h"
#include "base/map.h"
#include "base/types.h"
#include "base/defines.h"
#include "game/components/transform.h"
#include "PxActor.h"

namespace engine
{
using namespace physx;

class PhysXBody : public PhysicsBody
{
public:
	PhysXBody();
	~PhysXBody();

	bool create();
	void free();
	void freeShapes();

	void wakeUp(bool bValue);
	void setEnableGravity(bool bValue);
	void setBodyBehaviorType(EBehavior aValue);
	void setAutoSleepTime(f32 aValue);
	void setAutoSleep(bool bValue);
	void setMass(f32 aMass);

	bool isGravityEnabled() const;
	bool isSleeping() const;
	f32 autoSleepTime() const;
	bool isAutoSleeping() const;
	Vec3 linearVelocity() const;
	Vec3 angularVelocity() const;

	void getTransform(Vec3& rPos, Quat& rQuat);
	void setTransform(const Vec3& rPos, const Quat& rQuat);
	void attachSceneObject(TransformComponent* pObject);
	void addShape(PhysicsShape* pShape);
	void removeShape(PhysicsShape* pShape);
	void setTranslation(const Vec3& rPos);
	void setRotation(const Quat& rQuat);
	void addForce(const Vec3& rValue);
	void addForceAtPosition(const Vec3& rForce, const Vec3& rValue);
	void addTorque(const Vec3& rValue);

	void addRelativeForce(const Vec3& rValue);
	void addRelativeTorque(const Vec3& rValue);
	void addForceAtRelativePosition(const Vec3& rForce, const Vec3& rPos);
	void addRelativeForceAtPosition(const Vec3& rForce, const Vec3& rPos);
	void addRelativeForceAtRelativePosition(const Vec3& rForce, const Vec3& rPos);
	void setLinearVelocity(f32 aX, f32 aY, f32 aZ);
	void setLinearVelocity(const Vec3& rValue);
	void setAngularVelocity(f32 aX, f32 aY, f32 aZ);
	void setAngularVelocity(const Vec3& rValue);

	PxActor* m_pNxActor;
	Map<PhysicsShape*, PxShape*> m_nxShapesMap;
};
}