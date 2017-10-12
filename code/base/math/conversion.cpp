// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include <wchar.h>
#include <string.h>
#include "base/math/conversion.h"
#include "base/math/util.h"

namespace base
{
void toAngleAxis(const Quat& from, f32& outAngle, Vec3& outAxis)
{
	f32  s, omega;

	omega = (f32) acosf(from.w) * 2.0f;
	s = sinf(omega * 0.5f);

	if (s < 0.000001f)
		s = 0.000001f;

	outAxis.x = from.x / s;
	outAxis.y = from.y / s;
	outAxis.z = from.z / s;
	outAngle = (omega * 180.f) / (f32) pi;
}

void toAngleAxis(const Quat& from, AngleAxis& to)
{
	toAngleAxis(from, to.angle, to.axis);
}

void toAngleAxis(const Matrix& from, AngleAxis& to)
{
	to = toAngleAxis(toQuat(from));
}

AngleAxis toAngleAxis(const Quat& from)
{
	AngleAxis aa;
	toAngleAxis(from, aa);
	return aa;
}

AngleAxis toAngleAxis(const Matrix& from)
{
	return toAngleAxis(toQuat(from));
}

//////////////////////////////////////////////////////////////////////////

void toMatrix(const Quat& from, f32 to[16])
{
	f32 w2 = from.w;
	f32 x2 = from.x;
	f32 y2 = from.y;
	f32 z2 = from.z;

	f32 xx = x2 * x2;
	f32 yy = y2 * y2;
	f32 zz = z2 * z2;

#define B_QMATRIX(x, y) to[x + y * 4]

	B_QMATRIX(0, 0) = 1.0f - 2.0f * (yy + zz);
	B_QMATRIX(1, 0) = 2.0f * (x2 * y2 + w2 * z2);
	B_QMATRIX(2, 0) = 2.0f * (x2 * z2 - w2 * y2);
	B_QMATRIX(3, 0) = 0.0f;

	B_QMATRIX(0, 1) = 2.0f * (x2 * y2 - w2 * z2);
	B_QMATRIX(1, 1) = 1.0f - 2.0f * (xx + zz);
	B_QMATRIX(2, 1) = 2.0f * (y2 * z2 + w2 * x2);
	B_QMATRIX(3, 1) = 0.0f;

	B_QMATRIX(0, 2) = 2.0f * (x2 * z2 + w2 * y2);
	B_QMATRIX(1, 2) = 2.0f * (y2 * z2 - w2 * x2);
	B_QMATRIX(2, 2) = 1.0f - 2.0f * (xx + yy);
	B_QMATRIX(3, 2) = 0.0f;

	B_QMATRIX(0, 3) = 0.0f;
	B_QMATRIX(1, 3) = 0.0f;
	B_QMATRIX(2, 3) = 0.0f;
	B_QMATRIX(3, 3) = 1.0f;

#undef B_QMATRIX
}

void toMatrix(const Quat& from, f64 to[16])
{
	f64 w2 = from.w;
	f64 x2 = from.x;
	f64 y2 = from.y;
	f64 z2 = from.z;

	f64 xx = x2 * x2;
	f64 yy = y2 * y2;
	f64 zz = z2 * z2;

#define B_QMATRIX(x, y) to[x + y * 4]

	B_QMATRIX(0, 0) = 1.0f - 2.0f * (yy + zz);
	B_QMATRIX(1, 0) = 2.0f * (x2 * y2 + w2 * z2);
	B_QMATRIX(2, 0) = 2.0f * (x2 * z2 - w2 * y2);
	B_QMATRIX(3, 0) = 0.0f;

	B_QMATRIX(0, 1) = 2.0f * (x2 * y2 - w2 * z2);
	B_QMATRIX(1, 1) = 1.0f - 2.0f * (xx + zz);
	B_QMATRIX(2, 1) = 2.0f * (y2 * z2 + w2 * x2);
	B_QMATRIX(3, 1) = 0.0f;

	B_QMATRIX(0, 2) = 2.0f * (x2 * z2 + w2 * y2);
	B_QMATRIX(1, 2) = 2.0f * (y2 * z2 - w2 * x2);
	B_QMATRIX(2, 2) = 1.0f - 2.0f * (xx + yy);
	B_QMATRIX(3, 2) = 0.0f;

	B_QMATRIX(0, 3) = 0.0f;
	B_QMATRIX(1, 3) = 0.0f;
	B_QMATRIX(2, 3) = 0.0f;
	B_QMATRIX(3, 3) = 1.0f;

#undef B_QMATRIX
}

void toMatrix(const Quat& from, Matrix& to)
{
	toMatrix(from, (f32*)&to.m);
}

void toMatrix(const AngleAxis& from, Matrix& to)
{
	Quat q;
	toQuat(from, q);
	toMatrix(q, to);
}

void toMatrix(const Euler& euler, Euler::RotationOrder order, Matrix& to)
{
	Quat q;
	Matrix m;
	
	toQuat(euler, order, q);
	toMatrix(q, to);
}

void toInvertedMatrix(const Quat& from, Matrix& to)
{
	f32 x2, y2, z2, wx, wy, wz, xx, xy, xz, yy, yz, zz;

	x2 = from.x + from.x;
	y2 = from.y + from.y;
	z2 = from.z + from.z;
	wx = from.w * x2;
	wy = from.w * y2;
	wz = from.w * z2;
	xx = from.x * x2;
	xy = from.x * y2;
	xz = from.x * z2;
	yy = from.y * y2;
	yz = from.y * z2;
	zz = from.z * z2;

	to.setIdentity();

	to.m[0][0] = 1.0f - (yy + zz);
	to.m[0][1] = xy - wz;
	to.m[0][2] = xz + wy;
	to.m[0][3] = 0.0f;

	to.m[1][0] = xy + wz;
	to.m[1][1] = 1.0f - (xx + zz);
	to.m[1][2] = yz - wx;
	to.m[1][3] = 0.0f;

	to.m[2][0] = xz - wy;
	to.m[2][1] = yz + wx;
	to.m[2][2] = 1.0f - (xx + yy);
	to.m[2][3] = 0.0f;
}

void toFloats(const Matrix& from, f32 to[16])
{
	memcpy((f32*)&from.m, to, sizeof(f32)*16);
}

Matrix toMatrix(const Quat& from)
{
	Matrix m;
	toMatrix(from, m);
	return m;
}

Matrix toMatrix(const AngleAxis& from)
{
	Matrix m;
	toMatrix(from, m);
	return m;
}

Matrix toInvertedMatrix(const Quat& from)
{
	Matrix m;
	toInvertedMatrix(from, m);
	return m;
}

//////////////////////////////////////////////////////////////////////////

void toQuat(const Matrix& from, Quat& to)
{
	f32 qa[4];
	f32 tr = from.m[0][0] + from.m[1][1] + from.m[2][2];

	if (tr > 0.0f)
	{
		f32 s = sqrtf(tr + 1.0f) * 2;
		qa[3] = 0.25f * s;
		qa[0] = (from.m[1][2] - from.m[2][1]) / s;
		qa[1] = (from.m[2][0] - from.m[0][2]) / s;
		qa[2] = (from.m[0][1] - from.m[1][0]) / s;
	}
	else
	{
		i32 i, j, k, nxt[3] = { 1, 2, 0 };

		i = 0;

		if (from.m[1][1] > from.m[0][0])
			i = 1;

		if (from.m[2][2] > from.m[i][i])
			i = 2;

		j = nxt[i];
		k = nxt[j];

		f32 s = sqrtf((from.m[i][i] - (from.m[j][j] + from.m[k][k])) + 1.0f);

		qa[i] = s * 0.5f;
		s = 0.5f / s;
		qa[3] = (from.m[j][k] - from.m[k][j]) * s;
		qa[j] = (from.m[i][j] + from.m[j][i]) * s;
		qa[k] = (from.m[i][k] + from.m[k][i]) * s;
	}

	to.x = qa[0];
	to.y = qa[1];
	to.z = qa[2];
	to.w = qa[3];
}

void toQuat(const Matrixd& from, Quat& to)
{
	f64 qa[4];
	f64 tr = from.m[0][0] + from.m[1][1] + from.m[2][2];

	if (tr > 0.0f)
	{
		f64 s = sqrtf(tr + 1.0f);
		qa[3] = s * 0.5f;
		s = 0.5f / s;
		qa[0] = (from.m[1][2] - from.m[2][1]) * s;
		qa[1] = (from.m[2][0] - from.m[0][2]) * s;
		qa[2] = (from.m[0][1] - from.m[1][0]) * s;
	}
	else
	{
		i32 i, j, k, nxt[3] = { 1, 2, 0 };

		i = 0;

		if (from.m[1][1] > from.m[0][0])
			i = 1;

		if (from.m[2][2] > from.m[i][i])
			i = 2;

		j = nxt[i];
		k = nxt[j];

		f64 s = sqrtf((from.m[i][i] - (from.m[j][j] + from.m[k][k])) + 1.0f);

		qa[i] = s * 0.5f;
		s = 0.5f / s;
		qa[3] = (from.m[j][k] - from.m[k][j]) * s;
		qa[j] = (from.m[i][j] + from.m[j][i]) * s;
		qa[k] = (from.m[i][k] + from.m[k][i]) * s;
	}

	to.x = qa[0];
	to.y = qa[1];
	to.z = qa[2];
	to.w = qa[3];
}

void toQuat(const AngleAxis& from, Quat& to)
{
	toQuat(from.angle, from.axis.x, from.axis.y, from.axis.z, to);
}

void toQuat(f32 fromAngle, f32 fromX, f32 fromY, f32 fromZ, Quat& to)
{
	f32 s, omega;

	omega = B_DEGREES_TO_RADIANS(fromAngle) * 0.5f;
	s = sinf(omega);
	to.w = cosf(omega);
	to.x = fromX * s;
	to.y = fromY * s;
	to.z = fromZ * s;
}

void toQuat(f32 fromAngle, const Vec3& fromAxis, Quat& to)
{
	toQuat(fromAngle, fromAxis.x, fromAxis.y, fromAxis.z, to);
}

void toQuat(const Euler& euler, Euler::RotationOrder order, Quat& to)
{
	f32 cr, cp, cy, sr, sp, sy, cpcy, spsy;
	f32 roll, pitch, yaw;

	roll = euler.x;
	pitch = euler.y;
	yaw = euler.z;

	cr = cosf(roll * 0.5f);
	cp = cosf(pitch * 0.5f);
	cy = cosf(yaw * 0.5f);

	sr = sinf(roll * 0.5f);
	sp = sinf(pitch * 0.5f);
	sy = sinf(yaw * 0.5f);

	cpcy = cp * cy;
	spsy = sp * sy;

	to.w = cr * cpcy + sr * spsy;
	to.x = sr * cpcy - cr * spsy;
	to.y = cr * sp * cy + sr * cp * sy;
	to.z = cr * cp * sy - sr * sp * cy;
}

Quat toQuat(const Matrix& from)
{
	Quat q;
	toQuat(from, q);
	return q;
}

Quat toQuat(const Matrixd& from)
{
	Quat q;
	toQuat(from, q);
	return q;
}

Quat toQuat(const AngleAxis& from)
{
	Quat q;
	toQuat(from, q);
	return q;
}

Quat toQuat(f32 fromAngle, f32 fromX, f32 fromY, f32 fromZ)
{
	Quat q;
	toQuat(fromAngle, fromX, fromY, fromZ, q);
	return q;
}

Quat toQuat(f32 fromAngle, const Vec3& fromAxis)
{
	Quat q;
	toQuat(fromAngle, fromAxis, q);
	return q;
}

//////////////////////////////////////////////////////////////////////////

void toEuler(const Quat& from, Euler& outEuler, Euler::RotationOrder& order)
{
	f32 matrix[3][3];
	f32 cx, sx;
	f32 cy, sy, yr;
	f32 cz, sz;
	
	//TODO: is this ok?
	order = Euler::RotationOrder::XYZ;

	matrix[0][0] = 1.0f - (2.0f * from.y * from.y) - (2.0f * from.z * from.z);
	matrix[1][0] = (2.0f * from.x * from.y) + (2.0f * from.w * from.z);
	matrix[2][0] = (2.0f * from.x * from.z) - (2.0f * from.w * from.y);
	matrix[2][1] = (2.0f * from.y * from.z) + (2.0f * from.w * from.x);
	matrix[2][2] = 1.0f - (2.0f * from.x * from.x) - (2.0f * from.y * from.y);

	sy = -matrix[2][0];
	cy = sqrtf(1.0f - (sy * sy));
	yr = (f32) atan2f(sy, cy);
	outEuler.y = (yr * 180.0f) / (f32) pi;

	if (sy != 1.0f && sy != -1.0f) //TODO: dont check floats like that
	{
		cx = matrix[2][2] / cy;
		sx = matrix[2][1] / cy;
		outEuler.x = ((f32) atan2f(sx, cx) * 180.0f) / (f32) pi;

		cz = matrix[0][0] / cy;
		sz = matrix[1][0] / cy;
		outEuler.z = ((f32) atan2f(sz, cz) * 180.0f) / (f32) pi;
	}
	else
	{
		matrix[1][1] = 1.0f - (2.0f * from.x * from.x) - (2.0f * from.z * from.z);
		matrix[1][2] = (2.0f * from.y * from.z) - (2.0f * from.w * from.x);
		cx = matrix[1][1];
		sx = -matrix[1][2];
		outEuler.x = ((f32) atan2f(sx, cx) * 180.0f) / (f32) pi;

		cz = 1.0f;
		sz = 0.0f;
		outEuler.z = ((f32) atan2f(sz, cz) * 180.0f) / (f32) pi;
	}
}

Euler toEuler(const Quat& from, Euler::RotationOrder& order)
{
	Euler euler;
	
	toEuler(from, euler, order);
	
	return euler;
}

}