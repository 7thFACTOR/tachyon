#pragma once
#include "base/defines.h"
#include "base/math/vec2.h"

namespace base
{
template<typename Type, typename PointType>
class RectTpl
{
public:
	RectTpl()
		: x(0)
		, y(0)
		, width(0)
		, height(0)
	{}
	
	RectTpl(Type newX, Type newY, Type newWidth, Type newHeight)
	{
		x = newX;
		y = newY;
		width = newWidth;
		height = newHeight;
	}

	void set(Type newX, Type newY, Type newWidth, Type newHeight)
	{
		x = newX;
		y = newY;
		width = newWidth;
		height = newHeight;
	}
	
	RectTpl(const RectTpl& rc) { *this = rc; }
	inline Type left() const { return x; }
	inline Type top() const { return y; }
	inline Type right() const { return x + width; }
	inline Type bottom() const { return y + height; }
	inline PointType topLeft() const { return PointType(x, y); }
	inline PointType topRight() const { return PointType(x + width, y); }
	inline PointType bottomLeft() const { return PointType(x, y + height); }
	inline PointType bottomRight() const { return PointType(x + width, y + height); }
	inline PointType center() const { return PointType(x + width / 2.0f, y + height / 2.0f); }
	bool contains(const PointType& pt) { return pt.x >= x && pt.x <= x + width && pt.y >= y && pt.y <= y + height; }
	bool contains(Type X, Type Y) { return X >= x && X <= x + width && Y >= y && Y <= y + height; }
	bool contains(const RectTpl& other)
	{
		return x <= other.x && (other.x + other.width) <= x + width
			&& y <= other.y && (other.y + other.height) <= y + height;
	}
	bool overlaps(const RectTpl& other) const 
	{
		return x <= other.right() && right() >= other.x
			&& y <= other.bottom() && bottom() >= other.y;
	}
	inline void adjust(Type amountX, Type amountY) { x -= amountX; y -= amountY; width += amountX * 2; height += amountY * 2; }
	void setPosition(Type X, Type Y) { x = X; y = Y; }
	void setPosition(const PointType& pos) { x = pos.x; y = pos.y; }
	void setSize(Type _width, Type _height) { width = _width; height = _height; }
	void setSize(const PointType& size) { width = size.x; height = size.y; }

	RectTpl& operator += (const PointType& pt) { x += pt.x; y += pt.y; return *this; }
	RectTpl operator + (const PointType& pt) { RectTpl rc = *this; rc.x += pt.x; rc.y += pt.y; return rc; }

	inline RectTpl& operator = (const RectTpl& other)
	{
		x = other.x;
		y = other.y;
		width = other.width;
		height = other.height;

		return *this;
	}

	Type x, y, width, height;
};

typedef RectTpl<f32, Vec2f> RectF;
typedef RectTpl<f64, Vec2d> RectD;
typedef RectTpl<i32, Vec2i> RectI;
typedef RectF Rect;

}