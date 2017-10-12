// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/math/vec3.h"
#include "base/math/quat.h"
#include "base/math/matrix.h"
#include "logic/component.h"

namespace engine
{
using namespace base;

enum class TransformFlags
{
	None,
	//! if set, the final object rotation will be computed from the object's look direction
	UseLookDirection = B_BIT(0),
	//! if set, the roll angle will be added to the final object rotation
	UseRollAngle = B_BIT(1),
	//! does this object inherits its parent scale
	InheritParentScale = B_BIT(2),
	//! does this object inherits its parent rotation
	InheritParentRotation = B_BIT(3)
};
B_ENUM_AS_FLAGS(TransformFlags);

struct E_API TransformComponent : Component
{
	static const ComponentTypeId typeId = StdComponentTypeId_Transform;

	E_HAS_PROPERTIES

	TransformComponent() {}
	TransformComponent* parent = nullptr;
	TransformComponent* lookAt = nullptr;
	bool useParentTransform = true;
	BigVec3 translation = { 0.0f, 0.0f, 0.0f };
	Vec3 cameraRelativeTranslation = { 0.0f, 0.0f, 0.0f };
	Vec3 scale = { 1.0f, 1.0f, 1.0f };
	Quat rotation;
	TransformFlags flags = TransformFlags::InheritParentRotation | TransformFlags::InheritParentScale;
	Vec3 lookDirection = { 0.0f, 0.0f, 1.0f };
	Vec3 upAxis = { 0.0f, 1.0f, 0.0f };
	Vec3 rightAxis = { 1.0f, 0.0f, 0.0f };
	//! the roll angle around the local Z axis
	f32 roll = 0.0f;
	Matrix worldMatrix;
	Matrix localMatrix;
	Matrix parentSpaceMatrix;
	Matrixf cameraLocalSpaceWorldMatrix;
	//! the world translation inherited from parent to child
	Vec3 hierarchyTranslation;
	//! the world scale inherited from parent to child
	Vec3 hierarchyScale;
	//! the world rotation inherited from parent to child
	Quat hierarchyRotation;
	f32 distanceToCamera = 0.0f;
	u32 renderLayerIndex = 0;
	u32 levelOfDetail = 0;
};

}