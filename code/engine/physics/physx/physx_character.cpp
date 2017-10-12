// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "common.h"
#include "physx_character.h"
#include "physx_manager.h"
#include "core/core.h"

namespace engine
{
#define MAX_NB_PTS  100
static NxU32 gNbPts = 0;
static NxVec3 gPts[MAX_NB_PTS];

static class PhysXControllerHitReport : public NxUserControllerHitReport
{
public:
	virtual NxControllerAction onShapeHit(const NxControllerShapeHit& hit)
	{
		if (hit.shape)
		{
			NxCollisionGroup group = hit.shape->getGroup();

			if (group == ePhysXGroup_Collidable)
			{
				NxActor& actor = hit.shape->getActor();

				if (actor.isDynamic())
				{
					// We only allow horizontal pushes. Vertical pushes when we stand on dynamic objects creates
					// useless stress on the solver. It would be possible to enable/disable vertical pushes on
					// particular objects, if the gameplay requires it.
					if (hit.dir.y == 0.0f)
					{
						NxF32 coeff = actor.getMass() * hit.length * 10.0f;
						actor.addForceAtLocalPos(hit.dir * coeff, NxVec3(0, 0, 0), NX_IMPULSE);
					}
				}
			}
		}

		return NX_ACTION_NONE;
	}

	virtual NxControllerAction onControllerHit(const NxControllersHit& hit)
	{
		return NX_ACTION_NONE;
	}

} s_controllerHitReport;

PhysXCharacter::PhysXCharacter()
{
	m_lastCollFlags = 0;
}

PhysXCharacter::~PhysXCharacter()
{
	NxControllerManager* pMan = ((PhysXManager*)&physics())->m_pNxCtrlManager;

	if (m_pNxCtrl && pMan)
	{
		pMan->releaseController(*m_pNxCtrl);
	}
}

bool PhysXCharacter::create(f32 aCapsuleRadius, f32 aCapsuleHeight)
{
	NxCapsuleControllerDesc desc;

	desc.setToDefault();
	desc.radius = aCapsuleRadius;
	desc.height = aCapsuleHeight;
	desc.callback = &s_controllerHitReport;

	m_pNxCtrl = (NxCapsuleController*)((PhysXManager*)&physics())
														->m_pNxCtrlManager->createController(m_pNxScene, desc);

	B_ASSERT(m_pNxCtrl);

	if (!m_pNxCtrl)
	{
		return false;
	}

	m_pNxCtrl->setCollision(false);

	return true;
}

void PhysXCharacter::moveCharacter(
						const Vec3& rDelta,
						f32 aMinDistance,
						f32 aAutoStepSharpness)
{
	B_ASSERT(m_pNxCtrl);

	if (!m_pNxCtrl)
		return;

	NxU32 flags = 0;

	m_pNxCtrl->move(
				NxVec3(rDelta.x, rDelta.y, rDelta.z),
				-1,
				aMinDistance,
				flags,
				aAutoStepSharpness);
	m_lastCollFlags = flags;
}

void PhysXCharacter::enableCollisions(bool bEnable)
{
	B_ASSERT(m_pNxCtrl);

	if (!m_pNxCtrl)
		return;

	m_pNxCtrl->setCollision(bEnable);
}

Vec3 PhysXCharacter::translation() const
{
	B_ASSERT(m_pNxCtrl);

	if (!m_pNxCtrl)
		return Vec3();

	NxExtendedVec3 ev;

	ev = m_pNxCtrl->getPosition();

	return Vec3((f32)ev.x, (f32)ev.y, (f32)ev.z);
}

void PhysXCharacter::setTranslation(const Vec3& rPos)
{
	B_ASSERT(m_pNxCtrl);

	if (!m_pNxCtrl)
		return;

	NxExtendedVec3 ev(rPos.x, rPos.y, rPos.z);

	m_pNxCtrl->setPosition(ev);
}

void PhysXCharacter::setCapsuleHeight(f32 aHeight)
{
	m_pNxCtrl->setHeight(aHeight);
}

void PhysXCharacter::setCapsuleRadius(f32 aRadius)
{
	m_pNxCtrl->setRadius(aRadius);
}

f32 PhysXCharacter::capsuleHeight() const
{
	return m_pNxCtrl->getHeight();
}

f32 PhysXCharacter::capsuleRadius() const
{
	return m_pNxCtrl->getRadius();
}

bool PhysXCharacter::isCapsuleWorldOverlapping(f32 aRadius, f32 aHeight) const
{
	NxExtendedVec3 pos = m_pNxCtrl->getPosition();
	NxCapsule worldCapsule;

	worldCapsule.p0 = NxVec3(pos.x, pos.y - aHeight / 2.0f, pos.z);
	worldCapsule.p1 = NxVec3(pos.x, pos.y + aHeight / 2.0f, pos.z);
	worldCapsule.radius = aRadius;
	m_pNxCtrl->setCollision(false);

	bool bOverlap = false;

	if (m_pNxScene->checkOverlapCapsule(worldCapsule))
	{
		bOverlap = true;
	}

	m_pNxCtrl->setCollision(true);

	return bOverlap;
}

bool PhysXCharacter::checkGroundIntersection(
						f32* pOutDist,
						Vec3* pOutWorldPt,
						Vec3* pOutWorldNormal) const
{
	NxExtendedVec3 pos = m_pNxCtrl->getPosition();
	NxRaycastHit hit;

	pos.y -= capsuleHeight();
	memset(&hit, 0, sizeof(hit));

	NxShape* pShp;

	pShp = m_pNxScene->raycastClosestShape(
							NxRay(NxVec3(pos.x, pos.y, pos.z), NxVec3(0, -1, 0)), NX_ALL_SHAPES, hit);

	if (!pShp
		|| &(pShp->getActor()) == m_pNxCtrl->getActor())
	{
		return false;
	}

	if (pOutDist)
	{
		*pOutDist = hit.distance;
	}

	if (pOutWorldPt)
	{
		*pOutWorldPt = Vec3(hit.worldImpact.x, hit.worldImpact.y, hit.worldImpact.z);
	}

	if (pOutWorldNormal)
	{
		*pOutWorldNormal = Vec3(hit.worldNormal.x, hit.worldNormal.y, hit.worldNormal.z);
	}

	return true;
}

bool PhysXCharacter::isCollidingSides() const
{
	return (m_lastCollFlags & NXCC_COLLISION_SIDES) != 0;
}

bool PhysXCharacter::isCollidingGround() const
{
	return (m_lastCollFlags & NXCC_COLLISION_DOWN) != 0;
}

bool PhysXCharacter::isCollidingUp() const
{
	return (m_lastCollFlags & NXCC_COLLISION_UP) != 0;
}
}