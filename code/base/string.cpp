// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "base/string.h"
#include "base/assert.h"
#include "base/variant.h"
#include "base/math/util.h"
#include "base/util.h"
#include "base/stream.h"
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include "3rdparty/utf8/source/utf8.h"

namespace base
{
void String::streamData(class Stream& stream)
{
	stream | *this;
}

String::String(const Utf8Byte* str, size_t strLength)
	: longStr(nullptr)
	, longStrSize(0)
	, stringLength(0)
	, stringByteSize(0)
{
	appendRange(String(str), strLength);
}

String::String(String&& str)
{
	longStr = str.longStr;
	strcpy(shortStr, str.shortStr);
	longStrSize = str.longStrSize;
	stringLength = str.stringLength;
	stringByteSize = str.stringByteSize;

	str.longStr = nullptr;
	str.shortStr[0] = 0;
	str.longStrSize = 0;
	str.stringLength = 0;
	str.stringByteSize = 0;
}

void String::reserve(size_t maxLength)
{
	reserveBytes(maxLength * 4 + 1);
}

void String::reserveBytes(size_t maxByteSize)
{
	if (maxByteSize <= shortStringMaxByteSize)
	{
		B_SAFE_DELETE_ARRAY(longStr);
		longStrSize = 0;
		memset(shortStr, 0, shortStringMaxByteSize);
	}
	else if (maxByteSize > longStrSize)
	{
		B_SAFE_DELETE_ARRAY(longStr);
		longStr = new Utf8Byte[maxByteSize];
		B_ASSERT(longStr);
		longStrSize = maxByteSize;
		memset(longStr, 0, longStrSize);
	}
	else if (maxByteSize <= longStrSize)
	{
		if (!longStr)
		{
			longStr = new Utf8Byte[maxByteSize];
			B_ASSERT(longStr);
			longStrSize = maxByteSize;
		}

		memset(longStr, 0, maxByteSize);
	}

	stringLength = 0;
	stringByteSize = 0;
}

void String::resizeBytes(size_t maxByteSize)
{
	reserveBytes(maxByteSize);
	stringLength = 0;
	stringByteSize = maxByteSize - 1; // no ending zero counted in
}

void String::clear()
{
	if (longStr != nullptr)
	{
		delete [] longStr;
	}
	else
	{
		shortStr[0] = 0;
	}
	
	stringLength = stringByteSize = longStrSize = 0;
}

void String::repeat(const String& str, size_t count)
{
	for (size_t i = 0; i < count; i++)
	{
		(*this) += str;
	}
}

String& String::assign(const String& other)
{
	if (*this == other)
	{
		return *this;
	}

	return assign(other.c_str());
}

String& String::assign(const Utf8Byte* other)
{
	size_t sizeInBytes = strlen(other);
	size_t sizeInBytesPlusZero = sizeInBytes + 1;
	
	if (!other)
	{
		B_SAFE_DELETE_ARRAY(longStr);
		longStrSize = 0;
		stringLength = 0;
		stringByteSize = 0;
		shortStr[0] = 0;
	}
	else if (!longStr && sizeInBytesPlusZero <= shortStringMaxByteSize)
	{
		memcpy(shortStr, other, sizeInBytesPlusZero);
		shortStr[sizeInBytes] = 0;
	}
	else if (sizeInBytesPlusZero <= longStrSize && longStr && longStrSize)
	{
		memcpy(longStr, other, sizeInBytesPlusZero);
		longStr[sizeInBytes] = 0;
		shortStr[0] = 0;
	}
	else
	{
		reserveBytes(sizeInBytesPlusZero);
		memcpy(longStr, other, sizeInBytesPlusZero);
		longStr[sizeInBytes] = 0;
		shortStr[0] = 0;
	}

	stringByteSize = sizeInBytes;
	computeLength();
	
	return *this;
}

String& String::append(const String& other)
{
	// warning, when appending itself!
	B_ASSERT(&other != this);
	return appendRange(other, other.stringLength);
}

String& String::append(const Utf8Byte* other)
{
	// warning, when appending itself!
	B_ASSERT(other != c_str());
	return appendBytes(other, strlen(other));
}

String& String::appendCodepoint(Utf32Codepoint cp)
{
	Utf8Byte bytes[4] = { 0 };
	auto end = utf8::utf32to8(&cp, &cp + 1, bytes);

	return appendBytes(bytes, end - bytes);
}

String& String::appendRange(const String& other, size_t count)
{
	// warning, when appending itself!
	B_ASSERT(other.c_str() != c_str());
	B_ASSERT(&other != this);
	auto byteCount = other.computeByteCount(0, count);
	
	return appendBytes(other.c_str(), byteCount);
}

String& String::appendBytes(const Utf8Byte* other, size_t byteCount)
{
	if (byteCount > 0)
	{
		size_t newByteSize = stringByteSize + byteCount;
		size_t newByteSizePlusZero = newByteSize + 1;

		if (!longStr && (newByteSizePlusZero < shortStringMaxByteSize))
		{
			memcpy(shortStr + stringByteSize, other, byteCount);
			shortStr[newByteSize] = 0;
		}
		else if (newByteSizePlusZero < longStrSize && longStr)
		{
			memcpy(longStr + stringByteSize, other, byteCount);
			longStr[newByteSize] = 0;
		}
		else
		{
			String oldStr = *this;
			reserveBytes(newByteSize + newByteSize / 2 + 1);
			Utf8Byte* str = getCurrentBuffer();
			// copy to new buffer, the old string
			memcpy(str, oldStr.c_str(), oldStr.stringByteSize);
			// copy the incoming string
			memcpy(str + oldStr.stringByteSize, other, byteCount);
			stringByteSize = oldStr.stringByteSize;
			str[newByteSize] = 0;
		}
		
		stringByteSize += byteCount;
		computeLength();
	}

	return *this;
}

String String::subString(size_t startIndex, size_t count) const
{
	if (!count)
		return "";

	auto byteOffset = computeByteCount(0, startIndex);
	return String(c_str() + byteOffset, count);
}

size_t String::find(const String& substr, size_t startIndex) const
{
	if (startIndex >= stringLength)
	{
		return noIndex;
	}

	auto startByteOffset = computeByteCount(0, startIndex);

	auto foundStr = strstr((const Utf8Byte*)(c_str() + startByteOffset), substr.c_str());

	if (!foundStr)
	{
		return noIndex;
	}

	return foundStr - c_str();
}

size_t String::findChar(Utf32Codepoint chr, size_t startIndex) const
{	
	Utf8Byte* it = (Utf8Byte*)c_str();

	utf8::advance(it, startIndex, end());

	for (size_t i = startIndex; i < stringLength; i++)
	{
		auto c = utf8::next(it, end());
		
		if (chr == c)
		{
			return i;
		}
	}
	
	return noIndex;
}

void String::replace(const String& what, const String& with, size_t offset, bool all)
{
	auto crtBuffer = getCurrentBuffer();
	utf8::advance(crtBuffer, offset, end());
	const Utf8Byte* str = crtBuffer;
	size_t whatStrLen = what.length();
	size_t withStrLen = with.length();
	String dest;
	const Utf8Byte* occur = nullptr;

	if (what.isEmpty())
	{
		return;
	}

	if (!strstr(str, what.c_str()))
	{
		return;
	}

	while (nullptr != (occur = strstr(str, what.c_str())))
	{
		dest.appendBytes(str, size_t(occur - str));

		if (withStrLen)
		{
			dest.append(with);
		}

		str = occur + whatStrLen;

		if (!all)
			break;
	}

	dest.append(str);
	*this = dest;
}

void String::replace(size_t offset, size_t count, const String& with)
{
	String str = subString(offset, count);
	replace(str, with, offset, false);
}

String& String::erase(size_t start, size_t count)
{
	B_ASSERT(start + count < stringLength);

	auto buffer = getCurrentBuffer();

	auto startBytes = computeByteCount(0, start);
	auto countBytes = computeByteCount(start, count);

	if (longStr)
	{
		Utf8Byte* dst = longStr + startBytes;
		Utf8Byte* src = dst + countBytes;
		Utf8Byte* end = longStr + stringByteSize;
		size_t moveAmount = end - src + 1; // add \0

		memcpy(dst, src, moveAmount);
	}
	else
	{
		Utf8Byte* dst = shortStr + startBytes;
		Utf8Byte* src = dst + countBytes;
		Utf8Byte* end = shortStr + stringByteSize;
		size_t moveAmount = end - src + 1; // add \0
		
		memcpy(dst, src, moveAmount);
	}

	stringLength -= count;
	stringByteSize -= countBytes;

	return *this;
}

Utf32Codepoint String::front() const
{
	return (*this)[0];
}

Utf32Codepoint String::back() const
{
	return (*this)[length() - 1];
}

String::Iterator String::begin() const
{
	return (Utf8Byte*)c_str();
}

String::Iterator String::end() const
{
	return (Utf8Byte*)c_str() + stringByteSize;
}

size_t String::parseUntil(size_t startIndex, const String& stopCodepoints, String& outParsedString)
{
	while (startIndex < length())
	{
		Utf32Codepoint chr = (*this)[startIndex];
		Utf8Byte chr8[4] = { 0 };

		utf8::utf32to8(&chr, &chr + 1, chr8);

		if (strstr(stopCodepoints.c_str(), chr8))
		{
			return startIndex;
		}

		outParsedString += chr;
		startIndex++;
	}

	return startIndex;
}

void String::operator = (const String& str)
{
	assign(str);
}

void String::operator = (const Utf8Byte* str)
{
	assign(str);
}

String& String::operator += (const String& other)
{
	return append(other.c_str());
}

String& String::operator += (const Utf8Byte* other)
{
	return append(other);
}

String& String::operator += (char chr)
{
	return appendCodepoint(chr);
}

String& String::operator += (Utf32Codepoint codepoint)
{
	return appendCodepoint(codepoint);
}

String operator + (const String& str1, const String& str2)
{
	String s = str1;
	
	s.append(str2);
	
	return s;
}

String operator + (const String& str1, const Utf8Byte* str2)
{
	String s = str1;
	
	s.append(str2);

	return s;
}

String operator + (const String& str1, char chr)
{
	String s = str1;
	
	s.appendCodepoint(chr);
	
	return s;
}

String operator + (const String& str1, const Utf32Codepoint chr)
{
	String s = str1;

	s.appendCodepoint(chr);

	return s;
}

bool operator == (const String& str1, const String& str2)
{
	return !strcmp(str1.c_str(), str2.c_str());
}

bool operator == (const String& str1, const Utf8Byte* str2)
{
	return !strcmp(str1.c_str(), str2);
}

bool operator == (const Utf8Byte* str1, const String& str2)
{
	return !strcmp(str1, str2.c_str());
}

bool operator != (const String& str1, const String& str2)
{
	return 0 != strcmp(str1.c_str(), str2.c_str());
}

bool operator != (const String& str1, const Utf8Byte* str2)
{
	return 0 != strcmp(str1.c_str(), str2);
}

bool operator < (const String& str1, const String& str2)
{
	return strcmp(str1.c_str(), str2.c_str()) < 0;
}

bool operator > (const String& str1, const String& str2)
{
	return strcmp(str1.c_str(), str2.c_str()) > 0;
}

bool operator <= (const String& str1, const String& str2)
{
	return strcmp(str1.c_str(), str2.c_str()) <= 0;
}

bool operator >= (const String& str1, const String& str2)
{
	return strcmp(str1.c_str(), str2.c_str()) >= 0;
}

String& String::operator << (const String& val)
{
	*this += val;
	return *this;
}

String& String::operator << (const Utf8Byte* val)
{
	*this += val;
	return *this;
}

String& String::operator << (i8 val)
{
	*this += toString(val, currentIntZeroPadding.count);
	return *this;
}

String& String::operator << (i16 val)
{
	*this += toString(val, currentIntZeroPadding.count);
	return *this;
}

String& String::operator << (i32 val)
{
	*this += toString(val, currentIntZeroPadding.count);
	return *this;
}

String& String::operator << (i64 val)
{
	*this += toString(val, currentIntZeroPadding.count);
	return *this;
}

String& String::operator << (u8 val)
{
	*this += toString(val, currentIntZeroPadding.count);
	return *this;
}

String& String::operator << (u16 val)
{
	*this += toString(val, currentIntZeroPadding.count);
	return *this;
}

String& String::operator << (u32 val)
{
	*this += toString(val, currentIntZeroPadding.count);
	return *this;
}

String& String::operator << (u64 val)
{
	*this += toString(val, currentIntZeroPadding.count);
	return *this;
}

String& String::operator << (u128 val)
{
	*this += toUuidString(val);
	return *this;
}

String& String::operator << (f32 val)
{
	*this += toString(val, currentFloatDecimals.decimals);
	return *this;
}

String& String::operator << (f64 val)
{
	*this += toString(val, currentFloatDecimals.decimals);
	return *this;
}

String& String::operator << (bool val)
{
	*this += toString(val);
	return *this;
}

String& String::operator << (const FloatDecimals& val)
{
	currentFloatDecimals = val;

	return *this;
}

String& String::operator << (const IntZeroPadding& val)
{
	currentIntZeroPadding = val;

	return *this;
}

Utf32Codepoint String::operator [](size_t index) const
{
	B_ASSERT(B_INBOUNDS(index, 0, stringLength));

	auto crtBuffer = getCurrentBuffer();
	utf8::advance(crtBuffer, index, end());

	return utf8::peek_next(crtBuffer, end());
}

int String::asInt() const
{
	return atoi(c_str());
}

f32 String::asF32() const
{
	return (f32)atof(c_str());
}

f64 String::asF64() const
{
	return atof(c_str());
}

bool String::asBool() const
{
	if (*this == "true")
	{
		return true;
	}
	else if (*this == "false")
	{
		return false;
	}

	return (bool)atoi(c_str());
}

void String::computeLength()
{
	stringLength = utf8::distance(getCurrentBuffer(), end());
}

size_t String::computeIndexAtAddress(Utf8Byte* addr) const
{
	auto startAddr = getCurrentBuffer();

	return utf8::distance(startAddr, (Utf8Byte*)addr);
}

size_t String::computeByteCount(size_t startIndex, size_t count) const
{
	auto addrStart = getCurrentBuffer();

	utf8::advance(addrStart, startIndex, end());
	auto addrEnd = addrStart;
	utf8::advance(addrEnd, count, end());

	return addrEnd - addrStart;
}

}