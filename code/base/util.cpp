// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <ctype.h>
#include <wchar.h>
#include <assert.h>
#include <sstream>
#include <codecvt>
#include <locale>
#include "base/util.h"
#include "base/string.h"
#include "base/math/conversion.h"
#include "base/math/util.h"
#include "base/logger.h"
#include "base/platform.h"
#include "3rdparty/utf8/source/utf8.h"

#ifdef _WINDOWS
#include <windows.h>
#endif

namespace base
{
void toLowerCase(String& str)
{
	String lower;

	Utf8Byte* strPtr = (Utf8Byte*)str.c_str();

	if (strPtr)
	{
		Utf32Codepoint c = utf8::peek_next(strPtr, str.end());
		
		while (c)
		{
			lower += (Utf32Codepoint)tolower(c);
			utf8::next(strPtr, str.end());
			
			if (strPtr == str.end())
				break;

			c = utf8::peek_next(strPtr, str.end());
		}
	}

	str = lower;
}

void toUpperCase(String& str)
{
	String upper;
	Utf8Byte* strPtr = (Utf8Byte*)str.c_str();

	if (strPtr)
	{
		Utf32Codepoint c = utf8::peek_next(strPtr, str.end());

		while (c)
		{
			upper += (Utf32Codepoint)toupper(c);
			utf8::next(strPtr, str.end());

			if (strPtr == str.end())
				break;

			c = utf8::peek_next(strPtr, str.end());
		}
	}

	str = upper;
}

size_t explodeString(const String& str, Array<String>& tokens, const String& separator)
{
	size_t pos = str.find(separator, 0);
	size_t pit = separator.length();
	String theStr = str;

	while (pos != String::noIndex)
	{
		if (pos != 0)
		{
			tokens.append(theStr.subString(0, pos));
		}

		theStr = theStr.subString(pos + pit, theStr.length() - pos - pit);
		pos = theStr.find(separator, 0);
	}

	if (theStr != "")
		tokens.append(theStr);

	return tokens.size();
}

String implodeStrings(const Array<String>& tokens, const String& separator)
{
	String str;

	for (u32 i = 0, iCount = tokens.size(); i < iCount; ++i)
	{
		str += tokens[i];
		str += (i == tokens.size() - 1 ? String() : separator);
	}

	return str;
}

String repeatString(const String& str, size_t count)
{
	String theStr;

	for (size_t i = 0; i < count; ++i)
	{
		theStr += str;
	}

	return theStr;
}

static const String whitespaceCharacters = "\n\t\r ";

String trimLeft(const String& str)
{
	size_t charSetLen = whitespaceCharacters.length();
	size_t thisIndex = 0;
	bool stopped = false;

	while (!stopped && (thisIndex < str.length()))
	{
		size_t charSetIndex;
		bool match = false;

		for (charSetIndex = 0; charSetIndex < charSetLen; ++charSetIndex)
		{
			if (str[thisIndex] == whitespaceCharacters[charSetIndex])
			{
				match = true;
				break;
			}
		}
		if (!match)
		{
			// stop if no match
			stopped = true;
		}
		else
		{
			// a match, advance to next character
			++thisIndex;
		}
	}
	

	String trimmedString(str.c_str() + str.computeByteCount(0, thisIndex));

	return trimmedString;
}

String trimRight(const String& str)
{
	size_t charSetLen = whitespaceCharacters.length();
	i32 thisIndex = str.length() - 1;
	bool stopped = false;
	
	while (!stopped && (thisIndex >= 0))
	{
		size_t charSetIndex;
		bool match = false;

		for (charSetIndex = 0; charSetIndex < charSetLen; charSetIndex++)
		{
			if (str[thisIndex] == whitespaceCharacters[charSetIndex])
			{
				match = true;
				break;
			}
		}
		if (!match)
		{
			// stop if no match
			stopped = true;
		}
		else
		{
			// a match, advance to next character
			--thisIndex;
		}
	}

	return str.subString(0, thisIndex);
}

String trim(const String& str)
{
	String tmpStr = str;

	tmpStr = trimLeft(tmpStr);
	tmpStr = trimRight(tmpStr);

	return tmpStr;
}

String toHexString(int n, bool lowercase)
{
	char buf[64] = {0};
	
	if (lowercase)
		sprintf(buf, "%02x", n);
	else
		sprintf(buf, "%02X", n);
	
	return buf;
}

bool wildcardCompare(const String& wildcard, const String& text, bool caseSensitive)
{
	Utf8Byte* cp = nullptr;
	Utf8Byte* mp = nullptr;
	Utf8Byte* textPtr = (Utf8Byte*)text.c_str();
	Utf8Byte* wildcardPtr = (Utf8Byte*)wildcard.c_str();

	#define B_WILDCARD_COMPARE_CHAR(chr) (caseSensitive ? chr : tolower(chr))

	auto chrText = utf8::peek_next(textPtr, text.end());
	auto chrWildcard = utf8::peek_next(wildcardPtr, wildcard.end());

	while (chrText && (chrWildcard != '*'))
	{
		if (B_WILDCARD_COMPARE_CHAR(chrWildcard) != B_WILDCARD_COMPARE_CHAR(chrText)
			&& (chrWildcard != '?'))
		{
			return false;
		}

		utf8::next(textPtr, text.end());
		utf8::next(wildcardPtr, wildcard.end());

		if (textPtr == text.end())
			break;

		if (wildcardPtr == wildcard.end())
			break;

		chrText = utf8::peek_next(textPtr, text.end());
		chrWildcard = utf8::peek_next(wildcardPtr, wildcard.end());
	}

	while (chrText)
	{
		if (chrWildcard == '*')
		{
			utf8::next(wildcardPtr, wildcard.end());

			if (wildcardPtr == wildcard.end())
			{
				return true;
			}

			chrWildcard = utf8::peek_next(wildcardPtr, wildcard.end());

			if (!chrWildcard || wildcardPtr == wildcard.end())
			{
				return true;
			}

			mp = wildcardPtr;
			cp = textPtr;
			utf8::next(cp, text.end());
		}
		else if ((B_WILDCARD_COMPARE_CHAR(chrWildcard)
				== B_WILDCARD_COMPARE_CHAR(chrText))
				|| (chrWildcard == '?'))
		{
			utf8::next(textPtr, text.end());
			utf8::next(wildcardPtr, wildcard.end());

			if (textPtr == text.end())
				break;

			if (wildcardPtr == wildcard.end())
				break;

			chrText = utf8::peek_next(textPtr, text.end());
			chrWildcard = utf8::peek_next(wildcardPtr, wildcard.end());
		}
		else
		{
			wildcardPtr = mp;
			textPtr = cp;
			utf8::next(cp, text.end());

			if (cp == text.end())
				break;

			if (wildcardPtr == wildcard.end())
				break;

			chrText = utf8::peek_next(textPtr, text.end());
			chrWildcard = utf8::peek_next(wildcardPtr, wildcard.end());
		}
	}

	while (chrWildcard == '*')
	{
		utf8::next(wildcardPtr, wildcard.end());

		if (wildcardPtr == wildcard.end())
			break;

		chrWildcard = utf8::peek_next(wildcardPtr, wildcard.end());
	}

	return (!*wildcardPtr);
}

bool isFilesLastTimeSame(const String& filename, const String& otherFilename)
{
	DateTime dt1, dt2;

	if (!getFileDateTime(filename, 0, 0, &dt1))
		return false;

	if (!getFileDateTime(otherFilename, 0, 0, &dt2))
		return false;

	return dt1 == dt2;
}

void setSameFileTime(const String& filename, const String& otherFilename)
{
	DateTime dt;

	bool ok = getFileDateTime(filename, 0, 0, &dt);
	assert(ok);
	ok = setFileDateTime(otherFilename, 0, 0, &dt);
	assert(ok);
}

f32 convertBytesTo(MemoryUnit unit, u64 byteCount)
{
	switch (unit)
	{
	case MemoryUnit::KiloByte:
		return (f32) byteCount / (u64)1024;
		break;
	case MemoryUnit::MegaByte:
		return (f32) byteCount / (u64)1024 / 1024;
		break;
	case MemoryUnit::GigaByte:
		return (f32) byteCount / (u64) 1024 / 1024 / 1024;
		break;
	case MemoryUnit::TeraByte:
		return (f32) byteCount / (u64)1024 / 1024 / 1024 / 1024;
		break;
	case MemoryUnit::PetaByte:
		return (f32)byteCount / (u64)1024 / 1024 / 1024 / 1024 / 1024;
		break;
	default:
		return (f32)byteCount;
	}
}

String convertBytesToText(u64 byteCount)
{
	f32 amount = 0;
	String unit;

	if (byteCount >= (u64)1024
		&& byteCount < (u64)1024 * 1024)
	{
		amount = convertBytesTo(MemoryUnit::KiloByte, byteCount);
		unit = "KB";
	}
	else if (byteCount >= (u64)1024 * 1024
		&& byteCount < (u64)1024 * 1024 * 1024)
	{
		amount = convertBytesTo(MemoryUnit::MegaByte, byteCount);
		unit = "MB";
	}
	else if (byteCount >= (u64)1024 * 1024 * 1024
		&& byteCount < (u64)1024 * 1024 * 1024 * 1024)
	{
		amount = convertBytesTo(MemoryUnit::GigaByte, byteCount);
		unit = "GB";
	}
	else if (byteCount >= (u64)1024 * 1024 * 1024 * 1024
		&& byteCount < (u64)1024 * 1024 * 1024 * 1024 * 1024)
	{
		amount = convertBytesTo(MemoryUnit::TeraByte, byteCount);
		unit = "TB";
	}
	else if (byteCount >= (u64)1024 * 1024 * 1024 * 1024 * 1024
		&& byteCount < (u64)1024 * 1024 * 1024 * 1024 * 1024 * 1024)
	{
		amount = convertBytesTo(MemoryUnit::PetaByte, byteCount);
		unit = "PB";
	}

	return String() << String::floatDecimals(2) << amount << " " << unit << byteCount;
}

Vec3 toVec3(const String& str)
{
	Vec3 v;

	sscanf(str.c_str(), "%f;%f;%f", &v.x, &v.y, &v.z);

	return v;
}

Vec2 toVec2(const String& str)
{
	Vec2 v;

	sscanf(str.c_str(), "%f;%f", &v.x, &v.y);

	return v;
}

Quat toQuat(const String& str)
{
	Vec3 v;
	Quat q;
	f32 a;

	sscanf(str.c_str(), "%f;%f;%f;%f", &a, &v.x, &v.y, &v.z);
	toQuat(a, v, q);

	return q;
}

Color toColor(const String& str)
{
	Color v;

	sscanf(str.c_str(), "%f;%f;%f;%f", &v.r, &v.g, &v.b, &v.a);

	return v;
}

Matrix toMatrix(const String& str)
{
	Matrix m;

	sscanf(str.c_str(), "%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f",
		&m.m[0][0], &m.m[0][1], &m.m[0][2], &m.m[0][3],
		&m.m[1][0], &m.m[1][1], &m.m[1][2], &m.m[1][3],
		&m.m[2][0], &m.m[2][1], &m.m[2][2], &m.m[2][3],
		&m.m[3][0], &m.m[3][1], &m.m[3][2], &m.m[3][3]);

	return m;
}

BBox toBBox(const String& str)
{
	BBox v;
	Vec3 vMin, vMax;

	sscanf(str.c_str(), "%f;%f;%f;%f;%f;%f",
		&vMin.x, &vMin.y, &vMin.z,
		&vMax.x, &vMax.y, &vMax.z);

	v.set(vMin, vMax);

	return v;
}

static const int maxNumberToStringBufferSize = 21;

String charToString(Utf32Codepoint value)
{
	String str;

	str += value;

	return str;
}

String toString(bool value)
{
	return value ? "true" : "false";
}

String toUuidString(u128 value)
{
	String str;
	u8 bytes[16];

	bytes[0] = *((u8*)&value.data[0]);
	bytes[1] = *(((u8*)&value.data[0] + 1));
	bytes[2] = *(((u8*)&value.data[0] + 2));
	bytes[3] = *(((u8*)&value.data[0] + 3));

	bytes[4] = *(((u8*)&value.data[0] + 4));
	bytes[5] = *(((u8*)&value.data[0] + 5));
	bytes[6] = *(((u8*)&value.data[0] + 6));
	bytes[7] = *(((u8*)&value.data[0] + 7));

	bytes[8] = *((u8*)&value.data[1]);
	bytes[9] = *(((u8*)&value.data[1] + 1));
	bytes[10] = *(((u8*)&value.data[1] + 2));
	bytes[11] = *(((u8*)&value.data[1] + 3));

	bytes[12] = *(((u8*)&value.data[1] + 4));
	bytes[13] = *(((u8*)&value.data[1] + 5));
	bytes[14] = *(((u8*)&value.data[1] + 6));
	bytes[15] = *(((u8*)&value.data[1] + 7));

	str << toHexString(bytes[0]) << toHexString(bytes[1]) << toHexString(bytes[2]) << toHexString(bytes[3])
		<< toHexString(bytes[4]) << toHexString(bytes[5]) << toHexString(bytes[6]) << toHexString(bytes[7])
		<< toHexString(bytes[8]) << toHexString(bytes[9]) << toHexString(bytes[10]) << toHexString(bytes[11])
		<< toHexString(bytes[12]) << toHexString(bytes[13]) << toHexString(bytes[14]) << toHexString(bytes[15]);

	return str;
}

String toString(i8 value, int decimals)
{
	char str[maxNumberToStringBufferSize] = { 0 };

	if (decimals)
	{
		String strFmt = "%." + toString((i32)decimals) + "d";
		snprintf(str, maxNumberToStringBufferSize, strFmt.c_str(), value);
	}
	else
		snprintf(str, maxNumberToStringBufferSize, "%d", value);

	return str;
}

String toString(i16 value, int decimals)
{
	char str[maxNumberToStringBufferSize] = { 0 };

	if (decimals)
	{
		String strFmt = "%." + toString((i32)decimals) + "d";
		snprintf(str, maxNumberToStringBufferSize, strFmt.c_str(), value);
	}
	else
		snprintf(str, maxNumberToStringBufferSize, "%d", value);

	return str;
}

String toString(i32 value, int decimals)
{
	char str[maxNumberToStringBufferSize] = { 0 };

	if (decimals)
	{
		String strFmt = "%." + toString((i32)decimals) + "d";
		snprintf(str, maxNumberToStringBufferSize, strFmt.c_str(), value);
	}
	else
		snprintf(str, maxNumberToStringBufferSize, "%d", value);
	
	return str;
}

String toString(i64 value, int decimals)
{
	char str[maxNumberToStringBufferSize] = { 0 };

	if (decimals)
	{
		String strFmt = "%." + toString((i32)decimals) + "lld";
		snprintf(str, maxNumberToStringBufferSize, strFmt.c_str(), value);
	}
	else
		snprintf(str, maxNumberToStringBufferSize, "%lld", value);

	return str;
}

String toString(u8 value, int decimals)
{
	char str[maxNumberToStringBufferSize] = { 0 };

	if (decimals)
	{
		String strFmt = "%." + toString((i32)decimals) + "u";
		snprintf(str, maxNumberToStringBufferSize, strFmt.c_str(), value);
	}
	else
		snprintf(str, maxNumberToStringBufferSize, "%u", value);

	return str;
}

String toString(u16 value, int decimals)
{
	char str[maxNumberToStringBufferSize] = { 0 };

	if (decimals)
	{
		String strFmt = "%." + toString((i32)decimals) + "u";
		snprintf(str, maxNumberToStringBufferSize, strFmt.c_str(), value);
	}
	else
		snprintf(str, maxNumberToStringBufferSize, "%u", value);

	return str;
}

String toString(u32 value, int decimals)
{
	char str[maxNumberToStringBufferSize] = { 0 };

	if (decimals)
	{
		String strFmt = "%." + toString((i32)decimals) + "u";
		snprintf(str, maxNumberToStringBufferSize, strFmt.c_str(), value);
	}
	else
		snprintf(str, maxNumberToStringBufferSize, "%u", value);

	return str;
}

String toString(u64 value, int decimals)
{
	char str[maxNumberToStringBufferSize] = { 0 };

	if (decimals)
	{
		String strFmt = "%." + toString((i32)decimals) + "I64u";
		snprintf(str, maxNumberToStringBufferSize, strFmt.c_str(), value);
	}
	else
		snprintf(str, maxNumberToStringBufferSize, "%I64u", value);

	return str;
}

String toString(f32 value, int decimals)
{
	char str[maxNumberToStringBufferSize] = { 0 };

	if (decimals)
	{
		String strFmt = "%." + toString((i32)decimals) + "f";
		snprintf(str, maxNumberToStringBufferSize, strFmt.c_str(), value);
	}
	else
		snprintf(str, maxNumberToStringBufferSize, "%f", value);

	return str;
}

String toString(f64 value, int decimals)
{
	char str[maxNumberToStringBufferSize] = { 0 };

	if (decimals)
	{
		String strFmt = "%." + toString((i32)decimals) + "g";
		snprintf(str, maxNumberToStringBufferSize, strFmt.c_str(), value);
	}
	else
		snprintf(str, maxNumberToStringBufferSize, "%g", value);

	return str;
}

String toString(const Vec2& value)
{
	return String() << value.x << ";" << value.y;
}

String toString(const Vec3& value)
{
	return String() << value.x << ";" << value.y << ";" << value.z;
}

String toString(const Quat& value)
{
	AngleAxis aa;

	toAngleAxis(value, aa);

	return String() << aa.angle << ";" << aa.axis.x << ";" << aa.axis.y << ";" << aa.axis.z;
}

String toString(const AngleAxis& value)
{
	return String() << value.angle << ";" << value.axis.x << ";" << value.axis.y << ";" << value.axis.z;
}

String toString(const Color& value)
{
	return String() << value.r << ";" << value.g << ";" << value.b << ";" << value.a;
}

String toString(const Matrix& value)
{
	return String() <<
		value.m[0][0] << ";" << value.m[0][1] << ";" << value.m[0][2] << ";" << value.m[0][3] << ";" <<
		value.m[1][0] << ";" << value.m[1][1] << ";" << value.m[1][2] << ";" << value.m[1][3] << ";" <<
		value.m[2][0] << ";" << value.m[2][1] << ";" << value.m[2][2] << ";" << value.m[2][3] << ";" <<
		value.m[3][0] << ";" << value.m[3][1] << ";" << value.m[3][2] << ";" << value.m[3][3];
}

String toString(const BBox& value)
{
	return String() <<
		value.getMinCorner().x << ";" << value.getMinCorner().y << ";" << value.getMinCorner().z << ";" <<
		value.getMaxCorner().x << ";" << value.getMaxCorner().y << ";" << value.getMaxCorner().z;
}

bool fitRectToView(
	f32 imageWidth, f32 imageHeight,
	f32 viewWidth, f32 viewHeight,
	f32& newImageWidth, f32& newImageHeight,
	bool ignoreHeight, bool ignoreWidth)
{
	f32 aspectRatio = 1.0f;
	
	newImageWidth = imageWidth;
	newImageHeight = imageHeight;

	if (imageWidth <= viewWidth
		&& imageHeight <= viewHeight)
	{
		return false;
	}

	if (newImageWidth >= viewWidth && !ignoreWidth)
	{
		if (newImageWidth < 0.0001f)
			newImageWidth = 0.0001f;

		aspectRatio = (f32) viewWidth / newImageWidth;
		newImageWidth = viewWidth;
		newImageHeight *= aspectRatio;
	}

	if (newImageHeight >= viewHeight && !ignoreHeight)
	{
		if (newImageHeight < 0.0001f)
			newImageHeight = 0.0001f;

		aspectRatio = (f32) viewHeight / newImageHeight;
		newImageHeight = viewHeight;
		newImageWidth *= aspectRatio;
	}

	return true;
}

bool loadTextFile(const String& filename, String& outString)
{
	FILE* file = utf8fopen(filename, "r");

	if (!file)
	{
		B_LOG_ERROR("Cannot find text file: " << filename);
		return false;
	}

	outString = "";

	fseek(file, 0, SEEK_END);
	auto fsize = ftell(file);
	fseek(file, 0, SEEK_SET);

	outString.resizeBytes(fsize + 1);
	fread((void*)outString.c_str(), fsize, 1, file);

	return true;
}

FILE* utf8fopen(const String& filename, const String& flags)
{
#ifdef _WINDOWS
	FILE* f = _wfopen(
		(wchar_t*)stringToUtf16(filename).data(),
		(wchar_t*)stringToUtf16(flags).data());

	return f;
#elif defined(_LINUX)
	return fopen(filename.c_str(), flags.c_str());
#else
	B_ASSERT_NOT_IMPLEMENTED;
	return nullptr;
#endif
}

u128 generateUuid()
{
	//TODO: this function needs more unique randomness, the current implementation is naive and collisions will occur
	u128 u;

#ifdef _WINDOWS
	GUID guid;
	CoCreateGuid(&guid);

	*((u32*)&u.data[0]) = guid.Data1;
	*((u16*)&u.data[0] + 2) = guid.Data2;
	*((u16*)&u.data[0] + 3) = guid.Data3;
	u.data[1] = *(u64*)&guid.Data4;
#else
	auto oldSeed = getRandomSeed();

	setRandomSeed(getTimeMilliseconds());

	u8* ptr = (u8*)&u.data[0];

	for (int i = 0; i < 16; i++)
	{
		*ptr = (u8)randomInteger(256);
		ptr++;
	}
	
	setRandomSeed(oldSeed);
#endif

	return u;
}

u64 hashString(const String& str)
{
	JenkinsLookup3Hash64 hasher;

	hasher.add(str.c_str(), str.getByteSize());

	return hasher.getHash64();
}

void utf8ToUtf16NoAlloc(Utf8StringBuffer text, Utf16StringBuffer outText, size_t maxOutTextByteSize)
{
	utf8::utf8to16(text, text + strlen((char*)text) + 1, outText);
}

void utf8ToUtf32NoAlloc(Utf8StringBuffer text, Utf32StringBuffer outText, size_t maxOutTextByteSize)
{
	utf8::utf8to32(text, text + strlen((char*)text) + 1, outText);
}

void stringToUtf16(const String& text, Utf16CodeUnitArray& outText)
{
	outText.resize(text.length() + 1);
	outText.fill(0, outText.size(), 0);

	wchar_t* end = (wchar_t*)utf8::utf8to16((Utf8Byte*)text.c_str(), text.end(), (u16*)outText.data());
	size_t numChars = end - outText.begin();
	Utf16CodeUnitArray newText(outText, 0, numChars);
	outText = newText;
}

Utf16CodeUnitArray stringToUtf16(const String& text)
{
	Utf16CodeUnitArray arr;

	stringToUtf16(text, arr);

	return arr;
}

void utf16ToUtf8NoAlloc(const Utf16StringBuffer text, Utf8StringBuffer outText, size_t maxOutTextByteSize)
{
	utf8::utf16to8(text, text + wcslen((wchar_t*)text) + 1, outText);
}

String stringFromUtf16(Utf16StringBuffer text)
{
	Array<Utf8Byte> str;

	str.resize(wcslen((wchar_t*)text) * 4 + 1); // enough room, plus \0
	auto end = utf8::utf16to8(text, text + wcslen((wchar_t*)text), str.data());
	auto count = (size_t)(end - str.begin());
	
	return String(str.data(), count);
}

void utf32ToUtf8NoAlloc(const Utf32StringBuffer text, const Utf32StringBuffer textEnd, Utf8StringBuffer outText, size_t maxOutTextByteSize)
{
	utf8::utf32to8(text, textEnd, outText);
}

String stringFromUtf32(const Utf32StringBuffer text, const Utf32StringBuffer textEnd)
{
	size_t numChars = textEnd - text;
	Array<Utf8Byte> chars;
	
	chars.resize(numChars * 4 + 1); // enough room, plus \0

	utf32ToUtf8NoAlloc(text, textEnd, chars.data(), chars.size());

	return chars.data();
}

#define asciilower(c) ((u8)tolower((char)(c)))

#define rot(x, k) (((x) << (k)) | ((x) >> (32 - (k))))

#define mix(a, b, c) \
{ \
	a -= c;  a ^= rot(c, 4);  c += b; \
	b -= a;  b ^= rot(a, 6);  a += c; \
	c -= b;  c ^= rot(b, 8);  b += a; \
	a -= c;  a ^= rot(c,16);  c += b; \
	b -= a;  b ^= rot(a,19);  a += c; \
	c -= b;  c ^= rot(b, 4);  b += a; \
}

#define finish(a, b, c) \
{ \
	c ^= b; c -= rot(b,14); \
	a ^= c; a -= rot(c,11); \
	b ^= a; b -= rot(a,25); \
	c ^= b; c -= rot(b,16); \
	a ^= c; a -= rot(c, 4); \
	b ^= a; b -= rot(a,14); \
	c ^= b; c -= rot(b,24); \
}

void JenkinsLookup3Hash64::add(const void* const data, size_t sizeInBytes)
{
	u32 a, b, c;
	a = b = c = 0xdeadbeef + ((u32)sizeInBytes) + hash0;
	c += hash1;

	const u8* k = (const u8*)data;

	while (sizeInBytes > 12)
	{
		a += k[0];
		a += ((u32)k[1]) << 8;
		a += ((u32)k[2]) << 16;
		a += ((u32)k[3]) << 24;
		b += k[4];
		b += ((u32)k[5]) << 8;
		b += ((u32)k[6]) << 16;
		b += ((u32)k[7]) << 24;
		c += k[8];
		c += ((u32)k[9]) << 8;
		c += ((u32)k[10]) << 16;
		c += ((u32)k[11]) << 24;
		mix(a, b, c);
		sizeInBytes -= 12;
		k += 12;
	}

	// last block: affect all 32 bits of (c)
	switch (sizeInBytes)
	{
		// all the case statements fall through
	case 12: c += ((u32)k[11]) << 24;
	case 11: c += ((u32)k[10]) << 16;
	case 10: c += ((u32)k[9]) << 8;
	case  9: c += k[8];
	case  8: b += ((u32)k[7]) << 24;
	case  7: b += ((u32)k[6]) << 16;
	case  6: b += ((u32)k[5]) << 8;
	case  5: b += k[4];
	case  4: a += ((u32)k[3]) << 24;
	case  3: a += ((u32)k[2]) << 16;
	case  2: a += ((u32)k[1]) << 8;
	case  1: a += k[0];
		finish(a, b, c);
	default:
		break;
	}

	hash0 = c;
	hash1 = b;
}

void JenkinsLookup3Hash64::addAsciiLowerCase(const void* const data, size_t sizeInBytes)
{
	u32 a, b, c;
	a = b = c = 0xdeadbeef + ((u32)sizeInBytes) + hash0;
	c += hash1;

	const u8* k = (const u8*)data;


	while (sizeInBytes > 12)
	{
		a += asciilower(k[0]);
		a += ((u32)asciilower(k[1])) << 8;
		a += ((u32)asciilower(k[2])) << 16;
		a += ((u32)asciilower(k[3])) << 24;
		b += asciilower(k[4]);
		b += ((u32)asciilower(k[5])) << 8;
		b += ((u32)asciilower(k[6])) << 16;
		b += ((u32)asciilower(k[7])) << 24;
		c += asciilower(k[8]);
		c += ((u32)asciilower(k[9])) << 8;
		c += ((u32)asciilower(k[10])) << 16;
		c += ((u32)asciilower(k[11])) << 24;
		mix(a, b, c);
		sizeInBytes -= 12;
		k += 12;
	}

	// last block: affect all 32 bits of (c)
	switch (sizeInBytes)
	{
		// all the case statements fall through
	case 12: c += ((u32)asciilower(k[11])) << 24;
	case 11: c += ((u32)asciilower(k[10])) << 16;
	case 10: c += ((u32)asciilower(k[9])) << 8;
	case  9: c += asciilower(k[8]);
	case  8: b += ((u32)asciilower(k[7])) << 24;
	case  7: b += ((u32)asciilower(k[6])) << 16;
	case  6: b += ((u32)asciilower(k[5])) << 8;
	case  5: b += asciilower(k[4]);
	case  4: a += ((u32)asciilower(k[3])) << 24;
	case  3: a += ((u32)asciilower(k[2])) << 16;
	case  2: a += ((u32)asciilower(k[1])) << 8;
	case  1: a += k[0];
		finish(a, b, c);
	default:
		break;
	}

	hash0 = c;
	hash1 = b;
}

}