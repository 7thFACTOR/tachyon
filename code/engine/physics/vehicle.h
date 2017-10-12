// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/array.h"
#include "base/defines.h"
#include "base/class_registry.h"
#include "base/math/vec3.h"
#include "base/math/quat.h"
#include "core/defines.h"

namespace engine
{
using namespace base;

class Body;
class Joint;

enum class VehicleWheelShape
{
	Cylinder,
	Sphere,
	System
};

//! A physics vehicle wheel
class E_API VehicleWheel
{
public:
	VehicleWheel();

	virtual void createWheel(
		Body* parentBody,
		Joint* motor,
		const Vec3& position,
		const Quat& rotation,
		VehicleWheelShape shapeType,
		f32 wheelRadius,
		f32 wheelTickness);

protected:
	Vec3 translation;
	Quat rotation;
	f32 wheelRadius, wheelTickness;
	VehicleWheelShape shapeType;
};

//! A vehicle motor class holder
struct VehicleMotor
{
	f32 maxSpeed = 200; // kmph
	f32 damageDrag = 1.0f; // unit factor
	f32 randomDamageFail = 1.0f;
	f32 acceleration = 1.0f;
	f32 currentSpeed = 0.0f; // kmph
};

//! A physics vehicle, with unlimited wheels
class E_API Vehicle
{
public:
	Vehicle();
	virtual ~Vehicle();

	const Array<VehicleWheel*>& getWheels() const;

protected:
	Array<VehicleWheel*> wheels;
	VehicleMotor motor;
};

}