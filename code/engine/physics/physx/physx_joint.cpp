// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "common.h"
#include "physics/physx/physx_joint.h"
#include "physics/physx/physx_body.h"

namespace engine
{
PhysXJoint::PhysXJoint() : PhysicsJoint()
{
	m_pNxJoint = nullptr;
	m_pNxScene = nullptr;
	m_pNxJointDesc = nullptr;
}

PhysXJoint::~PhysXJoint()
{
}

bool PhysXJoint::create()
{
	free();

	if (m_pNxScene)
	{
		m_pNxJointDesc->jointFlags |=
			m_bCollisionBetweenBodies ? NX_JF_COLLISION_ENABLED : 0;
		m_pNxJoint = m_pNxScene->createJoint(*m_pNxJointDesc);
	}

	return true;
}

void PhysXJoint::free()
{
	if (!m_pNxJoint)
		return;

	if (m_pNxScene)
	{
		m_pNxScene->releaseJoint(*m_pNxJoint);
		m_pNxJoint = nullptr;
	}
}

void PhysXJoint::setBreakable(bool bIsBreakable, f32 aMaxBreakForce, f32 aMaxBreakTorque)
{
	m_bBreakable = bIsBreakable;
	m_maxBreakForce = aMaxBreakForce;
	m_maxBreakTorque = aMaxBreakTorque;

	if (m_pNxJoint)
	{
		if (bIsBreakable)
		{
			m_pNxJoint->setBreakable(aMaxBreakForce, aMaxBreakTorque);
		}
		else
		{
			m_pNxJoint->setBreakable(NX_MAX_REAL, NX_MAX_REAL);
		}
	}
}

void PhysXJoint::setCollisionBetweenBodies(bool bCollide)
{
	m_bCollisionBetweenBodies = bCollide;
	create();
}

void PhysXJoint::setMotor(bool bMotorized, f32 aMaxForce, f32 aVelocity)
{
	m_bMotor = bMotorized;
	m_maxMotorForce = aMaxForce;
	m_motorVelocity = aVelocity;

	NxMotorDesc motor;
	motor.maxForce = m_maxMotorForce;
	motor.freeSpin = NX_FALSE;
	motor.velTarget = m_motorVelocity;

	if (!m_pNxJoint)
		return;

	if (m_pNxJoint->isRevoluteJoint())
	{
		m_pNxJoint->isRevoluteJoint()->setMotor(motor);
	}
	else if (m_pNxJoint->isPulleyJoint())
	{
		m_pNxJoint->isPulleyJoint()->setMotor(motor);
	}
}

void PhysXJoint::addLimitPlane(
					const Vec3& rPlaneNormal,
					const Vec3& rPlanePoint,
					f32 aRestitution)
{
	if (m_pNxJoint)
	{
		m_pNxJoint->addLimitPlane(
						NxVec3(rPlaneNormal.x, rPlaneNormal.y, rPlaneNormal.z),
						NxVec3(rPlanePoint.x, rPlanePoint.y, rPlanePoint.z),
						aRestitution);
	}
}

void PhysXJoint::setLimitPoint(const Vec3& rPoint, bool bIsOnBody2)
{
	if (m_pNxJoint)
	{
		m_pNxJoint->setLimitPoint(NxVec3(rPoint.x, rPoint.y, rPoint.z), bIsOnBody2);
	}
}

void PhysXJoint::clearLimitPlanes()
{
	if (!m_pNxJoint)
		return;

	m_pNxJoint->purgeLimitPlanes();
}

//---

PhysXCylindricalJoint::PhysXCylindricalJoint() : PhysXJoint()
{
	m_type = eType_Cylindrical;
	m_pNxJointDesc = &m_jointDesc;
}

PhysXCylindricalJoint::~PhysXCylindricalJoint()
{
}

bool PhysXCylindricalJoint::create()
{
	if (!m_pBody1 || !m_pBody2)
		return false;

	m_jointDesc.setToDefault();
	m_jointDesc.actor[0] = ((PhysXBody*)m_pBody1)->m_pNxActor;
	m_jointDesc.actor[1] = ((PhysXBody*)m_pBody2)->m_pNxActor;
	m_jointDesc.setGlobalAnchor(NxVec3(m_translation.x, m_translation.y, m_translation.z));
	m_jointDesc.setGlobalAxis(NxVec3(m_localAxis.x, m_localAxis.y, m_localAxis.z));

	return PhysXJoint::create();
}

//---

PhysXUniversalJoint::PhysXUniversalJoint() : PhysXJoint()
{
	m_type = eType_Universal;
	m_pNxJointDesc = &m_jointDesc;
}

PhysXUniversalJoint::~PhysXUniversalJoint()
{
}

bool PhysXUniversalJoint::create()
{
	//TODO: create universal joint
	return false;
}

//---

PhysXDistanceJoint::PhysXDistanceJoint() : PhysXJoint()
{
	m_type = eType_Distance;
	m_pNxJointDesc = &m_jointDesc;
}

PhysXDistanceJoint::~PhysXDistanceJoint()
{
}

bool PhysXDistanceJoint::create()
{
	if (!m_pBody1 || !m_pBody2)
		return false;

	m_jointDesc.actor[0] = ((PhysXBody*)m_pBody1)->m_pNxActor;
	m_jointDesc.actor[1] = ((PhysXBody*)m_pBody2)->m_pNxActor;
	m_jointDesc.localAnchor[0] = NxVec3(
									m_localBodyAnchor[0].x,
									m_localBodyAnchor[0].y,
									m_localBodyAnchor[0].z);
	m_jointDesc.localAnchor[1] = NxVec3(
									m_localBodyAnchor[1].x,
									m_localBodyAnchor[1].y,
									m_localBodyAnchor[1].z);
	m_jointDesc.minDistance = m_minDistance;
	m_jointDesc.maxDistance = m_maxDistance;
	m_jointDesc.flags |=
				(m_bUseMinDistance ? NX_DJF_MIN_DISTANCE_ENABLED : 0)
				| (m_bUseMaxDistance ? NX_DJF_MAX_DISTANCE_ENABLED : 0);

	return PhysXJoint::create();
}

//---

PhysXFixedJoint::PhysXFixedJoint() : PhysXJoint()
{
	m_type = eType_Fixed;
	m_pNxJointDesc = &m_jointDesc;
}

PhysXFixedJoint::~PhysXFixedJoint()
{
}

bool PhysXFixedJoint::create()
{
	if (!m_pBody1 || !m_pBody2)
		return false;

	m_jointDesc.actor[0] = ((PhysXBody*)m_pBody1)->m_pNxActor;
	m_jointDesc.actor[1] = ((PhysXBody*)m_pBody2)->m_pNxActor;

	PhysXJoint::create();

	return true;
}

//---

PhysXPointInPlaneJoint::PhysXPointInPlaneJoint() : PhysXJoint()
{
	m_type = eType_PointInPlane;
	m_pNxJointDesc = &m_jointDesc;
}

PhysXPointInPlaneJoint::~PhysXPointInPlaneJoint()
{
}

bool PhysXPointInPlaneJoint::create()
{
	if (!m_pBody1 || !m_pBody2)
		return false;

	m_jointDesc.actor[0] = ((PhysXBody*)m_pBody1)->m_pNxActor;
	m_jointDesc.actor[1] = ((PhysXBody*)m_pBody2)->m_pNxActor;
	m_jointDesc.setGlobalAnchor(NxVec3(m_translation.x, m_translation.y, m_translation.z));
	m_jointDesc.setGlobalAxis(NxVec3(m_localAxis.x, m_localAxis.y, m_localAxis.z));
	m_jointDesc.jointFlags |= NX_JF_COLLISION_ENABLED;

	return PhysXJoint::create();
}

//---

PhysXPointOnLineJoint::PhysXPointOnLineJoint() : PhysXJoint()
{
	m_type = eType_PointOnLine;
	m_pNxJointDesc = &m_jointDesc;
}

PhysXPointOnLineJoint::~PhysXPointOnLineJoint()
{
}

bool PhysXPointOnLineJoint::create()
{
	if (!m_pBody1 || !m_pBody2)
		return false;

	m_jointDesc.actor[0] = ((PhysXBody*)m_pBody1)->m_pNxActor;
	m_jointDesc.actor[1] = ((PhysXBody*)m_pBody2)->m_pNxActor;
	m_jointDesc.setGlobalAnchor(NxVec3(m_translation.x, m_translation.y, m_translation.z));
	m_jointDesc.setGlobalAxis(NxVec3(m_localAxis.x, m_localAxis.y, m_localAxis.z));
	m_jointDesc.jointFlags |= NX_JF_COLLISION_ENABLED;

	return PhysXJoint::create();
}

//---

PhysXPrismaticJoint::PhysXPrismaticJoint() : PhysXJoint()
{
	m_type = eType_Prismatic;
	m_pNxJointDesc = &m_jointDesc;
}

PhysXPrismaticJoint::~PhysXPrismaticJoint()
{
}

bool PhysXPrismaticJoint::create()
{
	if (!m_pBody1 || !m_pBody2)
		return false;

	m_jointDesc.actor[0] = ((PhysXBody*)m_pBody1)->m_pNxActor;
	m_jointDesc.actor[1] = ((PhysXBody*)m_pBody2)->m_pNxActor;
	m_jointDesc.setGlobalAnchor(NxVec3(m_translation.x, m_translation.y, m_translation.z));
	m_jointDesc.setGlobalAxis(NxVec3(m_localAxis.x, m_localAxis.y, m_localAxis.z));

	return PhysXJoint::create();
}

//---

PhysXPulleyJoint::PhysXPulleyJoint() : PhysXJoint()
{
	m_type = eType_Pulley;
	m_pNxJointDesc = &m_jointDesc;
}

PhysXPulleyJoint::~PhysXPulleyJoint()
{
}

bool PhysXPulleyJoint::create()
{
	if (!m_pBody1 || !m_pBody2)
		return false;

	m_jointDesc.actor[0] = ((PhysXBody*)m_pBody1)->m_pNxActor;
	m_jointDesc.actor[1] = ((PhysXBody*)m_pBody2)->m_pNxActor;
	m_jointDesc.localAnchor[0] = NxVec3(
									m_localBodyAnchor[0].x,
									m_localBodyAnchor[0].y,
									m_localBodyAnchor[0].z);
	m_jointDesc.localAnchor[1] = NxVec3(
									m_localBodyAnchor[1].x,
									m_localBodyAnchor[1].y,
									m_localBodyAnchor[1].z);
	m_jointDesc.pulley[0] = NxVec3(m_pulley[0].x, m_pulley[0].y, m_pulley[0].z);
	m_jointDesc.pulley[1] = NxVec3(m_pulley[1].x, m_pulley[1].y, m_pulley[1].z);
	m_jointDesc.distance = m_ropeLength;
	m_jointDesc.stiffness = m_stiffness;
	m_jointDesc.ratio = m_transmissionRatio;
	m_jointDesc.flags |= NX_PJF_IS_RIGID;

	if (m_bMotor)
	{
		m_jointDesc.motor.maxForce = m_maxMotorForce;
		m_jointDesc.motor.freeSpin = NX_FALSE;
		m_jointDesc.motor.velTarget = m_motorVelocity;
		m_jointDesc.flags |= NX_RJF_MOTOR_ENABLED;
	}

	return PhysXJoint::create();
}

//---

PhysXRevoluteJoint::PhysXRevoluteJoint() : PhysXJoint()
{
	m_type = eType_Revolute;
	m_pNxJointDesc = &m_jointDesc;
}

PhysXRevoluteJoint::~PhysXRevoluteJoint()
{
}

bool PhysXRevoluteJoint::create()
{
	if (!m_pBody1 || !m_pBody2)
		return false;

	m_jointDesc.actor[0] = ((PhysXBody*)m_pBody1)->m_pNxActor;
	m_jointDesc.actor[1] = ((PhysXBody*)m_pBody2)->m_pNxActor;
	m_jointDesc.setGlobalAnchor(NxVec3(m_translation.x, m_translation.y, m_translation.z));
	m_jointDesc.setGlobalAxis(NxVec3(m_localAxis.x, m_localAxis.y, m_localAxis.z));

	if (m_bMotor)
	{
		m_jointDesc.motor.maxForce = m_maxMotorForce;
		m_jointDesc.motor.freeSpin = NX_FALSE;
		m_jointDesc.motor.velTarget = m_motorVelocity;
		m_jointDesc.flags |= NX_RJF_MOTOR_ENABLED;
	}

	if (m_bLimits)
	{
		m_jointDesc.limit.low.value = m_lowLimit.value;
		m_jointDesc.limit.low.restitution = m_lowLimit.restitution;
		m_jointDesc.limit.low.hardness = m_lowLimit.hardness;

		m_jointDesc.limit.high.value = m_highLimit.value;
		m_jointDesc.limit.high.restitution = m_highLimit.restitution;
		m_jointDesc.limit.high.hardness = m_highLimit.hardness;
	}

	return PhysXJoint::create();
}

//---

PhysXSphericalJoint::PhysXSphericalJoint() : PhysXJoint()
{
	m_type = eType_Spherical;
	m_pNxJointDesc = &m_jointDesc;
}

PhysXSphericalJoint::~PhysXSphericalJoint()
{
}

bool PhysXSphericalJoint::create()
{
	if (!m_pBody1 || !m_pBody2)
		return false;

	m_jointDesc.actor[0] = ((PhysXBody*)m_pBody1)->m_pNxActor;
	m_jointDesc.actor[1] = ((PhysXBody*)m_pBody2)->m_pNxActor;
	m_jointDesc.setGlobalAnchor(NxVec3(m_translation.x, m_translation.y, m_translation.z));

	return PhysXJoint::create();
}
}