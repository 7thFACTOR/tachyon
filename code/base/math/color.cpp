// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "base/math/color.h"
#include "base/string.h"
#include "base/util.h"

namespace base
{
B_API const Color Color::white = Color(1.0f, 1.0f, 1.0f, 1.0f);
B_API const Color Color::gray = Color(0.5f, 0.5f, 0.5f, 1.0f);
B_API const Color Color::darkGray = Color(0.3f, 0.3f, 0.3f, 1.0f);
B_API const Color Color::lightGray = Color(0.7f, 0.7f, 0.7f, 1.0f);
B_API const Color Color::red = Color(1.0f, 0.0f, 0.0f, 1.0f);
B_API const Color Color::green = Color(0.0f, 1.0f, 0.0f, 1.0f);
B_API const Color Color::blue = Color(0.0f, 0.0f, 1.0f, 1.0f);
B_API const Color Color::yellow = Color(1.0f, 1.0f, 0.0f, 1.0f);
B_API const Color Color::black = Color(0.0f, 0.0f, 0.0f, 1.0f);
B_API const Color Color::cyan = Color(0.0f, 1.0f, 1.0f, 1.0f);
B_API const Color Color::skyBlue = Color(0xffcc9900);
B_API const Color Color::magenta = Color(1.0f, 0.0f, 1.0f, 1.0f);
	
Color::Color()
{
	r = g = b = a = 1.0f;
}

Color::Color(const Color& color)
{
	*this = color;
}

Color::Color(f32 red, f32 green, f32 blue, f32 alpha)
{
	r = red;
	g = green;
	b = blue;
	a = alpha;
}

Color::Color(f32 rgbaComponents[4])
{
	r = rgbaComponents[0];
	g = rgbaComponents[1];
	b = rgbaComponents[2];
	a = rgbaComponents[3];
}

Color::Color(u32 color)
{
	setFromRgba(color);
}

Color::~Color()
{}

u32 Color::getRgba() const
{
	u32 col;
	u8 *color = (u8*) &col;

	color[0] = (u8)((r > 1.0f ? 1.0f : r) * 255);
	color[1] = (u8)((g > 1.0f ? 1.0f : g) * 255);
	color[2] = (u8)((b > 1.0f ? 1.0f : b) * 255);
	color[3] = (u8)((a > 1.0f ? 1.0f : a) * 255);

	return col;
}

void Color::set(f32 red, f32 green, f32 blue, f32 alpha)
{
	r = red;
	g = green;
	b = blue;
	a = alpha;
}

void Color::setFromRgba(u32 value)
{
	u8 *color = (u8*) &value;

	r = (f32) color[0] / 255.0f;
	g = (f32) color[1] / 255.0f;
	b = (f32) color[2] / 255.0f;
	a = (f32) color[3] / 255.0f;
}

Color Color::operator + (const Color& color) const
{
	Color result = *this;

	result += color;

	return result;
}

Color Color::operator - (const Color& color) const
{
	Color result = *this;

	result -= color;

	return result;
}

Color Color::operator * (const Color& color) const
{
	Color result = *this;

	result *= color;

	return result;
}

Color Color::operator / (const Color& color) const
{
	Color result = *this;

	result /= color;

	return result;
}

Color Color::operator + (f32 value) const
{
	Color result = *this;

	result += value;

	return result;
}

Color Color::operator - (f32 value) const
{
	Color result = *this;

	result -= value;

	return result;
}

Color Color::operator * (f32 value) const
{
	Color result = *this;

	result *= value;

	return result;
}

Color Color::operator / (f32 value) const
{
	Color result = *this;

	result /= value;

	return result;
}

Color& Color::operator += (const Color& color)
{
	r += color.r;
	g += color.g;
	b += color.b;
	a += color.a;

	return *this;
}

Color& Color::operator -= (const Color& color)
{
	r -= color.r;
	g -= color.g;
	b -= color.b;
	a -= color.a;

	return *this;
}

Color& Color::operator *= (const Color& color)
{
	r *= color.r;
	g *= color.g;
	b *= color.b;
	a *= color.a;

	return *this;
}

Color& Color::operator /= (const Color& color)
{
	r += color.r;
	g += color.g;
	b += color.b;
	a += color.a;

	return *this;
}

Color& Color::operator += (f32 value)
{
	r += value;
	g += value;
	b += value;
	a += value;

	return *this;
}

Color& Color::operator -= (f32 value)
{
	r -= value;
	g -= value;
	b -= value;
	a -= value;

	return *this;
}

Color& Color::operator *= (f32 value)
{
	r *= value;
	g *= value;
	b *= value;
	a *= value;

	return *this;
}

Color& Color::operator /= (f32 value)
{
	r /= value;
	g /= value;
	b /= value;
	a /= value;

	return *this;
}

const Color& Color::operator = (const Color& other)
{
	r = other.r;
	g = other.g;
	b = other.b;
	a = other.a;

	return *this;
}

const Color& Color::operator = (const String& other)
{
	*this = toColor(other);

	return *this;
}

}