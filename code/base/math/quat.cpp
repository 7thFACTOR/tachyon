// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "base/math/quat.h"
#include "base/math/util.h"
#include "base/math/conversion.h"

namespace base
{
const f64 quatDelta = 1e-6;

Quat::Quat()
{
	reset();
}

Quat::~Quat()
{}

Quat::Quat(const Quat& other)
{
	set(other);
}

Quat::Quat(const Euler& euler, Euler::RotationOrder order)
{
	toQuat(euler, order, *this);
}

Quat::Quat(f32 angle, f32 x, f32 y, f32 z)
{
	toQuat(angle, x, y, z, *this);
}

Quat::Quat(f32 angle, const Vec3& axis)
{
	toQuat(angle, axis.x, axis.y, axis.z, *this);
}

Quat::Quat(const AngleAxis& angleAxis)
{
	toQuat(angleAxis, *this);
}

Quat::Quat(const Matrix& matrix)
{
	*this = toQuat(matrix);
}

Quat& Quat::reset()
{
	w = 1.0f;
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;

	return *this;
}

Quat& Quat::set(const Quat& other)
{
	w = other.w;
	x = other.x;
	y = other.y;
	z = other.z;

	return *this;
}

Quat& Quat::setMembers(f32 newX, f32 newY, f32 newZ, f32 newW)
{
	x = newX;
	y = newY;
	z = newZ;
	w = newW;

	return *this;
}

Quat& Quat::postMultiply(const Quat& quat)
{
	Quat tempQ(*this);

	multiplyAndSet(tempQ, quat);

	return *this;
}

Quat& Quat::multiplyAndSet(const Quat& quat1, const Quat& quat2)
{
	w = quat2.w * quat1.w - quat2.x * quat1.x - quat2.y * quat1.y - quat2.z * quat1.z;
	x = quat2.w * quat1.x + quat2.x * quat1.w + quat2.y * quat1.z - quat2.z * quat1.y;
	y = quat2.w * quat1.y - quat2.x * quat1.z + quat2.y * quat1.w + quat2.z * quat1.x;
	z = quat2.w * quat1.z + quat2.x * quat1.y - quat2.y * quat1.x + quat2.z * quat1.w;

	return *this;
}

void Quat::setQuatFromDirectionAndUpAxis(const Vec3& direction, const Vec3& up)
{
	Vec3 xaxis = direction.getNormalized();
	Vec3 zaxis = up.cross(xaxis).getNormalized();
	Vec3 yaxis = zaxis.cross(xaxis);

	yaxis.normalize();

	f32 m[4][4] = { 0 };

	m[0][0] = xaxis.x; m[0][1] = yaxis.x; m[0][2] = zaxis.x;
	m[1][0] = xaxis.y; m[1][1] = yaxis.y; m[1][2] = zaxis.y;
	m[2][0] = xaxis.z; m[2][1] = yaxis.z; m[2][2] = zaxis.z;
	m[3][3] = 1.0f;

	// convert matrix to quat
	f32 qm[4];
	f32 tr = m[0][0] + m[1][1] + m[2][2];

	if (tr > 0.0f)
	{
		f32 s = sqrtf(tr + 1.0f);
		qm[3] = s * 0.5f;
		s = 0.5f / s;
		qm[0] = (m[1][2] - m[2][1]) * s;
		qm[1] = (m[2][0] - m[0][2]) * s;
		qm[2] = (m[0][1] - m[1][0]) * s;
	}
	else
	{
		int i, j, k, nxt[3] = { 1, 2, 0 };

		i = 0;

		if (m[1][1] > m[0][0])
			i = 1;

		if (m[2][2] > m[i][i])
			i = 2;

		j = nxt[i];
		k = nxt[j];

		f32 s = sqrtf((m[i][i] - (m[j][j] + m[k][k])) + 1.0f);

		qm[i] = s * 0.5f;
		s = 0.5f / s;
		qm[3] = (m[j][k] - m[k][j]) * s;
		qm[j] = (m[i][j] + m[j][i]) * s;
		qm[k] = (m[i][k] + m[k][i]) * s;
	}

	x = qm[0];
	y = qm[1];
	z = qm[2];
	w = qm[3];
}

void Quat::square(Quat& value)
{
	f32  s;

	s = 2 * w;
	value.w = (w * w - x * x - y * y - z * z);
	value.x = s * x;
	value.y = s * y;
	value.z = s * z;
}

void Quat::sqr(Quat& value)
{
	f32  len, m;
	f32  A, B;
	Quat r;

	len = sqrtf(w * w + x * x + y * y);

	if (fabs(len) > 0.0001f)
		len = 1.0f / len;
	else
		len = 1.0f;

	r.w = w * len;
	r.x = x * len;
	r.y = z * len;
	r.z = 0.0f;
	m = 1.0f / sqrtf(r.w * r.w + r.x * r.x);
	A = sqrtf((1.0f + r.y) * 0.5f);
	B = sqrtf((1.0f - r.y) * 0.5f);
	value.w = sqrtf(len) * B * r.w * m;
	value.x = sqrtf(len) * B * r.x * m;
	value.y = sqrtf(len) * A;
	value.z = z;
}

f32 Quat::getLength() const
{
	f32 t = (w * w + x * x + y * y + z * z);

	if (t <= 0.0f)
	{
		t = 1.0f;

		if (t < 0.0f)
		{
			return 0.0f;
		}
	}

	t = sqrtf(t);

	return t;
}

f32 Quat::dot(const Quat& value) const
{
	f32 len;

	len = getLength() * value.getLength();

	if (fabs(len) < 0.0001f)
		return 0.0f;

	len = 1.0f / len;

	return (w * value.w + x * value.x + y * value.y + z * value.z) * len;
}

f32 Quat::dotUnit(const Quat& value) const
{
	return (w * value.w + x * value.x + y * value.y + z * value.z);
}

void Quat::scale(f32 value)
{
	w = w * value;
	x = x * value;
	y = y * value;
	z = z * value;
}

void Quat::rescale(f32 value)
{
	f32  len;

	len = getLength();

	if (len < 0.0001f)
	{
		setMembers(0.0f, 0.0f, 0.0f, 1.0f);
	}

	value /= len;
	scale(value);
}

void Quat::normalize()
{
	f32 factor =  w * w + x * x + y * y + z * z;

	if (factor <= 0.0f)
		factor = 1.0f;

	f32 scaleBy(1.0f / sqrtf(factor));

	w = w * scaleBy;
	x = x * scaleBy;
	y = y * scaleBy;
	z = z * scaleBy;
}

void Quat::invert()
{
	f32  mag;

	mag = (w * w + x * x + y * y + z * z);

	if (mag < 0.0001f)
		mag = 1.0f;

	if (mag != 1.0f)
		mag = 1.0f / mag;
	else
		mag = 1.0f;

	w = w *  mag;
	x = x * -mag;
	y = y * -mag;
	z = z * -mag;
}

void Quat::negate()
{
	normalize();

	x = -x;
	y = -y;
	z = -z;
}

void Quat::exp()
{
	f32  len, len1;

	len = sqrtf(x * x + y * y + z * z);

	if (len > 0.0f)
		len1 = sinf(len) / len;
	else
		len1 = 1.0f;

	w = cosf(len);
	x = x * len1;
	y = y * len1;
	z = z * len1;
}

void Quat::log()
{
	f32  len;
	len = sqrtf(x * x + y * y + z * z);

	if (w != 0.0f)
		len = atanf(len / w);
	else
		len = pi / 2.0f;

	w = 0.0f;
	x = x * len;
	y = y * len;
	z = z * len;
}

void Quat::lnDif(const Quat& value)
{
	Quat inv, dif, temp;
	f32  len, len1, s;

	inv.set(*this);
	inv.invert();
	dif = inv * value;
	len = sqrtf(dif.x * dif.x + dif.y * dif.y + dif.z * dif.z);
	s = dot(value);

	if (s != 0.0f)
		len1 = atanf(len / s);
	else
		len1 = pi / 2.0f;

	if (len != 0.0f)
		len1 /= len;

	w = 0.0f;
	x = dif.x * len1;
	y = dif.y * len1;
	z = dif.z * len1;
}

void Quat::slerp(const Quat& quatA, const Quat& quatB, f32 spin, f32 alpha)
{
	f32 k1, k2;
	f32 angle, anglespin;
	f32 sina, cosa;
	i32 flip;

	cosa = quatA.dotUnit(quatB);

	if (cosa < 0.0f)
	{
		cosa = -cosa;
		flip = -1;
	}
	else
	{
		flip = 1;
	}

	if ((1.0f - cosa) < epsilon2)
	{
		k1 = 1.0f - alpha;
		k2 = alpha;
	}
	else
	{
		angle = acosf(cosa);
		sina = sinf(angle);
		anglespin = angle + spin * PI;
		k1 = sinf(angle - alpha * anglespin) / sina;
		k2 = sinf(alpha * anglespin) / sina;
	}

	k2 *= (f32)flip;
	x = k1 * quatA.x + k2 * quatB.x;
	y = k1 * quatA.y + k2 * quatB.y;
	z = k1 * quatA.z + k2 * quatB.z;
	w = k1 * quatA.w + k2 * quatB.w;
}

void Quat::slerp(const Quat& quatA, const Quat& quatB, f32 time)
{
	Quat to1, res;
	double omega, cosom, sinom, scale0, scale1;

	// calc cosine
	cosom = quatA.x * quatB.x + quatA.y * quatB.y + quatA.z * quatB.z + quatA.w * quatB.w;

	// adjust signs(if necessary)
	if (cosom < 0.0f)
	{
		cosom = -cosom;
		to1.x = -quatB.x;
		to1.y = -quatB.y;
		to1.z = -quatB.z;
		to1.w = -quatB.w;
	}
	else
	{
		to1.x = quatB.x;
		to1.y = quatB.y;
		to1.z = quatB.z;
		to1.w = quatB.w;
	}

	// calculate coefficients
	if ((1.0f - cosom) > quatDelta)
	{
		// standard case(slerp)
		omega = acosf(cosom);
		sinom = sinf(omega);
		scale0 = sinf((1.0f - time) * omega) / sinom;
		scale1 = sinf(time * omega) / sinom;
	}
	else
	{
		// "from" and "to" quaternions are very close
		// so we can do a linear interpolation
		scale0 = 1.0f - time;
		scale1 = time;
	}

	// calculate final values
	x = scale0 * quatA.x + scale1 * to1.x;
	y = scale0 * quatA.y + scale1 * to1.y;
	z = scale0 * quatA.z + scale1 * to1.z;
	w = scale0 * quatA.w + scale1 * to1.w;
}

void Quat::multiply(f32 value)
{
	x *= value;
	y *= value;
	z *= value;
	w *= value;
}

Quat Quat::operator + (const Quat& value) const
{
	Quat result;

	result.w = w + value.w;
	result.x = x + value.x;
	result.y = y + value.y;
	result.z = z + value.z;

	return result;
}

Quat Quat::operator - (const Quat& value) const
{
	Quat result;

	result.w = w - value.w;
	result.x = x - value.x;
	result.y = y - value.y;
	result.z = z - value.z;

	return result;
}

Quat Quat::operator * (const Quat& value) const
{
	Quat temp;

	temp.w = w * value.w - x * value.x - y * value.y - z * value.z;
	temp.x = w * value.x + x * value.w + y * value.z - z * value.y;
	temp.y = w * value.y + y * value.w + z * value.x - x * value.z;
	temp.z = w * value.z + z * value.w + x * value.y - y * value.x;

	return temp;
}

Quat Quat::operator / (const Quat& value) const
{
	Quat q, t, s, temp;

	q = value;
	q.x = -q.x;
	q.y = -q.y;
	q.z = -q.z;
	t = (*this) * q;
	s = q * q;
	temp.w = t.w / s.w;
	temp.x = t.x / s.w;
	temp.y = t.y / s.w;
	temp.z = t.z / s.w;

	return temp;
}

Quat& Quat::operator += (const Quat& value)
{
	w += value.w;
	x += value.x;
	y += value.y;
	z += value.z;

	return *this;
}

Quat& Quat::operator -= (const Quat& value)
{
	w -= value.w;
	x -= value.x;
	y -= value.y;
	z -= value.z;

	return *this;
}

Quat& Quat::operator *= (const Quat& value)
{
	w = w * value.w - x * value.x - y * value.y - z * value.z;
	x = w * value.x + x * value.w + y * value.z - z * value.y;
	y = w * value.y + y * value.w + z * value.x - x * value.z;
	z = w * value.z + z * value.w + x * value.y - y * value.x;

	return *this;
}

Quat& Quat::operator /= (const Quat& value)
{
	Quat q, t, s;

	q = value;
	q.x = -q.x;
	q.y = -q.y;
	q.z = -q.z;
	t = (*this) * q;
	s = q * q;
	w = t.w / s.w;
	x = t.x / s.w;
	y = t.y / s.w;
	z = t.z / s.w;

	return *this;
}

Quat& Quat::operator += (f32 value)
{
	w += value;
	x += value;
	y += value;
	z += value;

	return *this;
}

Quat& Quat::operator -= (f32 value)
{
	w -= value;
	x -= value;
	y -= value;
	z -= value;

	return *this;
}

Quat& Quat::operator *= (f32 value)
{
	w *= value;
	x *= value;
	y *= value;
	z *= value;

	return *this;
}

Quat& Quat::operator /= (f32 value)
{
	w /= value;
	x /= value;
	y /= value;
	z /= value;

	return *this;
}

void Quat::blend(f32 factor, const Quat& other)
{
	f32 norm;
	norm = x * other.x + y * other.y + z * other.z + w * other.w;

	bool flip;
	flip = false;

	if (norm < 0.0f)
	{
		norm = -norm;
		flip = true;
	}

	f32 invDist;

	if (1.0f - norm < 0.000001f)
	{
		invDist = 1.0f - factor;
	}
	else
	{
		f32 theta;
		
		theta = (f32) acosf(norm);

		f32 s;
		s = (f32)(1.0f / sin(theta));

		invDist = (f32)sinf((1.0f - factor) * theta) * s;
		factor = (f32)sinf(factor * theta) * s;
	}

	if (flip)
	{
		factor = -factor;
	}

	x = invDist * x + factor * other.x;
	y = invDist * y + factor * other.y;
	z = invDist * z + factor * other.z;
	w = invDist * w + factor * other.w;
}

}