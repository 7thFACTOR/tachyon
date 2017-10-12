// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/streamable.h"

namespace base
{
//! An ASCII/UTF8 string
class B_API String : public Streamable
{
public:
	static const size_t noIndex = ~0;
	typedef char* Iterator;

	struct B_API FloatDecimals
	{
		FloatDecimals() {}
		FloatDecimals(u32 decimalPlaces)
			: decimals(decimalPlaces)
		{}

		u32 decimals = 0;
	} currentFloatDecimals;

	struct B_API IntZeroPadding
	{
		IntZeroPadding() {}
		IntZeroPadding(u32 zeroesCount)
			: count(zeroesCount)
		{}

		u32 count = 0;
	} currentIntZeroPadding;

	String()
		: longStr(nullptr)
		, bufferSize(0)
		, stringLength(0)
	{
		shortStr[0] = 0;
	}

	String(const char* str)
		: longStr(nullptr)
		, bufferSize(0)
		, stringLength(0)
	{
		shortStr[0] = 0;
		assign(str);
	}
	
	String(const String& str)
		: longStr(nullptr)
		, bufferSize(0)
		, stringLength(0)
	{
		shortStr[0] = 0;
		assign(str);
	}

	String(String&& str);

	String(size_t maxLength)
		: longStr(nullptr)
		, bufferSize(0)
		, stringLength(0)
	{
		shortStr[0] = 0;
		reserve(maxLength);
	}

	String(char* str, size_t strLength);

	~String()
	{
		delete [] longStr;
	}

	void streamData(class Stream& stream) override;

	inline size_t length() const { return stringLength; }
	inline bool isEmpty() const { return stringLength == 0; }
	inline bool notEmpty() const { return stringLength != 0; }
	inline const char* c_str() const { return longStr != nullptr ? longStr : shortStr; }
	void set(const char* str, size_t count);
	void clear();
	void resize(size_t length, char fillChar = 0);
	void reserve(size_t maxLength);
	String& assign(const String& other);
	String& assign(const char* other);
	String& append(const String& other);
	String& append(const char* other);
	String& appendRange(const String& other, size_t count);
	String subString(size_t startIndex, size_t count) const;
	size_t find(const String& substr, size_t startIndex = 0) const;
	size_t findChar(char chr, size_t startIndex = 0) const;
	void replace(const String& what, const String& with, size_t offset = 0, bool bAll = true);
	void replace(size_t offset, size_t count, const String& with);
	String& erase(size_t start, size_t count);
	void erase(Iterator iter);
	void erase(Iterator first, Iterator last);
	char& front() const;
	char& back() const;
	Iterator begin() const;
	Iterator end() const;
	void insert(size_t index, char chr);
	void insert(size_t index, const String& str);
	size_t parseUntil(size_t startIndex, const char* stopChars, String& outParsedString);

	void operator = (const String& str);
	void operator = (const char* str);
	String& operator += (const String& other);
	String& operator += (const char* other);
	String& operator += (char chr);
	friend B_API String operator + (const String& str1, const String& str2);
	friend B_API String operator + (const String& str1, const char* str2);
	friend B_API String operator + (const String& str1, const char chr);
	friend B_API bool operator == (const String& str1, const String& str2);
	friend B_API bool operator == (const String& str1, const char* str2);
	friend B_API bool operator == (const char* str1, const String& str2);
	friend B_API bool operator != (const String& str1, const String& str2);
	friend B_API bool operator != (const String& str1, const char* str2);
	friend B_API bool operator < (const String& str1, const String& str2);
	friend B_API bool operator > (const String& str1, const String& str2);
	friend B_API bool operator <= (const String& str1, const String& str2);
	friend B_API bool operator >= (const String& str1, const String& str2);
	
	String& operator << (const String& val);
	String& operator << (const char* val);
	String& operator << (i8 val);
	String& operator << (i16 val);
	String& operator << (i32 val);
	String& operator << (i64 val);
	String& operator << (u8 val);
	String& operator << (u16 val);
	String& operator << (u32 val);
	String& operator << (u64 val);
	String& operator << (u128 val);
	String& operator << (f32 val);
	String& operator << (f64 val);
	String& operator << (bool val);
	String& operator << (const FloatDecimals& val);
	String& operator << (const IntZeroPadding& val);
	
	static inline FloatDecimals floatDecimals(u32 decimals) { return { decimals }; }
	static inline IntZeroPadding intDecimals(u32 decimals) { return { decimals }; }
	char& operator [](size_t index);
	char operator [](size_t index) const;

	int asInt() const;
	f32 asF32() const;
	f64 asF64() const;
	bool asBool() const;

protected:
	static const u32 shortStringSize = 25;
	
	char* longStr = nullptr;
	char shortStr[shortStringSize];
	size_t stringLength = 0;
	size_t bufferSize = 0;
};

}