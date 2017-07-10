#pragma once

namespace base
{
template<typename Type>
class Vec2Tpl
{
public:
	Vec2Tpl();
	Vec2Tpl(const Vec2Tpl& other);
	Vec2Tpl(Type newX, Type newY);
	Vec2Tpl(Type value);
	Type dot(const Vec2Tpl& other) const;
	Type getDistance(const Vec2Tpl& other) const;
	inline void makeAbsolute();
	Type getCos(const Vec2Tpl& other) const;
	void normalize();
	Vec2Tpl getNormalized() const;
	Vec2Tpl& normalizeTo(Vec2Tpl& to);
	Vec2Tpl getNegated() const;
	void negate();
	Vec2Tpl& negateTo(Vec2Tpl& to);
	Vec2Tpl getRotated(const Vec2Tpl& origin, f32 angle) const;
	bool isOnLine(const Vec2Tpl& lineA, const Vec2Tpl& lineB, Type tolerance = 0.0001f) const;
	inline bool isEquivalent(const Vec2Tpl& other, Type tolerance = 0.0001f) const;
	inline Type getLength() const;
	inline Type getSquaredLength() const;
	void setLength(Type length);
	inline Vec2Tpl& set(Type newX, Type newY);
	inline Vec2Tpl& clear();
	Type operator [](int index) const;
	inline Vec2Tpl& operator += (const Vec2Tpl& value);
	inline Vec2Tpl& operator *= (const Vec2Tpl& value);
	inline Vec2Tpl& operator -= (const Vec2Tpl& value);
	inline Vec2Tpl& operator /= (const Vec2Tpl& value);
	inline Vec2Tpl& operator += (Type value);
	inline Vec2Tpl& operator *= (Type value);
	inline Vec2Tpl& operator -= (Type value);
	inline Vec2Tpl& operator /= (Type value);
	inline Vec2Tpl operator + (const Vec2Tpl& value) const;
	inline Vec2Tpl operator * (const Vec2Tpl& value) const;
	inline Vec2Tpl operator - (const Vec2Tpl& value) const;
	inline Vec2Tpl operator / (const Vec2Tpl& value) const;
	inline Vec2Tpl operator + (const Type value) const;
	inline Vec2Tpl operator * (const Type value) const;
	inline Vec2Tpl operator - (const Type value) const;
	inline Vec2Tpl operator / (const Type value) const;
	inline Vec2Tpl operator / (const size_t value) const;
	inline Vec2Tpl& operator = (const f32 value);
	inline Vec2Tpl& operator = (const i32 value);
	inline bool operator <= (const Vec2Tpl& other) const;
	inline bool operator >= (const Vec2Tpl& other) const;
	inline bool operator < (const Vec2Tpl& other) const;
	inline bool operator > (const Vec2Tpl& other) const;
	inline bool operator != (const Vec2Tpl& other) const;
	inline Vec2Tpl& operator = (const Vec2Tpl& other);

	Type x, y;
};

}