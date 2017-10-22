// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/streamable.h"

namespace base
{
typedef i8 Utf8Byte;
typedef u32 Utf32Codepoint;

//! An UTF8 string
class B_API String : public Streamable
{
public:
	static const size_t noIndex = ~0;
	typedef Utf8Byte* Iterator;

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

	String(const Utf8Byte* str)
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

	String(Utf8Byte* str, size_t strByteSize);

	~String()
	{
		delete [] longStr;
	}

	void streamData(class Stream& stream) override;
	//! \return the length of the string (of the unicode codepoints)
	inline size_t length() const { return stringLength; }
	inline bool isEmpty() const { return stringLength == 0; }
	inline bool notEmpty() const { return stringLength != 0; }
	inline const Utf8Byte* c_str() const { return longStr != nullptr ? longStr : shortStr; }
	void set(const Utf8Byte* str, size_t byteCount);
	void clear();
	void repeat(const String& str, size_t count);
	void reserve(size_t maxLength);
	String& assign(const String& other);
	String& assign(const Utf8Byte* other);
	String& append(const String& other);
	String& append(const Utf8Byte* other);
	String& append(const Utf8Byte* other, size_t count);
	String& appendRange(const String& other, size_t count);
	String subString(size_t startIndex, size_t count) const;
	size_t find(const String& substr, size_t startIndex = 0) const;
	size_t findChar(Utf32Codepoint chr, size_t startIndex = 0) const;
	void replace(const String& what, const String& with, size_t offset = 0, bool bAll = true);
	void replace(size_t offset, size_t count, const String& with);
	String& erase(size_t start, size_t count);
	void erase(Iterator iter);
	void erase(Iterator first, Iterator last);
	u32& front() const;
	u32& back() const;
	Iterator begin() const;
	Iterator end() const;
	void insert(size_t index, u32 chr);
	void insert(size_t index, const String& str);
	size_t parseUntil(size_t startIndex, const Utf8Byte* stopChars, String& outParsedString);

	void operator = (const String& str);
	void operator = (const Utf8Byte* str);
	String& operator += (const String& other);
	String& operator += (const Utf8Byte* other);
	String& operator += (Utf32Codepoint chr);
	friend B_API String operator + (const String& str1, const String& str2);
	friend B_API String operator + (const String& str1, const Utf8Byte* str2);
	friend B_API String operator + (const String& str1, const Utf32Codepoint chr);
	friend B_API bool operator == (const String& str1, const String& str2);
	friend B_API bool operator == (const String& str1, const Utf8Byte* str2);
	friend B_API bool operator == (const Utf8Byte* str1, const String& str2);
	friend B_API bool operator != (const String& str1, const String& str2);
	friend B_API bool operator != (const String& str1, const Utf8Byte* str2);
	friend B_API bool operator < (const String& str1, const String& str2);
	friend B_API bool operator > (const String& str1, const String& str2);
	friend B_API bool operator <= (const String& str1, const String& str2);
	friend B_API bool operator >= (const String& str1, const String& str2);
	
	String& operator << (const String& val);
	String& operator << (const Utf8Byte* val);
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
	Utf8Byte& operator [](size_t index); // we can modify individual utf8 bytes, but not codepoints
	Utf32Codepoint operator [](size_t index) const;

	int asInt() const;
	f32 asF32() const;
	f64 asF64() const;
	bool asBool() const;

protected:
	void computeLength();
	size_t computeIndexAtAddress(Utf8Byte* addr);
	inline Utf8Byte* getCurrentBuffer() const { return (Utf8Byte*)c_str(); }

	static const u32 shortStringMaxLength = 25;
	static const u32 shortStringMaxByteSize = shortStringMaxLength * 4; //! mul by 4 (max bytes per codepoint)
	
	Utf8Byte* longStr = nullptr;
	Utf8Byte shortStr[shortStringMaxByteSize]; 
	size_t stringLength = 0; // string length in code points
	size_t stringByteSize = 0; // string size in bytes
	size_t bufferSize = 0; // max capacity in bytes
};

}