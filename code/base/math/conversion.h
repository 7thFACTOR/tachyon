#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/math/vec3.h"
#include "base/math/quat.h"
#include "base/math/matrix.h"
#include "base/math/euler.h"
#include "base/math/angle_axis.h"

namespace base
{
B_EXPORT void toAngleAxis(const Quat& from, f32& outAngle, Vec3& outAxis);
B_EXPORT void toAngleAxis(const Quat& from, AngleAxis& to);
B_EXPORT void toAngleAxis(const Matrix& from, AngleAxis& to);
B_EXPORT AngleAxis toAngleAxis(const Quat& from);
B_EXPORT AngleAxis toAngleAxis(const Matrix& from);

B_EXPORT void toMatrix(const Quat& from, f32 to[16]);
B_EXPORT void toMatrix(const Quat& from, f64 to[16]);
B_EXPORT void toMatrix(const Quat& from, Matrix& to);
B_EXPORT void toMatrix(const AngleAxis& from, Matrix& to);
B_EXPORT void toMatrix(const Euler& euler, Euler::RotationOrder order, Matrix& to);
B_EXPORT void toInvertedMatrix(const Quat& from, Matrix& to);
B_EXPORT Matrix toMatrix(const Quat& from);
B_EXPORT Matrix toMatrix(const AngleAxis& from);
B_EXPORT Matrix toInvertedMatrix(const Quat& from);

B_EXPORT void toQuat(const Matrix& from, Quat& to);
B_EXPORT void toQuat(const Matrixd& from, Quat& to);
B_EXPORT void toQuat(const AngleAxis& from, Quat& to);
B_EXPORT void toQuat(f32 fromAngle, f32 fromX, f32 fromY, f32 fromZ, Quat& to);
B_EXPORT void toQuat(f32 fromAngle, const Vec3& fromAxis, Quat& to);
B_EXPORT void toQuat(const Euler& fromEuler, Euler::RotationOrder order, Quat& to);
B_EXPORT Quat toQuat(const Matrix& from);
B_EXPORT Quat toQuat(const Matrixd& from);
B_EXPORT Quat toQuat(const AngleAxis& from);
B_EXPORT Quat toQuat(f32 fromAngle, f32 fromX, f32 fromY, f32 fromZ);
B_EXPORT Quat toQuat(f32 fromAngle, const Vec3& fromAxis);

B_EXPORT void toEuler(const Quat& from, Euler::RotationOrder& outOrder, Euler& outEuler);
B_EXPORT Euler toEuler(const Quat& from, Euler::RotationOrder& outOrder);

}