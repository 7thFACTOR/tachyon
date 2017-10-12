// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once

namespace base
{
template<typename Type>
class Vec3Tpl
{
public:
	static const Vec3Tpl up() { return Vec3Tpl(0, 1, 0); }
	static const Vec3Tpl forward() { return Vec3Tpl(0, 0, 1); }

	Vec3Tpl();
	Vec3Tpl(const Vec3Tpl& other);
	Vec3Tpl(Type newX, Type newY, Type newZ);
	Vec3Tpl(Type value);
	Vec3Tpl cross(const Vec3Tpl& other) const;
	Type dot(const Vec3Tpl& other) const;
	Type getDistance(const Vec3Tpl& to) const;
	inline void makeAbsolute();
	Type getCosAngle(const Vec3Tpl& other) const;
	void normalize();
	Vec3Tpl getNormalized() const;
	Vec3Tpl& normalizeTo(Vec3Tpl& to);
	Vec3Tpl getNegated() const;
	void negate();
	Vec3Tpl& negateTo(Vec3Tpl& to);
	bool isOnLine(const Vec3Tpl& lineA, const Vec3Tpl& lineB, Type tolerance = 0.0001f) const;
	bool isInTriangle(const Vec3Tpl& vertexPos1, const Vec3Tpl& vertexPos2, const Vec3Tpl& vertexPos3) const;
	inline bool isEquivalent(const Vec3Tpl& other, Type tolerance = 0.001f) const;
	inline Type getLength() const;
	inline Type getSquaredLength() const;
	void setLength(Type length);
	inline Vec3Tpl& set(Type newX, Type newY, Type newZ);
	inline Vec3Tpl& makeZero();
	Type operator [](int index) const;
	inline Vec3Tpl& operator += (const Vec3Tpl& value);
	inline Vec3Tpl& operator *= (const Vec3Tpl& value);
	inline Vec3Tpl& operator -= (const Vec3Tpl& value);
	inline Vec3Tpl& operator /= (const Vec3Tpl& value);
	inline Vec3Tpl& operator += (Type value);
	inline Vec3Tpl& operator *= (Type value);
	inline Vec3Tpl& operator -= (Type value);
	inline Vec3Tpl& operator /= (Type value);
	inline Vec3Tpl operator + (const Vec3Tpl& value) const;
	inline Vec3Tpl operator * (const Vec3Tpl& value) const;
	inline Vec3Tpl operator - (const Vec3Tpl& value) const;
	inline Vec3Tpl operator / (const Vec3Tpl& value) const;
	inline Vec3Tpl operator + (const Type value) const;
	inline Vec3Tpl operator * (const Type value) const;
	inline Vec3Tpl operator - (const Type value) const;
	inline Vec3Tpl operator / (const Type value) const;
	inline Vec3Tpl operator / (const i32 value) const;
	inline Vec3Tpl operator / (const size_t value) const;
	inline Vec3Tpl& operator = (const Type value);
	inline Vec3Tpl& operator = (const i32 value);
	inline bool operator <= (const Vec3Tpl& other) const;
	inline bool operator >= (const Vec3Tpl& other) const;
	inline bool operator < (const Vec3Tpl& other) const;
	inline bool operator > (const Vec3Tpl& other) const;
	inline bool operator != (const Vec3Tpl& other) const;

	Type x, y, z;

protected:
	bool isOnSameSide(const Vec3Tpl& p1, const Vec3Tpl& p2, const Vec3Tpl& a, const Vec3Tpl& b);
};

}