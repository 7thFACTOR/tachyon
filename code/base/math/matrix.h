// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/math/types.h"
#include "base/math/quat.h"
#include "base/math/matrix_decl.h"

namespace base
{
template <typename Type>
MatrixTpl<Type>::MatrixTpl()
{
	setIdentity();
}

template <typename Type>
MatrixTpl<Type>::MatrixTpl(const Type data[16])
{
	toMatrix(data, *this);
}

template <typename Type>
MatrixTpl<Type>::MatrixTpl(const Type data[4][4])
{
	for (u32 i = 0; i < 4; i++)
	{
		memcpy(&m[i], &data[i], sizeof(Type) * 4);
	}
}

template <typename Type>
MatrixTpl<Type>::MatrixTpl(const MatrixTpl& other)
{
	*this = other;
}

template <typename Type>
void MatrixTpl<Type>::setIdentity()
{
	makeZero();
	m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
}

template <typename Type>
void MatrixTpl<Type>::makeZero()
{
	for (u32 i = 0; i < 4; ++i)
	{
		for (u32 j = 0; j < 4; ++j)
		{
			m[i][j] = 0.0f;
		}
	}
}

template <typename Type>
void MatrixTpl<Type>::setUnit()
{
	for (u32 i = 0; i < 4; i++)
	{
		for (u32 j = 0; j < 4; j++)
		{
			m[i][j] = 1.0f;
		}
	}
}

template <typename Type>
void MatrixTpl<Type>::multiply(const MatrixTpl& src, MatrixTpl& dst) const
{
	for (u32 j = 0; j < 4; j++)
	{
		for (u32 i = 0; i < 4; i++)
		{
			dst.m[i][j] = m[i][0] * src.m[0][j] + m[i][1] * src.m[1][j]
				+ m[i][2] * src.m[2][j] + m[i][3] * src.m[3][j];
		}
	}
}

template <typename Type>
void MatrixTpl<Type>::multiply(const MatrixTpl& src)
{
	Type tt[4][4];
	u32 i, j;

	for (j = 0; j < 4; ++j)
	{
		for (i = 0; i < 4; ++i)
		{
			tt[i][j] = m[i][0] * src.m[0][j] + m[i][1] * src.m[1][j]
				+ m[i][2] * src.m[2][j] + m[i][3] * src.m[3][j];
		}
	}

	for (j = 0; j < 4; ++j)
	{
		for (i = 0; i < 4; ++i)
		{
			m[j][i] = tt[j][i];
		}
	}
}

template <typename Type>
void MatrixTpl<Type>::transform(const Vec3Tpl<Type>& src, Vec3Tpl<Type>& dst) const
{
	dst.x = src.x * m[0][0] + src.y * m[1][0] + src.z * m[2][0] + m[3][0];
	dst.y = src.x * m[0][1] + src.y * m[1][1] + src.z * m[2][1] + m[3][1];
	dst.z = src.x * m[0][2] + src.y * m[1][2] + src.z * m[2][2] + m[3][2];
}

template <typename Type>
void MatrixTpl<Type>::transform(const Quat& src, Quat& dst) const
{
	Quat vr;

	vr.x = src.x * m[0][0] + src.y * m[1][0] + src.z * m[2][0] + m[3][0] * src.w;
	vr.y = src.x * m[0][1] + src.y * m[1][1] + src.z * m[2][1] + m[3][1] * src.w;
	vr.z = src.x * m[0][2] + src.y * m[1][2] + src.z * m[2][2] + m[3][2] * src.w;
	vr.w = src.x * m[0][3] + src.y * m[1][3] + src.z * m[2][3] + m[3][3] * src.w;

	dst = vr;
}

template <typename Type>
void MatrixTpl<Type>::setRotationX(f32 angle)
{
	angle = angle * piOver180;

	f32 cu = cosf(angle), su = sinf(angle);

	m[1][1] = cu;
	m[1][2] = su;
	m[2][1] = -su;
	m[2][2] = cu;
}

template <typename Type>
void MatrixTpl<Type>::setRotationY(f32 angle)
{
	angle = angle * piOver180;

	f32 cu = cosf(angle), su = sinf(angle);

	m[0][0] = cu;
	m[0][2] = -su;
	m[2][0] = su;
	m[2][2] = cu;
}

template <typename Type>
void MatrixTpl<Type>::setRotationZ(f32 angle)
{
	angle = angle * piOver180;

	f32 cu = cosf(angle), su = sinf(angle);

	m[0][0] = cu;
	m[0][1] = su;
	m[1][0] = -su;
	m[1][1] = cu;
}

template <typename Type>
void MatrixTpl<Type>::setRotation(AxisType axis, f32 angle)
{
	switch (axis)
	{
	case AxisType::XAxis:
		setRotationX(angle);
		break;
	case AxisType::YAxis:
		setRotationY(angle);
		break;
	case AxisType::ZAxis:
		setRotationZ(angle);
		break;
	}
}

template <typename Type>
void MatrixTpl<Type>::setRotation(f32 eulerX, f32 eulerY, f32 eulerZ)
{
	setRotationX(eulerX);
	setRotationY(eulerY);
	setRotationZ(eulerZ);
}

template <typename Type>
void MatrixTpl<Type>::setTranslation(Type x, Type y, Type z)
{
	m[3][0] = x;
	m[3][1] = y;
	m[3][2] = z;
}

template <typename Type>
void MatrixTpl<Type>::setTranslation(const Vec3Tpl<Type>& trans)
{
	m[3][0] = trans.x;
	m[3][1] = trans.y;
	m[3][2] = trans.z;
}

template <typename Type>
void MatrixTpl<Type>::setScale(Type x, Type y, Type z)
{
	m[0][0] = x;
	m[1][1] = y;
	m[2][2] = z;
	m[3][3] = 1.0f;
}

template <typename Type>
void MatrixTpl<Type>::setScale(const Vec3Tpl<Type>& scale)
{
	m[0][0] = scale.x;
	m[1][1] = scale.y;
	m[2][2] = scale.z;
	m[3][3] = 1.0f;
}

template <typename Type>
void MatrixTpl<Type>::preTranslate(const Vec3Tpl<Type>& pos)
{
	Vec3Tpl<Type> vv;

	transform(pos, vv);
	m[3][0] = vv.x;
	m[3][1] = vv.y;
	m[3][2] = vv.z;
}

template <typename Type>
void MatrixTpl<Type>::fill(Type value)
{
	for (u32 i = 0; i < 4; ++i)
	{
		for (u32 j = 0; j < 4; ++j)
		{
			m[i][j] = value;
		}
	}
}

template <typename Type>
MatrixTpl<Type> MatrixTpl<Type>::concatenate(const MatrixTpl& matrix)
{
	MatrixTpl result;

	makeZero();

	for (u32 i = 0; i < 4; ++i)
	{
		for (u32 j = 0; j < 4; ++j)
		{
			result.m[i][j] += m[i][j] * matrix.m[j][i];
		}
	}

	return result;
}

template <typename Type>
void MatrixTpl<Type>::setScale(Type scale)
{
	m[0][0] = scale;
	m[1][1] = scale;
	m[2][2] = scale;
	m[3][3] = 1.0f;
}

template <typename Type>
void MatrixTpl<Type>::invert()
{
	Type m00 = m[0][0], m01 = m[0][1], m02 = m[0][2], m03 = m[0][3];
	Type m10 = m[1][0], m11 = m[1][1], m12 = m[1][2], m13 = m[1][3];
	Type m20 = m[2][0], m21 = m[2][1], m22 = m[2][2], m23 = m[2][3];
	Type m30 = m[3][0], m31 = m[3][1], m32 = m[3][2], m33 = m[3][3];

	Type v0 = m20 * m31 - m21 * m30;
	Type v1 = m20 * m32 - m22 * m30;
	Type v2 = m20 * m33 - m23 * m30;
	Type v3 = m21 * m32 - m22 * m31;
	Type v4 = m21 * m33 - m23 * m31;
	Type v5 = m22 * m33 - m23 * m32;

	Type t00 = +(v5 * m11 - v4 * m12 + v3 * m13);
	Type t10 = -(v5 * m10 - v2 * m12 + v1 * m13);
	Type t20 = +(v4 * m10 - v2 * m11 + v0 * m13);
	Type t30 = -(v3 * m10 - v1 * m11 + v0 * m12);

	Type invDet = 1.0f / (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);

	Type d00 = t00 * invDet;
	Type d10 = t10 * invDet;
	Type d20 = t20 * invDet;
	Type d30 = t30 * invDet;

	Type d01 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
	Type d11 = +(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
	Type d21 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
	Type d31 = +(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

	v0 = m10 * m31 - m11 * m30;
	v1 = m10 * m32 - m12 * m30;
	v2 = m10 * m33 - m13 * m30;
	v3 = m11 * m32 - m12 * m31;
	v4 = m11 * m33 - m13 * m31;
	v5 = m12 * m33 - m13 * m32;

	Type d02 = +(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
	Type d12 = -(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
	Type d22 = +(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
	Type d32 = -(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

	v0 = m21 * m10 - m20 * m11;
	v1 = m22 * m10 - m20 * m12;
	v2 = m23 * m10 - m20 * m13;
	v3 = m22 * m11 - m21 * m12;
	v4 = m23 * m11 - m21 * m13;
	v5 = m23 * m12 - m22 * m13;

	Type d03 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
	Type d13 = +(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
	Type d23 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
	Type d33 = +(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

	m[0][0] = d00;
	m[0][1] = d01;
	m[0][2] = d02;
	m[0][3] = d03;
	m[1][0] = d10;
	m[1][1] = d11;
	m[1][2] = d12;
	m[1][3] = d13;
	m[2][0] = d20;
	m[2][1] = d21;
	m[2][2] = d22;
	m[2][3] = d23;
	m[3][0] = d30;
	m[3][1] = d31;
	m[3][2] = d32;
	m[3][3] = d33;
}

template <typename Type>
Type MatrixTpl<Type>::getDeterminant() const
{
	return (m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
		m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
		m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]));
}

template <typename Type>
void MatrixTpl<Type>::transpose()
{
	MatrixTpl mtx = *this;

	m[0][1] = mtx.m[1][0];
	m[0][2] = mtx.m[2][0];
	m[0][3] = mtx.m[3][0];

	m[1][0] = mtx.m[0][1];
	m[1][2] = mtx.m[2][1];
	m[1][3] = mtx.m[3][1];

	m[2][0] = mtx.m[0][2];
	m[2][1] = mtx.m[1][2];
	m[2][3] = mtx.m[3][2];

	m[3][0] = mtx.m[0][3];
	m[3][1] = mtx.m[1][3];
	m[3][2] = mtx.m[2][3];
}

template <typename Type>
MatrixTpl<Type> MatrixTpl<Type>::getTransposed() const
{
	MatrixTpl mtx;

	mtx.m[0][1] = m[1][0];
	mtx.m[0][2] = m[2][0];
	mtx.m[0][3] = m[3][0];

	mtx.m[1][0] = m[0][1];
	mtx.m[1][2] = m[2][1];
	mtx.m[1][3] = m[3][1];

	mtx.m[2][0] = m[0][2];
	mtx.m[2][1] = m[1][2];
	mtx.m[2][3] = m[3][2];

	mtx.m[3][0] = m[0][3];
	mtx.m[3][1] = m[1][3];
	mtx.m[3][2] = m[2][3];

	return mtx;
}

template <typename Type>
Vec3Tpl<Type> MatrixTpl<Type>::getTranslation() const
{
	return Vec3Tpl<Type>(m[3][0], m[3][1], m[3][2]);
}

template <typename Type>
Vec3Tpl<Type> MatrixTpl<Type>::operator * (const Vec3Tpl<Type>& vec) const
{
	Vec3Tpl<Type> v;

	transform(vec, v);

	return v;
}

template <typename Type>
MatrixTpl<Type> MatrixTpl<Type>::operator * (const MatrixTpl& matrix) const
{
	MatrixTpl mtx;

	multiply(matrix, mtx);

	return mtx;
}

template <typename Type>
MatrixTpl<Type>& MatrixTpl<Type>::operator *= (const MatrixTpl& matrix)
{
	MatrixTpl mtx;

	multiply(matrix, mtx);
	*this = mtx;

	return *this;
}

template <typename Type>
MatrixTpl<Type>::operator Type* ()
{
	return (Type*)m;
}

template <typename Type>
void MatrixTpl<Type>::lookAt(const Vec3Tpl<Type>& from, const Vec3Tpl<Type>& to, const Vec3Tpl<Type>& up)
{
	lookAtLeftHand(from, to, up);
}

template <typename Type>
void MatrixTpl<Type>::lookTowards(const Vec3Tpl<Type>& to, const Vec3Tpl<Type>& up)
{
	Vec3Tpl<Type> dir = to.getNegated(), upDir = up;
	Vec3Tpl<Type> side;

	dir.normalize();
	upDir.negate();
	side = dir.cross(upDir);
	side.normalize();
	upDir = dir.cross(side);
	upDir.negate();

	m[0][0] = side[0];
	m[0][1] = upDir[0];
	m[0][2] = dir[0];
	m[1][0] = side[1];
	m[1][1] = upDir[1];
	m[1][2] = dir[1];
	m[2][0] = side[2];
	m[2][1] = upDir[2];
	m[2][2] = dir[2];
	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = 0.0f;
	m[0][3] = 0.0f;
	m[1][3] = 0.0f;
	m[2][3] = 0.0f;
	m[3][3] = 1.0f;
}

template <typename Type>
void MatrixTpl<Type>::perspective(f32 fov, f32 aspect, Type near, Type far)
{
	perspectiveLeftHand(fov, aspect, near, far);
}

template <typename Type>
void MatrixTpl<Type>::ortho(Type left, Type right, Type bottom, Type top, Type near, Type far)
{
	orthoLeftHand(left, right, bottom, top, near, far);
}

template <typename Type>
void MatrixTpl<Type>::lookAtLeftHand(const Vec3Tpl<Type>& from, const Vec3Tpl<Type>& to, const Vec3Tpl<Type>& up)
{
	// Left hand Lookat
	//----------------------------------------------------------------
	// zaxis = normal(At - Eye)
	// xaxis = normal(cross(Up, zaxis))
	// yaxis = cross(zaxis, xaxis)
	// xaxis.x           yaxis.x           zaxis.x          0
	// xaxis.y           yaxis.y           zaxis.y          0
	// xaxis.z           yaxis.z           zaxis.z          0
	// -dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye)  l

	Vec3Tpl<Type> x, y, z;

	z = (to - from);
	z.normalize();
	x = up.cross(z);
	x.normalize();
	y = z.cross(x);
	y.normalize();

	m[0][0] = x.x;
	m[0][1] = x.y;
	m[0][2] = x.z;
	m[0][3] = 0.0f;

	m[1][0] = y.x;
	m[1][1] = y.y;
	m[1][2] = y.z;
	m[1][3] = 0.0f;

	m[2][0] = z.x;
	m[2][1] = z.y;
	m[2][2] = z.z;
	m[2][3] = 0.0f;

	m[3][0] = from.x;
	m[3][1] = from.y;
	m[3][2] = from.z;
	m[3][3] = 1.0f;
}

template <typename Type>
void MatrixTpl<Type>::perspectiveLeftHand(f32 fov, f32 aspect, Type near, Type far)
{
	// Left-Hand perspective projection, Z pointing towards screen
	//-------------------------------------------------------------------------
	//    xScale       0            0              0
	//        0        yScale       0              0
	//        0        0        zf/(zf-zn)         1
	//        0        0        -zn*zf/(zf-zn)     0
	//    where:
	//    yScale = cot(fovY/2)
	//
	//    xScale = aspect ratio * yScale

	fov = B_DEGREES_TO_RADIANS(fov);

	Type yScale = (Type) 1.0f / tanf(fov / 2.0f);
	Type xScale = aspect * yScale;

	m[0][0] = xScale;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	m[0][3] = 0.0f;
	m[1][0] = 0.0f;
	m[1][1] = yScale;
	m[1][2] = 0.0f;
	m[1][3] = 0.0f;
	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = far / (far - near);
	m[2][3] = 1.0f;
	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = -near * far / (far - near);
	m[3][3] = 0.0f;
}

template <typename Type>
void MatrixTpl<Type>::orthoLeftHand(Type left, Type right, Type bottom, Type top, Type near, Type far)
{
	// Left-Hand ortho projection, Z pointing towards screen
	//------------------------------------------------------------------------------
	// 2/(r-l)      0            0           0
	// 0            2/(t-b)      0           0
	// 0            0            1/(zf-zn)   0
	// (l+r)/(l-r)  (t+b)/(b-t)  zn/(zn-zf)  1

	m[0][0] = 2.0f / (right - left);
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	m[0][3] = 0.0f;

	m[1][0] = 0.0f;
	m[1][1] = 2.0f / (top - bottom);
	m[1][2] = 0.0f;
	m[1][3] = 0.0f;

	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = 1.0f / (far - near);
	m[2][3] = 0.0f;

	m[3][0] = (left + right) / (left - right);
	m[3][1] = (top + bottom) / (bottom - top);
	m[3][2] = near / (near - far);
	m[3][3] = 1.0f;
}

template <typename Type>
void MatrixTpl<Type>::lookAtRightHand(const Vec3Tpl<Type>& from, const Vec3Tpl<Type>& to, const Vec3Tpl<Type>& up)
{
	// Right hand Lookat
	//----------------------------------------------------------------
	// zaxis = normal(Eye - At)
	// xaxis = normal(cross(Up, zaxis))
	// yaxis = cross(zaxis, xaxis)
	// xaxis.x           yaxis.x           zaxis.x          0
	// xaxis.y           yaxis.y           zaxis.y          0
	// xaxis.z           yaxis.z           zaxis.z          0
	//-dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye)  1

	Vec3Tpl<Type> x, y, z;
	z = from - to;
	z.normalize();
	x = up.cross(z);
	x.normalize();
	y = z.cross(x);
	y.normalize();

	m[0][0] = x.x;
	m[0][1] = y.x;
	m[0][2] = z.x;
	m[0][3] = 0.0f;
	m[1][0] = x.y;
	m[1][1] = y.y;
	m[1][2] = z.y;
	m[1][3] = 0.0f;
	m[2][0] = x.z;
	m[2][1] = y.z;
	m[2][2] = z.z;
	m[2][3] = 0.0f;
	m[3][0] = -x.dot(from);
	m[3][1] = -y.dot(from);
	m[3][2] = -z.dot(from);
	m[3][3] = 1.0f;
}

template <typename Type>
void MatrixTpl<Type>::perspectiveRightHand(f32 fov, f32 aspect, Type near, Type far)
{
	// Right-Hand perspective projection, Z pointing towards you
	//-------------------------------------------------------------------------
	//    xScale       0            0              0
	//        0        yScale       0              0
	//        0        0        zf/(zn-zf)        -1
	//        0        0        zn*zf/(zn-zf)      0
	//    where:
	//    yScale = cot(fovY/2)
	//
	//    xScale = aspect ratio * yScale

	fov = B_DEGREES_TO_RADIANS(fov);

	Type yScale = (Type) 1.0f / tanf(fov / 2.0f);
	Type xScale = aspect * yScale;

	m[0][0] = xScale;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	m[0][3] = 0.0f;
	m[1][0] = 0.0f;
	m[1][1] = yScale;
	m[1][2] = 0.0f;
	m[1][3] = 0.0f;
	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = far / (near - far);
	m[2][3] = -1.0f;
	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = near * far / (near - far);
	m[3][3] = 0.0f;
}

template <typename Type>
void MatrixTpl<Type>::orthoRightHand(Type left, Type right, Type bottom, Type top, Type near, Type far)
{
	// Right-Hand ortho projection, Z pointing towards you
	//-------------------------------------------------------------------------
	// 2/(r-l)      0            0           0
	// 0            2/(t-b)      0           0
	// 0            0            1/(zn-zf)   0
	// (l+r)/(l-r)  (t+b)/(b-t)  zn/(zn-zf)  1

	m[0][0] = (Type) 2.0f / (right - left);
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	m[0][3] = 0.0f;

	m[1][0] = 0.0f;
	m[1][1] = (Type) 2.0f / (top - bottom);
	m[1][2] = 0.0f;
	m[1][3] = 0.0f;

	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = (Type) 1.0f / (near - far);
	m[2][3] = 0.0f;

	m[3][0] = (Type)(left + right) / (left - right);
	m[3][1] = (Type)(top + bottom) / (bottom - top);
	m[3][2] = (Type)near / (near - far);
	m[3][3] = 1.0f;
}

template <typename Type>
Vec3Tpl<Type> MatrixTpl<Type>::getScale() const
{
	Vec3Tpl<Type> s;

	s.x = sqrtf(m[0][0] * m[0][0] + m[0][1] * m[0][1] + m[0][2] * m[0][2]);
	s.y = sqrtf(m[1][0] * m[1][0] + m[1][1] * m[1][1] + m[1][2] * m[1][2]);
	s.z = sqrtf(m[2][0] * m[2][0] + m[2][1] * m[2][1] + m[2][2] * m[2][2]);

	return s;
}

template <typename Type>
void MatrixTpl<Type>::clearTranslation()
{
	m[3][0] = 0;
	m[3][1] = 0;
	m[3][2] = 0;
}

template <typename Type>
void MatrixTpl<Type>::clearRotation()
{
	Vec3Tpl<Type> trans = getTranslation(), scl = getScale();

	setIdentity();
	setTranslation(trans.x, trans.y, trans.z);
	setScale(scl.x, scl.y, scl.z);
}

template <typename Type>
void MatrixTpl<Type>::clearScale()
{
	//TODO: not like this
	m[0][0] = 1.0f;
	m[1][1] = 1.0f;
	m[2][2] = 1.0f;
	m[3][3] = 1.0f;
}

template <typename Type>
MatrixTpl<Type> MatrixTpl<Type>::getRotationOnly() const
{
	return toMatrix(toQuat(*this));
}

template <typename Type>
MatrixTpl<Type> MatrixTpl<Type>::getInverted() const
{
	MatrixTpl thisOne = *this;

	thisOne.invert();

	return thisOne;
}

template <typename Type>
inline Vec3Tpl<Type> MatrixTpl<Type>::getRightAxis() const { return Vec3Tpl<Type>(m[0][0], m[1][0], m[2][0]).getNormalized(); }

template <typename Type>
inline Vec3Tpl<Type> MatrixTpl<Type>::getUpAxis() const { return Vec3Tpl<Type>(m[0][1], m[1][1], m[2][1]).getNormalized(); }

template <typename Type>
inline Vec3Tpl<Type> MatrixTpl<Type>::getForwardAxis() const { return Vec3Tpl<Type>(m[0][2], m[1][2], m[2][2]).getNormalized(); }

template <typename Type>
MatrixTpl<Type> MatrixTpl<Type>::makeTranslationMatrix(const Vec3Tpl<Type>& trans)
{
	MatrixTpl m;
	
	m.setTranslation(trans);
	
	return m;
}

typedef MatrixTpl<f32> Matrix;
typedef MatrixTpl<f32> Matrixf;
typedef MatrixTpl<f64> Matrixd;

}