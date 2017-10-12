// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/types.h"
#include "base/defines.h"
#include "base/class_registry.h"
#include "logic/components/render/transform.h"
#include "base/table.h"
#include "base/math/bbox.h"

namespace engine
{
class PhysicsMaterial;
class PhysicsBody;
class Mesh;

enum class PhysicsShapeType
{
	Box,
	Cylinder,
	Capsule,
	Sphere,
	Plane,
	ConvexHull,
	TriangleMesh,
	HeightField,
	
	Count
};

//! This class is a physic shape, used for collision
class E_API PhysicsShape
{
public:
	virtual ~PhysicsShape();

	//! \return the shape's type
	PhysicsShapeType getType() const;
	//! \return the shape's current assigned material
	PhysicsMaterial* getMaterial() const;
	//! \return the parent body for this shape, a body is made of one or more shapes
	PhysicsBody* getParentBody() const;
	const Vec3& getTranslation() const;
	const Quat& getRotation() const;
	//! set shape's type
	virtual void setShapeType(const PhysicsShapeType value);
	//! set shape's material
	virtual void setMaterial(PhysicsMaterial* material);
	//! set shape's parent body
	virtual void setParentBody(PhysicsBody* body);

protected:
	//! the physics shape can only be created through the PhysicsSpace
	PhysicsShape();

	//! physics shape type
	PhysicsShapeType shapeType;
	//! the assigned physics material
	PhysicsMaterial* material;
	//! the shape's parent body
	PhysicsBody* parentBody;
	Vec3 translation, scale;
	Quat rotation;
};

struct E_API BoxPhysicsShape
{
	virtual ~BoxPhysicsShape(){}
	virtual bool create(f32 width, f32 height, f32 depth) = 0;
	virtual f32 getBoxWidth() const = 0;
	virtual f32 getBoxHeight() const = 0;
	virtual f32 getBoxDepth() const = 0;
	virtual void getBox(BBox& outBox) const = 0;
};

struct E_API CylinderPhysicsShape
{
	virtual ~CylinderPhysicsShape(){}
	virtual bool create(f32 radius, f32 height, u32 sideCount) = 0;
	virtual f32 getCylinderRadius() const = 0;
	virtual f32 getCylinderHeight() const = 0;
	virtual u32 getCylinderSideCount() const = 0;
};

struct E_API CapsulePhysicsShape
{
	virtual ~CapsulePhysicsShape(){}
	virtual bool create(f32 radius, f32 height) = 0;
	virtual f32 getCapsuleRadius() const = 0;
	virtual f32 getCapsuleHeight() const = 0;
};

struct E_API SpherePhysicsShape
{
	virtual ~SpherePhysicsShape(){}
	virtual bool create(f32 radius) = 0;
	virtual f32 getSphereRadius() const = 0;
};

struct E_API PlanePhysicsShape
{
	virtual ~PlanePhysicsShape(){}
	virtual bool create(const Vec3& normal, f32 distanceAlongNormal = 0) = 0;
	virtual Vec3 getPlaneNormal() const = 0;
	virtual f32 getPlaneDistanceAlongNormal() const = 0;
};

struct E_API ConvexHullPhysicsShape
{
	virtual ~ConvexHullPhysicsShape(){}
	virtual bool create(const Array<Vec3>& pointCloud) = 0;
};

struct E_API TriangleMeshPhysicsShape
{
	virtual ~TriangleMeshPhysicsShape(){}
	virtual bool create(Mesh* triMesh) = 0;
};

struct E_API HeightFieldPhysicsShape
{
	virtual ~HeightFieldPhysicsShape(){}
	virtual bool create(const TableF32& heightField) = 0;
};

}