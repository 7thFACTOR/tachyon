#pragma once
#include "base/defines.h"
#include "base/types.h"
#include <math.h>

namespace base
{
class String;

class B_API Color
{
public:
	Color();
	Color(const Color& color);
	Color(f32 red, f32 green, f32 blue, f32 alpha = 1.0f);
	Color(f32 rgbaComponents[4]);
	Color(u32 color);
	~Color();

	u32 getRgba() const;
	void set(f32 red, f32 green, f32 blue, f32 alpha = 1.0f);
	void setFromRgba(u32 value);
	Color getScaled(f32 scale) const { return *this * scale; }

	Color operator + (const Color& color) const;
	Color operator - (const Color& color) const;
	Color operator * (const Color& color) const;
	Color operator / (const Color& color) const;
	Color operator + (f32 value) const;
	Color operator - (f32 value) const;
	Color operator * (f32 value) const;
	Color operator / (f32 value) const;
	Color& operator += (const Color& color);
	Color& operator -= (const Color& color);
	Color& operator *= (const Color& color);
	Color& operator /= (const Color& color);
	Color& operator += (f32 value);
	Color& operator -= (f32 value);
	Color& operator *= (f32 value);
	Color& operator /= (f32 value);
	const Color& operator = (const Color& other);
	const Color& operator = (const String& other);
	inline bool operator == (const Color& other) const
	{
		const f32 epsilon = 0.0001f;

		return fabs(r - other.r) < epsilon
			&& fabs(g - other.g) < epsilon
			&& fabs(b - other.b) < epsilon
			&& fabs(a - other.a) < epsilon;
	}

	inline bool operator <= (const Color& other) const
	{
		return r <= other.r && g <= other.g && b <= other.b && a <= other.a;
	}

	inline bool operator >= (const Color& other) const
	{
		return r >= other.r && g >= other.g && b >= other.b && a >= other.a;
	}

	inline bool operator < (const Color& other) const
	{
		return r < other.r && g < other.g && b < other.b && a < other.a;
	}

	inline bool operator > (const Color& other) const
	{
		return r > other.r && g > other.g && b > other.b && a > other.a;
	}

	inline bool operator != (const Color& other) const
	{
		const f32 epsilon = 0.0001f;

		return fabs(r - other.r) > epsilon
			|| fabs(g - other.g) > epsilon
			|| fabs(b - other.b) > epsilon
			|| fabs(a - other.a) > epsilon;
	}

	f32 r, g, b, a;

	static const Color white;
	static const Color gray;
	static const Color darkGray;
	static const Color lightGray;
	static const Color red;
	static const Color green;
	static const Color blue;
	static const Color yellow;
	static const Color black;
	static const Color cyan;
	static const Color skyBlue;
	static const Color magenta;
};

}