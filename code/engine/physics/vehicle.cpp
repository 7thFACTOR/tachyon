// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "physics/vehicle.h"

namespace engine
{
VehicleWheel::VehicleWheel()
{
	wheelRadius = 1;
	wheelTickness = 0.3f;
	shapeType = VehicleWheelShape::System;
}

void VehicleWheel::createWheel(
		Body* parentBody,
		Joint* motor,
		const Vec3& position,
		const Quat& rotation,
		VehicleWheelShape shapeType,
		f32 wheelRadius,
		f32 wheelTickness)
{}

//---

Vehicle::Vehicle()
{}

Vehicle::~Vehicle()
{}

const Array<VehicleWheel*>& Vehicle::getWheels() const
{
	return wheels;
}

}