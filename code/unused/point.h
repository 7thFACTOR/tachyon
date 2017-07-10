#pragma once
#include <math.h>
#include <horus/defines.h>
#include <horus/types.h>

namespace horus
{
class HORUS_API Point
{
public:
	Point()
		: x(0)
		, y(0)
	{}

	Point(const Point& other)
	{
		x = other.x;
		y = other.y;
	}

	Point(f32 X, f32 Y)
	{
		x = X;
		y = Y;
	}

	Point(f32 value)
	{
		x = value;
		y = value;
	}

	Point normalized() const;
	void normalize();
	Point& normalizeTo(Point& value);
	void negate();
	Point negated() const;
	Point& negateTo(Point& value);
	f32 dot(const Point& point) const;
	f32 distance(const Point& point) const;
	f32 cos(const Point& point) const;
	void makeAbsolute();
	Point rotate(const Point& origin, f32 angle) const;
	//! returns true if vector is on line [rA,point], using tolerance
	bool isOnLine(const Point& lineA, const Point& lineB, f32 tolerance = 0.0001f) const;

	inline f32 length() const
	{
		return (f32)sqrtf(x * x + y * y);
	}

	inline f32 squaredLength() const
	{
		return x * x + y * y;
	}

	inline Point& set(f32 X, f32 Y)
	{
		x = X;
		y = Y;

		return *this;
	}

	inline Point& zero()
	{
		x = y = 0;

		return *this;
	}

	f32 operator [](int index) const
	{
		if (index == 0)
		{
			return x;
		}
		else if (index == 1)
		{
			return y;
		}

		return 0;
	}

	inline Point& operator += (const Point& value)
	{
		x += value.x;
		y += value.y;

		return *this;
	}

	inline Point& operator *= (const Point& value)
	{
		x *= value.x;
		y *= value.y;

		return *this;
	}

	inline Point& operator -= (const Point& value)
	{
		x -= value.x;
		y -= value.y;

		return *this;
	}

	inline Point& operator /= (const Point& value)
	{
		x /= value.x;
		y /= value.y;

		return *this;
	}

	inline Point& operator += (f32 value)
	{
		x += value;
		y += value;

		return *this;
	}

	inline Point& operator *= (f32 value)
	{
		x *= value;
		y *= value;

		return *this;
	}

	inline Point& operator -= (f32 value)
	{
		x -= value;
		y -= value;

		return *this;
	}

	inline Point& operator /= (f32 value)
	{
		x /= value;
		y /= value;

		return *this;
	}

	inline Point operator + (const Point& value) const
	{
		Point result;

		result .x = x + value.x;
		result .y = y + value.y;

		return result;
	}

	inline Point operator * (const Point& value) const
	{
		Point result;

		result .x = x * value.x;
		result .y = y * value.y;

		return result;
	}

	inline Point operator - (const Point& value) const
	{
		Point result;

		result .x = x - value.x;
		result .y = y - value.y;

		return result;
	}

	inline Point operator / (const Point& value) const
	{
		Point result;

		result .x = x / value.x;
		result .y = y / value.y;

		return result;
	}

	inline Point operator + (const f32 value) const
	{
		Point result;

		result .x = x + value;
		result .y = y + value;

		return result;
	}

	inline Point operator * (const f32 value) const
	{
		Point result;

		result .x = x * value;
		result .y = y * value;

		return result;
	}

	inline Point operator - (const f32 value) const
	{
		Point result;

		result .x = x - value;
		result .y = y - value;

		return result;
	}

	inline Point operator / (const f32 value) const
	{
		Point result;

		result .x = x / value;
		result .y = y / value;

		return result;
	}

	inline Point& operator = (const f32 value)
	{
		x = value;
		y = value;

		return *this;
	}

	inline Point& operator = (const i32 value)
	{
		x = value;
		y = value;

		return *this;
	}

	inline bool operator <= (const Point& other) const
	{
		return x <= other.x
			&& y <= other.y;
	}

	inline bool operator >= (const Point& other) const
	{
		return x >= other.x
			&& y >= other.y;
	}

	inline bool operator < (const Point& other) const
	{
		return x < other.x
			&& y < other.y;
	}

	inline bool operator > (const Point& other) const
	{
		return x > other.x
			&& y > other.y;
	}

	inline bool operator != (const Point& other) const
	{
		return x != other.x
			&& y != other.y;
	}

	f32 x, y;
};

}