#include "base/types.h"
#include "physics/shape.h"

namespace engine
{
PhysicsShape::PhysicsShape()
{
	parentBody = nullptr;
	material = nullptr;
	scale.set(1.0f, 1.0f, 1.0f);
}

PhysicsShape::~PhysicsShape()
{}

PhysicsMaterial* PhysicsShape::getMaterial() const
{
	return material;
}

void PhysicsShape::setMaterial(PhysicsMaterial* newMaterial)
{
	material = newMaterial;
}

PhysicsShapeType PhysicsShape::getType() const
{
	return shapeType;
}

void PhysicsShape::setShapeType(PhysicsShapeType value)
{
	shapeType = value;
}

void PhysicsShape::setParentBody(PhysicsBody* body)
{
	parentBody = body;
}

PhysicsBody* PhysicsShape::getParentBody() const
{
	return parentBody;
}

const Vec3& PhysicsShape::getTranslation() const
{
	return translation;
}

const Quat& PhysicsShape::getRotation() const
{
	return rotation;
}

}