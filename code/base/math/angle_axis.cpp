#include "base/math/angle_axis.h"
#include "base/math/conversion.h"

namespace base
{
AngleAxis::AngleAxis()
{
	angle = 0;
	axis.makeZero();
}

AngleAxis::~AngleAxis()
{}

AngleAxis::AngleAxis(f32 angle, f32 x, f32 y, f32 z)
{
	set(angle, x, y, z);
}

AngleAxis::AngleAxis(f32 angle, const Vec3& axis)
{
	set(angle, axis);
}

AngleAxis::AngleAxis(const AngleAxis& angleAxis)
{
	set(angleAxis);
}

AngleAxis::AngleAxis(const Matrix& matrix)
{
	set(matrix);
}

AngleAxis::AngleAxis(const Quat& quat)
{
	set(quat);
}

void AngleAxis::set(f32 newAngle, f32 x, f32 y, f32 z)
{
	angle = newAngle;
	axis.set(x, y, z);
}

void AngleAxis::set(f32 newAngle, const Vec3& newAxis)
{
	angle = newAngle;
	axis = newAxis;
}

void AngleAxis::set(const AngleAxis& angleAxis)
{
	angle = angleAxis.angle;
	axis = angleAxis.axis;
}

void AngleAxis::set(const Matrix& matrix)
{
	set(toAngleAxis(matrix));
}

void AngleAxis::set(const Quat& quat)
{
	set(toAngleAxis(quat));
}

}