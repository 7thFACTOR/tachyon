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
	resize(strLength);

	memcpy((void*)c_str(), str, strLength);
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

String& String::assign(const CharType* other)
{
	size_t length = wcslen(other);
	
	if (!other)
	{
		B_SAFE_DELETE_ARRAY(longStr);
		bufferSize = 0;
		shortStr[0] = 0;
	}
	else if (!longStr && length < shortStringMaxLength)
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

String& String::append(const CharType* other)
{
	// warning, when appending itself!
	B_ASSERT(other != c_str());
	return appendRange(other, wcslen(other));
}

String& String::appendRange(const String& other, size_t count)
{
	if (count > 0)
	{
		size_t newLength = stringLength + count;

		if (!longStr && (newLength < shortStringMaxLength))
		{
			memcpy(shortStr + stringLength, other.c_str(), count * sizeof(CharType));
			shortStr[newLength] = 0;
		}
		else if (newLength < bufferSize && longStr)
		{
			memcpy(longStr + stringLength, other.c_str(), count * sizeof(CharType));
			longStr[newLength] = 0;
		}
		else
		{
			String oldStr = *this;
			reserve(newLength + newLength / 2);
			CharType* str = (CharType*)c_str();
			memcpy(str, oldStr.c_str(), oldStr.length() * sizeof(CharType));
			memcpy(str + oldStr.length(), other.c_str(), count * sizeof(CharType));
			str[newLength] = 0;
		}
		
		stringLength = newLength;
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

	const CharType* foundStr = wcsstr((const CharType*)(c_str() + startIndex), substr.c_str());

	if (!foundStr)
	{
		return noIndex;
	}

	index = foundStr - c_str();
	
	return index;
}

size_t String::findChar(CharType chr, size_t startIndex) const
{	
	for (size_t i = startIndex; i < stringLength; i++)
	{
		if (chr == c_str()[i])
		{
			return i;
		}
	}
	
	return noIndex;
}

void String::replace(const String& what, const String& with, size_t offset, bool all)
{
	const CharType* str = c_str() + offset;
	size_t whatStrLen = what.length();
	size_t withStrLen = with.length();
	String dest;
	const CharType* occur = nullptr;

	if (what.isEmpty())
	{
		return;
	}

	if (!wcsstr(str, what.c_str()))
	{
		return;
	}

	while (nullptr != (occur = wcsstr(str, what.c_str())))
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

size_t String::parseUntil(size_t startIndex, const CharType* stopChars, String& outParsedString)
{
	while (startIndex < length())
	{
		CharType chr = c_str()[startIndex];

		if (wcschr(stopChars, chr))
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

void String::operator = (const CharType* str)
{
	assign(str);
}

String& String::operator += (const String& other)
{
	return append(other.c_str());
}

String& String::operator += (const CharType* other)
{
	return append(other);
}

String& String::operator += (CharType chr)
{
	CharType c[2] = { chr, 0 };
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

String operator + (const String& str1, const String::CharType chr)
{
	String::CharType c[2] = { chr, 0 };
	String s = str1;
	
	s.append(c);
	
	return s;
}

bool operator == (const String& str1, const String& str2)
{
	return !wcscmp(str1.c_str(), str2.c_str());
}

bool operator == (const String& str1, const String::CharType* str2)
{
	return !wcscmp(str1.c_str(), str2);
}

bool operator == (const String::CharType* str1, const String& str2)
{
	return !wcscmp(str1, str2.c_str());
}

bool operator != (const String& str1, const String& str2)
{
	return 0 != wcscmp(str1.c_str(), str2.c_str());
}

bool operator != (const String& str1, const String::CharType* str2)
{
	return 0 != wcscmp(str1.c_str(), str2);
}

bool operator < (const String& str1, const String& str2)
{
	return wcscmp(str1.c_str(), str2.c_str()) < 0;
}

bool operator > (const String& str1, const String& str2)
{
	return wcscmp(str1.c_str(), str2.c_str()) > 0;
}

bool operator <= (const String& str1, const String& str2)
{
	return wcscmp(str1.c_str(), str2.c_str()) <= 0;
}

bool operator >= (const String& str1, const String& str2)
{
	return wcscmp(str1.c_str(), str2.c_str()) >= 0;
}

String& String::operator << (const String& val)
{
	*this += val;
	return *this;
}

String& String::operator << (const CharType* val)
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

String::CharType& String::operator [](size_t index)
{
	B_ASSERT(B_INBOUNDS(index, 0, stringLength));
	return longStr ? longStr[index] : shortStr[index];
}

String::CharType String::operator [](size_t index) const
{
	B_ASSERT(B_INBOUNDS(index, 0, stringLength));
	return longStr ? longStr[index] : shortStr[index];
}

int String::asInt() const
{
	return _wtoi(c_str());
}

f32 String::asF32() const
{
	return (f32)_wtof(c_str());
}

f64 String::asF64() const
{
	return _wtof(c_str());
}

bool String::asBool() const
{
	if (*this == B_TEXT("true"))
	{
		return true;
	}
	else if (*this == B_TEXT("false"))
	{
		return false;
	}

	return (bool)_wtoi(c_str());
}

}