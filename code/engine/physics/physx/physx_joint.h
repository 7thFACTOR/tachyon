#pragma once
#include "common.h"
#include "base/types.h"
#include "base/defines.h"
#include "physics/joint.h"
#include "extensions/PxJoint.h"

namespace engine
{
using namespace physx;

class PhysXJoint : public PhysicsJoint
{
public:
	PhysXJoint();
	~PhysXJoint();

	virtual bool  create();
	void free();
	void setBreakable(bool bIsBreakable, f32 aMaxBreakForce = 1, f32 aMaxBreakTorque = 1);
	void setCollisionBetweenBodies(bool bCollide);
	void setMotor(bool bMotorized, f32 aMaxForce, f32 aVelocity);
	void addLimitPlane(const Vec3& rPlaneNormal, const Vec3& rPlanePoint, f32 aRestitution = 0);
	void setLimitPoint(const Vec3& rPoint, bool bIsOnBody2 = true);
	void clearLimitPlanes();

	PxJoint* m_pNxJoint;
	PxScene* m_pNxScene;
	PxJointDesc* m_pNxJointDesc;
};

class PhysXCylindricalJoint : public PhysXJoint
{
public:
	PhysXCylindricalJoint();
	~PhysXCylindricalJoint();

	bool create();

	NxCylindricalJointDesc m_jointDesc;
};

// TODO: DOF6
class PhysXUniversalJoint : public PhysXJoint
{
public:
	PhysXUniversalJoint();
	~PhysXUniversalJoint();

	bool create();

	NxD6JointDesc m_jointDesc;
};

class PhysXDistanceJoint : public PhysXJoint
{
public:
	PhysXDistanceJoint();
	~PhysXDistanceJoint();

	bool create();

	NxDistanceJointDesc m_jointDesc;
	f32 m_minDistance, m_maxDistance;
	bool m_bUseMinDistance, m_bUseMaxDistance;
};

class PhysXFixedJoint : public PhysXJoint
{
public:
	PhysXFixedJoint();
	~PhysXFixedJoint();

	bool create();

	NxFixedJointDesc m_jointDesc;
};

class PhysXPointInPlaneJoint : public PhysXJoint
{
public:
	PhysXPointInPlaneJoint();
	~PhysXPointInPlaneJoint();

	bool create();

	NxPointInPlaneJointDesc m_jointDesc;
};

class PhysXPointOnLineJoint : public PhysXJoint
{
public:
	PhysXPointOnLineJoint();
	~PhysXPointOnLineJoint();

	bool create();

	NxPointOnLineJointDesc m_jointDesc;
};

class PhysXPrismaticJoint : public PhysXJoint
{
public:
	PhysXPrismaticJoint();
	~PhysXPrismaticJoint();

	bool create();

	NxPrismaticJointDesc m_jointDesc;
};

class PhysXPulleyJoint : public PhysXJoint
{
public:
	PhysXPulleyJoint();
	~PhysXPulleyJoint();

	bool create();

	NxPulleyJointDesc m_jointDesc;
	Vec3 m_pulley[2];
	f32 m_ropeLength, m_stiffness, m_transmissionRatio;
	bool m_bIsRigid;
};

class PhysXRevoluteJoint : public PhysXJoint
{
public:
	PhysXRevoluteJoint();
	~PhysXRevoluteJoint();

	bool create();

	NxRevoluteJointDesc m_jointDesc;
	bool m_bEnableLimits;
	PhysicsJointLimit m_lowLimit, m_highLimit;
};

class PhysXSphericalJoint : public PhysXJoint
{
public:
	PhysXSphericalJoint();
	~PhysXSphericalJoint();

	bool create();

	NxSphericalJointDesc m_jointDesc;
};
}