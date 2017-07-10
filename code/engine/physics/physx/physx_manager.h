#pragma once
#include "common.h"
#include "physics/space.h"
#include "physics/physics_manager.h"
#include "base/mutex.h"
#include "base/array.h"
#include "base/table.h"
#include "physics/physics_material.h"
#include "physics/space.h"

namespace engine
{
class PhysXManager;
class PhysXSpace;
class PhysXBody;

class PhysXMaterial : public PhysicsMaterial
{
public:
	PhysXMaterial();
	~PhysXMaterial();

	void update();

	NxMaterial* m_pNxMaterial;
};

class PhysXUserContactReport : public NxUserContactReport
{
public:
	void onContactNotify(NxContactPair& pair, NxU32 events);

	PhysXSpace* m_pSpace;
	Array<PhysicsContactPairInfo> m_contacts;
};

class PhysXSpace : public PhysicsSpace
{
public:
	PhysXSpace();
	~PhysXSpace();

	bool create();
	void free();
	void setListener(PhysicsEventListener* pHandler);
	void setGravity(f32 aX, f32 aY, f32 aZ);
	void setGravity(const Vec3& rGravity);

	PhysicsBody* createBody();
	PhysicsCharacter* createCharacter();
	PhysicsMaterial* createMaterial();
	PhysicsJoint* createJoint(PhysicsJoint::EType aType);
	PhysicsForceField* createForceField(PhysicsForceField::EType aType);
	PhysicsCloth* createCloth();
	PhysicsRagDoll* createRagDoll();
	PhysicsRope* createRope();
	PhysicsSoftBody* createSoftBody();
	PhysicsVehicle* createVehicle();
	PhysicsShape* createBoxShape(f32 aWidth, f32 aHeight, f32 aDepth);
	PhysicsShape* createCylinderShape(f32 aRadius, f32 aHeight, size_t aSegments);
	PhysicsShape* createCapsuleShape(f32 aRadius, f32 aHeight);
	PhysicsShape* createSphereShape(f32 aRadius);
	PhysicsShape* createPlaneShape(const Vec3& rPlaneNormal, f32 aDistanceAlongNormal = 0);
	PhysicsShape* createConvexHullShape(const Array<Vec3>& rPointCloud);
	PhysicsShape* createTriangleMeshShape(GeometryBuffer* pMesh);
	PhysicsShape* createHeightFieldShape(const Table<f32>& rHeightMap);
	PhysXBody* getPhysXBodyByNxActor(NxActor* pActor);

	NxScene* m_pNxScene;
	PhysXManager* m_pNxProvider;
	PhysXUserContactReport m_onContactReport;
};

extern Mutex s_simulationMutex;

class PhysXManager : public PhysicsManager
{
public:
	B_RUNTIME_CLASS(PhysXManager)
	PhysXManager();
	~PhysXManager();

	PhysicsSpace* newPhysicsSpace();
	bool initialize();
	void simulate(f32 aStep);

	NxPhysicsSDK* m_pNxPhysicsSDK;
	NxCookingInterface* m_pNxCooking;
	NxControllerManager* m_pNxCtrlManager;
};
}