// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include <stdio.h>
#include "base/defines.h"
#include "base/math/vec2.h"
#include "base/math/vec3.h"
#include "base/math/quat.h"
#include "base/math/angle_axis.h"
#include "base/math/matrix.h"
#include "base/math/bbox.h"
#include "base/math/color.h"
#include "base/array.h"
#include "base/string.h"

namespace base
{
typedef char Utf8Char;
typedef const char* Utf8String;
typedef char* Utf8StringBuffer;
typedef wchar_t Utf16Char;
typedef const wchar_t* Utf16String;
typedef wchar_t* Utf16StringBuffer;
typedef u32 Utf32Char;
typedef const u32* Utf32String;
typedef u32* Utf32StringBuffer;

enum class MemoryUnit
{
	Byte,
	KiloByte,
	MegaByte,
	GigaByte,
	TeraByte,
	PetaByte, // :D

	Count
};

B_EXPORT void toLowerCase(String& str);
B_EXPORT void toUpperCase(String& str);
B_EXPORT size_t explodeString(const String& str, Array<String>& tokens, const String& separator);
B_EXPORT String implodeStrings(const Array<String>& tokens, const String& separator);
B_EXPORT String repeatString(const String& str, size_t count);
B_EXPORT String trimLeft(const String& str);
B_EXPORT String trimRight(const String& str);
B_EXPORT String trim(const String& str);
B_EXPORT bool wildcardCompare(const String& wildcard, const String& text, bool caseSensitive = true);
B_EXPORT f32 convertBytesTo(MemoryUnit unit, size_t byteCount);
B_EXPORT String convertBytesToText(size_t byteCount);

B_EXPORT String charToString(char value);
B_EXPORT String toHexString(int n, bool lowercase = true);
B_EXPORT String toString(bool value);
B_EXPORT String toString(i8 value, int decimals = 0);
B_EXPORT String toString(i16 value, int decimals = 0);
B_EXPORT String toString(i32 value, int decimals = 0);
B_EXPORT String toString(i64 value, int decimals = 0);

B_EXPORT String toString(u8 value, int decimals = 0);
B_EXPORT String toString(u16 value, int decimals = 0);
B_EXPORT String toString(u32 value, int decimals = 0);
B_EXPORT String toString(u64 value, int decimals = 0);

B_EXPORT String toString(f32 value, int decimals = 0);
B_EXPORT String toString(f64 value, int decimals = 0);

B_EXPORT String toUuidString(u128 value);

B_EXPORT String toString(const Vec2& value);
B_EXPORT String toString(const Vec3& value);
B_EXPORT String toString(const Quat& value);
B_EXPORT String toString(const AngleAxis& value);
B_EXPORT String toString(const Matrix& value);
B_EXPORT String toString(const Color& value);
B_EXPORT String toString(const BBox& value);

//! convert a string to Vec3 3D, must be formatted like this: "x;y;z" -> "100;23;120.3"
B_EXPORT Vec3 toVec3(const String& str);
//! convert a string to Vec2 2D, must be formatted like this: "x;y" -> "233;120.3"
B_EXPORT Vec2 toVec2(const String& str);
inline Vec3 toVec3(const Vec2& val) { return Vec3(val.x, val.y, 0.0f); }
inline Vec2 toVec2(const Vec3& val) { return Vec2(val.x, val.y); }
//! convert a string to Quaternion, must be formatted like this: "angle;x;y;z" -> "90;1;0;1"
B_EXPORT Quat toQuat(const String& str);
//! convert a string to Color, must be formatted like this: "r;g;b;a" -> "0.4;.5;1;1"
B_EXPORT Color toColor(const String& str);
//! convert a string to Matrix, must be formatted as 16 f32 values separated by ";"
B_EXPORT Matrix toMatrix(const String& str);
//! convert a string to Box, must be formatted as min/max corners, 6 f32 values separated by ";"
B_EXPORT BBox toBBox(const String& str);
//TODO: maybe add position also, if image is too small, rename to Rect not image
B_EXPORT bool fitImageSizeToView(
	f32 imageWidth, f32 imageHeight,
	f32 viewWidth, f32 viewHeight,
	f32& newImageWidth, f32& newImageHeight,
	bool ignoreHeight = false,
	bool ignoreWidth = false);

inline u32 closestPowerOf2(u32 value)
{
	u32 p2;
	
	for (p2 = 1; p2 < value; p2 <<= 1);
	
	return p2;
}

B_EXPORT bool loadTextFile(const String& filename, String& outString);
B_EXPORT FILE* utf8fopen(const String& filename, const String& flags);

B_EXPORT u128 generateUuid();
B_EXPORT u64 hashString(const String& str);

//! convert from UTF8 to UTF16
B_EXPORT bool utf8ToUtf16NoAlloc(Utf8String text, wchar_t* outText, size_t maxOutTextByteSize);
B_EXPORT bool utf8ToUtf32NoAlloc(Utf8String text, Utf32StringBuffer outText, size_t maxOutTextByteSize);
B_EXPORT bool stringToUtf16(const String& text, Array<Utf16Char> outChars);

B_EXPORT bool utf16ToUtf8NoAlloc(const wchar_t* text, Utf8StringBuffer outText, size_t maxOutTextByteSize);
B_EXPORT bool utf16ToUtf32NoAlloc(const wchar_t* text, Utf32StringBuffer outText, size_t maxOutTextByteSize);
B_EXPORT String stringFromUtf16(Utf16StringBuffer text);

B_EXPORT bool utf32ToUtf8NoAlloc(Utf32String text, Utf8String* outText, size_t maxOutTextByteSize);
B_EXPORT bool utf32ToUtf16NoAlloc(Utf32String text, wchar_t* outText, size_t maxOutTextByteSize);

class B_API JenkinsLookup3Hash64
{
private:
	u32 hash0;
	u32 hash1;

public:
	JenkinsLookup3Hash64()
		: hash0(0)
		, hash1(0)
	{}

	void reset()
	{
		hash0 = 0;
		hash1 = 0;
	}

	u32 getHash32() const
	{
		return hash0;
	}

	u64 getHash64() const
	{
		return (((u64)hash1) << 32) | hash0;
	}

	bool operator == (const JenkinsLookup3Hash64& a) const
	{
		return (hash0 == a.hash0) && (hash1 == a.hash1);
	}

	bool operator != (const JenkinsLookup3Hash64& a) const
	{
		return (hash0 != a.hash0) || (hash1 != a.hash1);
	}

	void add(const void* data, size_t sizeInBytes);
	void addAsciiLowerCase(const void* data, size_t sizeInBytes);
};

// Enums stringify utilities

#define B_BEGIN_ENUM_STRINGIFY(enumDictionaryName)\
	static Dictionary<String, u32> enumDictionaryName##StringToEnum;\
	static Dictionary<u32, String> enumDictionaryName##EnumToString;\
	Dictionary<String, u32>& get##enumDictionaryName##StringToEnum() { return enumDictionaryName##StringToEnum; }\
	Dictionary<u32, String>& get##enumDictionaryName##EnumToString() { return enumDictionaryName##EnumToString; }\
	static struct AutoInitEnumValueMap_##enumDictionaryName\
	{\
		AutoInitEnumValueMap_##enumDictionaryName()\
		{\
			Dictionary<String, u32>& stringToEnum = enumDictionaryName##StringToEnum;\
			Dictionary<u32, String>& enumToString = enumDictionaryName##EnumToString;

#define B_DECLARE_ENUM_STRINGIFY(enumDictionaryName)\
	Dictionary<String, u32>& get##enumDictionaryName##StringToEnum();\
	Dictionary<u32, String>& get##enumDictionaryName##EnumToString();

#define B_ADD_ENUM_STRINGIFY(enumName, enumValue)\
	stringToEnum[enumName] = (u32)enumValue;\
	enumToString[(u32)enumValue] = enumName;

#define B_ADD_ENUM_STRINGIFY2(enumValue)\
	stringToEnum[#enumValue] = (u32)enumValue;\
	enumToString[(u32)enumValue] = #enumValue;

#define B_END_ENUM_STRINGIFY(enumDictionaryName) } } autoInitEnumValueDictionary_##enumDictionaryName;
#define B_ENUM_TO_STRING(enumDictionaryName, enumValue) get##enumDictionaryName##EnumToString()[enumValue]
#define B_STRING_TO_ENUM(enumDictionaryName, enumName) get##enumDictionaryName##StringToEnum()[String(enumName)]

}