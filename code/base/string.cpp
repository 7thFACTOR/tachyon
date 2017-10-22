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

String::String(Utf8Byte* str, size_t strLength)
	: longStr(nullptr)
	, bufferSize(0)
	, stringLength(0)
{
	append(str, strLength);
}

String::String(String&& str)
{
	longStr = str.longStr;
	wcscpy(shortStr, str.shortStr);
	bufferSize = str.bufferSize;
	stringLength = str.stringLength;

	str.longStr = nullptr;
	str.shortStr[0] = 0;
	str.bufferSize = 0;
	str.stringLength = 0;
}

void String::resize(size_t length, CharType fillChar)
{
	reserve(length);
	stringLength = length;

	CharType* str = (CharType*)c_str();

	if (fillChar)
	{
		memset(str, fillChar, stringLength * sizeof(CharType));
	}

	str[stringLength] = 0;
}

void String::reserve(size_t maxLength)
{
	const size_t maxSize = maxLength * 4 + 1 /*ending \0*/;

	if (maxSize <= shortStringMaxLength * 4)
	{
		B_SAFE_DELETE_ARRAY(longStr);
		bufferSize = 0;
		memset(shortStr, 0, shortStringMaxLength * 4);
	}
	else if (maxSize > bufferSize)
	{
		B_SAFE_DELETE_ARRAY(longStr);
		longStr = new CharType[maxSize];
		B_ASSERT(longStr);
		bufferSize = maxSize;
		memset(longStr, 0, bufferSize * sizeof(CharType));
	}
	else if (maxSize <= bufferSize)
	{
		if (!longStr)
		{
			longStr = new CharType[maxSize];
			B_ASSERT(longStr);
			bufferSize = maxSize;
		}

		memset(longStr, 0, maxSize * sizeof(CharType));
	}

	stringLength = 0;
}

void String::set(const CharType* str, size_t count)
{
	if (!str)
	{
		B_SAFE_DELETE_ARRAY(longStr);
		memset(shortStr, 0, shortStringMaxLength * sizeof(CharType));
		stringLength = 0;
		bufferSize = 0;
	}
	else if ((nullptr == longStr) && (count < shortStringMaxLength))
	{
		B_SAFE_DELETE_ARRAY(longStr);
		bufferSize = 0;
		memcpy(shortStr, str, count * sizeof(CharType));
		shortStr[count] = 0;
	}
	else if (longStr != nullptr && count < bufferSize)
	{
		memcpy(longStr, str, count * sizeof(CharType));
		longStr[count] = 0;
		memset(shortStr, 0, shortStringMaxLength * sizeof(CharType));
	}
	else
	{
		// need to allocate bigger heap buffer
		reserve(count + 1); //+ \0

		if (longStr != nullptr)
		{
			memcpy(longStr, str, count * sizeof(CharType));
			longStr[count] = 0;
			memset(shortStr, 0, shortStringMaxLength * sizeof(CharType));
		}
	}

	stringLength = count;
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
	
	stringLength = bufferSize = 0;
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
	
	if (!other)
	{
		B_SAFE_DELETE_ARRAY(longStr);
		bufferSize = 0;
		shortStr[0] = 0;
	}
	else if (!longStr && sizeInBytes < shortStringMaxLength * 4)
	{
		memcpy(shortStr, other, length * sizeof(CharType));
		shortStr[length] = 0;
	}
	else if (length < bufferSize && longStr && bufferSize)
	{
		memcpy(longStr, other, length * sizeof(CharType));
		longStr[length] = 0;
		shortStr[0] = 0;
	}
	else
	{
		reserve(length + 1);
		memcpy(longStr, other, length * sizeof(CharType));
		longStr[length] = 0;
		shortStr[0] = 0;
	}

	stringLength = length;
	
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
	return appendRange(other, wcslen(other));
}

String& String::append(const Utf8Byte* other, size_t count)
{
	// warning, when appending itself!
	B_ASSERT(other != c_str());
	return appendRange(other, count);
}

String& String::appendRange(const String& other, size_t count)
{
	if (count > 0)
	{
		auto rangeByteSize = other.computeRangeByteSize(0, count);
		size_t newByteSize = stringByteSize + rangeByteSize;

		if (!longStr && (newByteSize < shortStringMaxLength * 4))
		{
			memcpy(shortStr + stringByteSize, other.c_str(), rangeByteSize);
			shortStr[newByteSize] = 0;
		}
		else if (newByteSize < bufferSize && longStr)
		{
			memcpy(longStr + stringByteSize, other.c_str(), rangeByteSize);
			longStr[newByteSize] = 0;
		}
		else
		{
			String oldStr = *this;
			reserveBytes(newByteSize + newByteSize / 2);
			char* str = (char*)c_str();
			// copy to new buffer, the old string
			memcpy(str, oldStr.c_str(), oldStr.stringByteSize);
			// copy the incoming string
			memcpy(str + oldStr.stringByteSize, other.c_str(), rangeByteSize);
			str[newByteSize] = 0;
		}
		
		computeLength();
	}

	return *this;
}

String String::subString(size_t startIndex, size_t count) const
{
	String substr;
	
	substr.appendRange(c_str() + startIndex, count);
	
	return substr;
}

size_t String::find(const String& substr, size_t startIndex) const
{
	size_t index;

	if (startIndex >= stringLength)
	{
		return noIndex;
	}

	const Utf8Byte* foundStr = strstr((const Utf8Byte*)(c_str() + startIndex), substr.c_str());

	if (!foundStr)
	{
		return noIndex;
	}

	index = foundStr - c_str();
	
	return index;
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
		dest.appendRange(str, size_t(occur - str));

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

	if (longStr)
	{
		CharType* dst = longStr + start;
		CharType* src = dst + count;
		CharType* end = longStr + stringLength;
		size_t moveAmount = end - src + 1; // add \0
		memcpy(dst, src, moveAmount * sizeof(CharType));
	}
	else
	{
		CharType* dst = shortStr + start;
		CharType* src = dst + count;
		CharType* end = shortStr + stringLength;
		size_t moveAmount = end - src + 1; // add \0
		memcpy(dst, src, moveAmount * sizeof(CharType));
	}

	stringLength -= count;

	return *this;
}

void String::erase(Iterator iter)
{
	if (longStr)
	{
		size_t tip = (size_t)longStr + (size_t)stringLength * sizeof(CharType);
		memcpy(iter, iter + 1, tip - (size_t)(iter + 1));
	}
	else
	{
		size_t tip = (size_t)shortStr + (size_t)stringLength * sizeof(CharType);
		memcpy(iter, iter + 1, tip - (size_t)(iter + 1));
	}

	--stringLength;
}

void String::erase(Iterator first, Iterator last)
{
	if (longStr)
	{
		CharType* dst = first;
		CharType* src = last;
		CharType* end = longStr + stringLength;
		size_t moveAmount = end - src;
		memcpy(dst, src, moveAmount * sizeof(CharType));
	}
	else
	{
		CharType* dst = first;
		CharType* src = last;
		CharType* end = shortStr + stringLength;
		size_t moveAmount = end - src;
		memcpy(dst, src, moveAmount * sizeof(CharType));
	}

	stringLength -= last - first;
}

String::CharType& String::front() const
{
	B_ASSERT(stringLength);
	B_ASSERT(c_str());
	return ((CharType*)c_str())[0];
}

String::CharType& String::back() const
{
	B_ASSERT(stringLength);
	B_ASSERT(c_str());
	return ((CharType*)c_str())[stringLength - 1];
}

String::Iterator String::begin() const
{
	return (CharType*)c_str();
}

String::Iterator String::end() const
{
	return (CharType*)c_str() + stringLength;
}

void String::insert(size_t index, CharType chr)
{
	String oldStr = *this;

	if (longStr)
	{
		if (stringLength + 1 >= bufferSize)
		{
			reserve(stringLength + 1);
		}
	}
	else
	{
		if (stringLength + 1 >= shortStringMaxLength)
		{
			reserve(stringLength + 1);
		}
	}

	CharType* thisPtr = (CharType*)c_str();

	if (index)
	{
		memcpy(thisPtr, oldStr.c_str(), index * sizeof(CharType));
	}

	thisPtr[index] = chr;

	if (!oldStr.isEmpty())
	{
		memcpy(thisPtr + index + 1, oldStr.c_str() + index, (oldStr.length() - index) * sizeof(CharType));
	}

	thisPtr[oldStr.length() + 1] = 0;
	stringLength = oldStr.length() + 1;
}

void String::insert(size_t index, const String& str)
{
	size_t lenStr = str.length();
	String origStr = *this;
	
	if (longStr)
	{
		if (stringLength + lenStr >= bufferSize)
		{
			reserve(stringLength + lenStr);
		}
	}
	else
	{
		if (stringLength + lenStr >= shortStringMaxLength)
		{
			reserve(stringLength + lenStr);
		}
	}

	CharType* thisPtr = (CharType*)c_str();

	memcpy(thisPtr, origStr.c_str(), index * sizeof(CharType));
	memcpy(thisPtr + index, str.c_str(), lenStr * sizeof(CharType));
	memcpy(thisPtr + index + lenStr, origStr.c_str() + index, (origStr.length() - index) * sizeof(CharType));
	stringLength = origStr.length() + lenStr;
}

size_t String::parseUntil(size_t startIndex, const Utf8Byte* stopChars, String& outParsedString)
{
	while (startIndex < length())
	{
		Utf32Codepoint chr = (*this)[startIndex];
		Utf8Byte chr8[4] = { 0 };

		utf8::utf32to8(&chr, &chr + 1, chr8);

		if (strstr(stopChars, chr8))
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
	Utf8Byte c[2] = { chr, 0 };
	return append(c);
}

String operator + (const String& str1, const String& str2)
{
	String s = str1;
	
	s.append(str2);
	
	return s;
}

String operator + (const String& str1, const String::CharType* str2)
{
	String s = str1;
	
	s.append(str2);

	return s;
}

String operator + (const String& str1, const Utf32Codepoint chr)
{
	Utf8Byte c[2] = { chr, 0 };
	String s = str1;
	
	s.append(c);
	
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

Utf8Byte& String::operator [](size_t index)
{
	B_ASSERT(B_INBOUNDS(index, 0, stringLength));
	
	auto crtBuffer = getCurrentBuffer();

	return crtBuffer[index];
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
	auto startAddr = getCurrentBuffer();

	stringLength = utf8::distance(startAddr, end());
}

size_t String::computeIndexAtAddress(Utf8Byte* addr)
{
	auto startAddr = getCurrentBuffer();

	return utf8::distance(startAddr, addr);
}

}