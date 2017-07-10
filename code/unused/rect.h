#pragma once

#include <horus/defines.h>
#include <horus/point.h>

namespace horus
{
template<typename _Type, typename _PointType>
class RectTpl
{
public:
	RectTpl() {}
	
	RectTpl(_Type X, _Type Y, _Type W, _Type H)
		: x(X)
		, y(Y)
		, width(W)
		, height(H)
	{}

	void set(_Type X, _Type Y, _Type W, _Type H)
	{
		x = X;
		y = Y;
		width = W;
		height = H;
	}
	
	RectTpl(const RectTpl& rc) { *this = rc; }
	inline _Type left() const { return x; }
	inline _Type top() const { return y; }
	inline _Type right() const { return x + width; }
	inline _Type bottom() const { return y + height; }
	inline _PointType topLeft() const { return _PointType(x, y); }
	inline _PointType topRight() const { return _PointType(x + width, y); }
	inline _PointType bottomLeft() const { return _PointType(x, y + height); }
	inline _PointType bottomRight() const { return _PointType(x + width, y); }
	inline _PointType center() const { return _PointType(x + width / 2.0f, y + height / 2.0f); }
	bool contains(const _PointType& pt) const { return pt.x >= x && pt.x <= x + width && pt.y >= y && pt.y <= y + height; }
	bool contains(_Type X, _Type Y) const { return X >= x && X <= x + width && Y >= y && Y <= y + height; }
	bool contains(const RectTpl& other) const
	{
		return x <= other.x && (other.x + other.width) <= x + width
			&& y <= other.y && (other.y + other.height) <= y + height;
	}
	bool overlaps(const RectTpl& other) const { return	x <= other.right() && right() >= other.x && y <= other.bottom() && bottom() >= other.y; }
	void inflate(_Type amountX, _Type amountY) { x -= amountX; y -= amountY; width += amountX * 2; height += amountY * 2; }
	inline void setPosition(_Type X, _Type Y) { x = X; y = Y; }
	inline void setPosition(const _PointType& pos) { x = pos.x; y = pos.y; }
	inline void setSize(_Type W, _Type H) { width = W; height = H; }
	inline void setSize(const _PointType& size) { width = size.x; height = size.y; }

	RectTpl& operator += (const Point& pt) { x += pt.x; y += pt.y; return *this; }
	RectTpl operator + (const Point& pt) { RectTpl rc = *this; rc.x += pt.x; rc.y += pt.y; return rc; }

	_Type x, y, width, height;
};

typedef RectTpl<f32, Point> Rect;
typedef RectTpl<i32, Point> RectI32;

}