// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "common.h"
#include "physics/shape.h"

namespace engine
{
class PhysXShape : public PhysicsShape
{
public:
	PhysXShape();
	~PhysXShape();

	void free();
	void setTranslation(f32 aX, f32 aY, f32 aZ);
	void setMaterial(PhysicsMaterial* pMaterial);

	PxShapeDesc* m_pNxShapeDesc;
	PxShape* m_pNxShape;
};

class PhysXBoxShape : public PhysXShape, public IBoxPhysicsShape
{
public:
	PhysXBoxShape();
	~PhysXBoxShape();

	bool create(f32 aWidth, f32 aHeight, f32 aDepth);
	f32 boxWidth() const;
	f32 boxHeight() const;
	f32 boxDepth() const;
	void getBox(BBox& rOutBox) const;

	NxBoxShapeDesc m_nxShapeDesc;
};

class PhysXCylinderShape : public PhysXShape, public ICylinderPhysicsShape
{
public:
	PhysXCylinderShape();
	~PhysXCylinderShape();

	bool create(f32 aRadius, f32 aHeight, u32 aSideCount = 30);
	f32 cylinderRadius() const;
	f32 cylinderHeight() const;
	u32 cylinderSideCount() const;

	NxConvexShapeDesc m_nxShapeDesc;
	f32 m_cylinderRadius, m_cylinderHeight;
	u32 m_cylinderSideCount;
};

class PhysXCapsuleShape : public PhysXShape, public ICapsulePhysicsShape
{
public:
	PhysXCapsuleShape();
	~PhysXCapsuleShape();

	bool create(f32 aRadius, f32 aHeight);
	f32 capsuleRadius() const;
	f32 capsuleHeight() const;

	NxCapsuleShapeDesc  m_nxShapeDesc;
};

class PhysXSphereShape : public PhysXShape, public ISpherePhysicsShape
{
public:
	PhysXSphereShape();
	~PhysXSphereShape();

	bool create(f32 aRadius);
	f32 sphereRadius() const;

	NxSphereShapeDesc m_nxShapeDesc;
};

class PhysXPlaneShape : public PhysXShape, public IPlanePhysicsShape
{
public:
	PhysXPlaneShape();
	~PhysXPlaneShape();

	bool create(const Vec3& rNormal, f32 aDistanceAlongNormal = 0);
	Vec3 planeNormal() const;
	f32 planeDistanceAlongNormal() const;

	NxPlaneShapeDesc m_nxShapeDesc;
};

class PhysXConvexHullShape : public PhysXShape, public IConvexHullPhysicsShape
{
public:
	PhysXConvexHullShape();
	~PhysXConvexHullShape();

	bool create(const Array<Vec3>& rPointCloud);

	NxConvexShapeDesc m_nxShapeDesc;
};

class PhysXTriangleMeshShape : public PhysXShape, public ITriangleMeshPhysicsShape
{
public:
	PhysXTriangleMeshShape();
	~PhysXTriangleMeshShape();

	bool create(GeometryBuffer* pGbTriMesh);

	NxTriangleMeshShapeDesc m_nxShapeDesc;
};

class PhysXHeightFieldShape : public PhysXShape, public IHeightFieldPhysicsShape
{
public:
	PhysXHeightFieldShape();
	~PhysXHeightFieldShape();

	bool create(const Table<f32>& rHeightField);

	NxHeightFieldShapeDesc m_nxShapeDesc;
};
}