// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "common.h"
#include "physx_body.h"
#include "physx_shape.h"
#include "physx_manager.h"
#include "physics/space.h"
#include "physics/physics_manager.h"

namespace engine
{
PhysXBody::PhysXBody() : PhysicsBody()
{
	m_pNxActor = nullptr;
	m_pPhysicsSpace = nullptr;
}

PhysXBody::~PhysXBody()
{
	free();
}

void PhysXBody::freeShapes()
{
	for (u32 i = 0, iCount = m_shapes.size(); i < iCount; ++i)
	{
		delete m_shapes[i];
	}

	m_shapes.clear();
	m_nxShapesMap.clear();
}

bool PhysXBody::create()
{
	NxActorDesc actorDesc;
	NxBodyDesc  bodyDesc;

	bodyDesc.setToDefault();
	actorDesc.setToDefault();
	actorDesc.body = &bodyDesc;
	bodyDesc.mass = m_mass;
	actorDesc.group = ePhysXGroup_Collidable;

	if (m_bodyBehaviorType == eBehavior_Kinematic)
	{
		actorDesc.density = 1;
		bodyDesc.mass = 0;
	}

	if (m_bodyBehaviorType == eBehavior_Dynamic)
	{
		actorDesc.density = 0;
	}

	if (m_bodyBehaviorType == eBehavior_Static)
	{
		actorDesc.body = nullptr;
	}

	for (u32 i = 0, iCount = m_shapes.size(); i < iCount; ++i)
	{
		((PhysXShape*)m_shapes[i])->m_pNxShapeDesc->skinWidth = 0.001f;
		actorDesc.shapes.push_back(((PhysXShape*)m_shapes[i])->m_pNxShapeDesc);
	}

	B_ASSERT(((PhysXSpace*)m_pPhysicsSpace));
	B_ASSERT(((PhysXSpace*)m_pPhysicsSpace)->m_pNxScene);
	s_simulationMutex.lock();
	m_pNxActor = ((PhysXSpace*)m_pPhysicsSpace)->m_pNxScene->createActor(actorDesc);
	s_simulationMutex.unlock();
	B_ASSERT(m_pNxActor);

	if (!m_pNxActor)
	{
		return false;
	}

	for (u32 i = 0, iCount = m_shapes.size(); i < iCount; ++i)
	{
		((PhysXShape*)m_shapes[i])->m_pNxShape = m_pNxActor->getShapes()[i];
		m_pNxActor->getShapes()[i]->setFlag(NX_SF_POINT_CONTACT_FORCE, true);
		((PhysXShape*)m_shapes[i])->m_pNxShape->setLocalPosition(
													NxVec3(
														m_shapes[i]->translation().x,
														m_shapes[i]->translation().y,
														m_shapes[i]->translation().z));
		m_nxShapesMap[m_shapes[i]] = ((PhysXShape*)m_shapes[i])->m_pNxShape;
	}

	if (m_bodyBehaviorType == eBehavior_Dynamic
		&& !m_shapes.isEmpty())
	{
		m_pNxActor->updateMassFromShapes(0.0f, m_mass);
		m_pNxActor->setContactReportFlags(NX_NOTIFY_ON_TOUCH);
	}

	return true;
}

void PhysXBody::free()
{
	Array<PhysicsBody*>::Iterator iter =
		((const PhysXSpace*)m_pPhysicsSpace)->bodies().find(this);

	freeShapes();

	if (iter != ((PhysXSpace*)m_pPhysicsSpace)->bodies().end())
	{
		((PhysXSpace*)m_pPhysicsSpace)->removeBody(*iter);
	}

	if (m_pNxActor)
	{
		((PhysXSpace*)m_pPhysicsSpace)->m_pNxScene->releaseActor(*m_pNxActor);
		m_pNxActor = nullptr;
	}
}

void PhysXBody::getTransform(Vec3& rPos, Quat& rQuat)
{
	B_ASSERT(m_pNxActor);

	if (!m_pNxActor)
		return;

	NxVec3 pos;
	NxQuat rot;

	pos = m_pNxActor->getGlobalPosition();
	rot = m_pNxActor->getGlobalOrientationQuat();
	rPos.set(pos.x, pos.y, pos.z);
	rQuat.x = rot.x;
	rQuat.y = rot.y;
	rQuat.z = rot.z;
	rQuat.w = rot.w;
}

void PhysXBody::setTranslation(const Vec3& rPos)
{
	if (!m_pNxActor)
		return;

	if (m_bodyBehaviorType == eBehavior_Kinematic)
	{
		m_pNxActor->moveGlobalPosition(NxVec3(rPos.x, rPos.y, rPos.z));
	}
	else
	{
		m_pNxActor->setGlobalPosition(NxVec3(rPos.x, rPos.y, rPos.z));
	}
}

void PhysXBody::addShape(PhysicsShape* pShape)
{
	pShape->setParentBody(this);
	m_shapes.append(pShape);

	if (m_pNxActor)
	{
		NxShape* pNxShape = m_pNxActor->createShape(*(((PhysXShape*)pShape)->m_pNxShapeDesc));
		B_ASSERT(pNxShape);
		((PhysXShape*)pShape)->m_pNxShape = pNxShape;
		m_nxShapesMap[pShape] = pNxShape;
	}
}

void PhysXBody::removeShape(PhysicsShape* pShape)
{
	B_ASSERT(m_pNxActor);

	if (m_nxShapesMap[pShape])
	{
		Map<PhysicsShape*, NxShape*>::Iterator iter = m_nxShapesMap.find(pShape);
		Array<PhysicsShape*>::Iterator iterShape = m_shapes.find(pShape);

		if (iter != m_nxShapesMap.end()
			&& iterShape != m_shapes.end())
		{
			m_nxShapesMap.erase(iter);
			m_shapes.erase(iterShape);
			// rebuild actor
			create();
		}
	}
}

void PhysXBody::addForce(const Vec3& rValue)
{
	B_ASSERT(m_pNxActor);
	NxVec3 force(rValue.x, rValue.y, rValue.z);
	m_pNxActor->addForce(force, NX_IMPULSE);
}

void PhysXBody::addForceAtPosition(const Vec3& rForce, const Vec3& rValue)
{
	B_ASSERT(m_pNxActor);
	NxVec3 force(rForce.x, rForce.y, rForce.z);
	NxVec3 value(rValue.x, rValue.y, rValue.z);
	m_pNxActor->addForceAtPos(force, value, NX_IMPULSE);
}

void PhysXBody::addTorque(const Vec3& rValue)
{
	B_ASSERT(m_pNxActor);
	NxVec3 value(rValue.x, rValue.y, rValue.z);
	m_pNxActor->addTorque(value, NX_IMPULSE);
}

void PhysXBody::wakeUp(bool bValue)
{
	B_ASSERT(m_pNxActor);
	PhysicsBody::wakeUp(bValue);
	m_pNxActor->wakeUp(m_autoSleepTime ? m_autoSleepTime : NX_SLEEP_INTERVAL);
}

void PhysXBody::setEnableGravity(bool bValue)
{
	B_ASSERT(m_pNxActor);
	PhysicsBody::setEnableGravity(bValue);

	if (bValue)
	{
		m_pNxActor->raiseBodyFlag(NX_BF_DISABLE_GRAVITY);
	}
	else
	{
		m_pNxActor->clearBodyFlag(NX_BF_DISABLE_GRAVITY);
	}
}

void PhysXBody::setBodyBehaviorType(EBehavior aValue)
{
	PhysicsBody::setBodyBehaviorType(aValue);
}

void PhysXBody::setAutoSleepTime(f32 aValue)
{
	B_ASSERT(m_pNxActor);
	PhysicsBody::setAutoSleepTime(aValue);
	m_pNxActor->wakeUp(aValue ? aValue : NX_SLEEP_INTERVAL);
}

void PhysXBody::setAutoSleep(bool bValue)
{
	B_ASSERT(m_pNxActor);
	PhysicsBody::setAutoSleep(bValue);
	m_pNxActor->wakeUp(m_autoSleepTime ? m_autoSleepTime : NX_SLEEP_INTERVAL);
}

void PhysXBody::setMass(f32 aMass)
{
	B_ASSERT(m_pNxActor);
	PhysicsBody::setMass(aMass);

	if (m_pNxActor)
	{
		if (m_bodyBehaviorType == eBehavior_Dynamic)
		{
			m_pNxActor->updateMassFromShapes((NxReal)(aMass <= 0 ? 1 : 0), (NxReal)(aMass <= 0 ? 0 : aMass));
		}
	}
}

void PhysXBody::addRelativeForce(const Vec3& rValue)
{
	B_ASSERT(m_pNxActor);
	NxVec3 value(rValue.x, rValue.y, rValue.z);
	m_pNxActor->addLocalForce(value, NX_IMPULSE);
}

void PhysXBody::addRelativeTorque(const Vec3& rValue)
{
	B_ASSERT(m_pNxActor);
	NxVec3 value(rValue.x, rValue.y, rValue.z);
	m_pNxActor->addLocalTorque(value, NX_IMPULSE);
}

void PhysXBody::addForceAtRelativePosition(const Vec3& rForce, const Vec3& rPos)
{
	B_ASSERT(m_pNxActor);
	NxVec3 force(rForce.x, rForce.y, rForce.z);
	NxVec3 pos(rPos.x, rPos.y, rPos.z);
	m_pNxActor->addForceAtLocalPos(force, pos, NX_IMPULSE);
}

void PhysXBody::addRelativeForceAtPosition(const Vec3& rForce, const Vec3& rPos)
{
	B_ASSERT(m_pNxActor);
	NxVec3 force(rForce.x, rForce.y, rForce.z);
	NxVec3 pos(rPos.x, rPos.y, rPos.z);
	m_pNxActor->addLocalForceAtPos(force, pos, NX_IMPULSE);
}

void PhysXBody::addRelativeForceAtRelativePosition(const Vec3& rForce, const Vec3& rPos)
{
	B_ASSERT(m_pNxActor);
	NxVec3 force(rForce.x, rForce.y, rForce.z);
	NxVec3 pos(rPos.x, rPos.y, rPos.z);
	m_pNxActor->addLocalForceAtLocalPos(force, pos, NX_IMPULSE);
}

void PhysXBody::setLinearVelocity(f32 aX, f32 aY, f32 aZ)
{
	B_ASSERT(m_pNxActor);
	NxVec3 force(aX, aY, aZ);
	m_pNxActor->setLinearVelocity(force);
}

void PhysXBody::setLinearVelocity(const Vec3& rValue)
{
	B_ASSERT(m_pNxActor);
	NxVec3 force(rValue.x, rValue.y, rValue.z);
	m_pNxActor->setLinearVelocity(force);
}

void PhysXBody::setAngularVelocity(f32 aX, f32 aY, f32 aZ)
{
	B_ASSERT(m_pNxActor);
	NxVec3 force(aX, aY, aZ);
	m_pNxActor->setAngularVelocity(force);
}

void PhysXBody::setAngularVelocity(const Vec3& rValue)
{
	B_ASSERT(m_pNxActor);
	NxVec3 force(rValue.x, rValue.y, rValue.z);
	m_pNxActor->setAngularVelocity(force);
}

bool PhysXBody::isGravityEnabled() const
{
	B_ASSERT(m_pNxActor);

	return m_pNxActor->readBodyFlag(NX_BF_DISABLE_GRAVITY);
}

bool PhysXBody::isSleeping() const
{
	if (!m_pNxActor)
		return false;

	return m_pNxActor->isSleeping();
}

f32 PhysXBody::autoSleepTime() const
{
	return m_autoSleepTime;
}

bool PhysXBody::isAutoSleeping() const
{
	return m_bAutoSleep;
}

Vec3 PhysXBody::linearVelocity() const
{
	if (!m_pNxActor)
		return Vec3();

	NxVec3 value = m_pNxActor->getLinearVelocity();

	return Vec3(value.x, value.y, value.z);
}

Vec3 PhysXBody::angularVelocity() const
{
	if (!m_pNxActor)
		return Vec3();

	NxVec3 value = m_pNxActor->getAngularVelocity();

	return Vec3(value.x, value.y, value.z);
}

void PhysXBody::setRotation(const Quat& rQuat)
{
	NxQuat quat;

	if (!m_pNxActor)
		return;

	quat.x = rQuat.x;
	quat.y = rQuat.y;
	quat.z = rQuat.z;
	quat.w = rQuat.w;
	m_pNxActor->setGlobalOrientationQuat(quat);
}

void PhysXBody::setTransform(const Vec3& rPos, const Quat& rQuat)
{
	setTranslation(rPos);
	setRotation(rQuat);
}
}