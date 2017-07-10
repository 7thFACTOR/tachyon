#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/string.h"
#include "base/math/vec2.h"
#include "base/math/vec3.h"
#include "base/math/matrix.h"
#include "base/array.h"
#include "base/logger.h"

namespace base
{
class Quat;
class BBox;
class Ray;
class Color;
class MemoryAllocator;
class DateTime;

enum class SeekMode
{
	Set = 0,
	Current
};

const u64 endOfStream = ~0;

class B_API Stream
{
public:
	Stream();
	virtual ~Stream();
	bool isOpened() const;
	virtual u64 getSize() const = 0;
	virtual u64 getPosition() const = 0;
	virtual bool read(void* data, u64 dataSize) = 0;
	virtual bool write(const void* data, u64 dataSize) = 0;
	virtual bool seek(SeekMode mode, u64 offset) = 0;
	void skip(u64 byteCount);
	virtual bool isEndOfStream() const = 0;
	inline bool isReading() const { return reading; }

	void readAllText(String& str);
	virtual bool readString(String& str);
	virtual bool readLine(String& line);
	bool readBool(bool& value);
	bool readInt8(i8& value);
	bool readInt16(i16& value);
	bool readInt32(i32& value);
	bool readInt64(i64& value);
	bool readUint8(u8& value);
	bool readUint16(u16& value);
	bool readUint32(u32& value);
	bool readUint64(u64& value);
	bool readUint128(u128& value);
	bool readSizeT(size_t& value);
	bool readFloat(f32& value);
	bool readDouble(f64& value);
	bool readColor(Color& value);
	bool readVec2(Vec2& value);
	bool readVec3(Vec3& value);
	bool readBigVec3(BigVec3& value);
	bool readQuat(Quat& value);
	bool readMatrix(Matrix& value);
	bool readBox(BBox& value);
	bool readRay(Ray& value);
	bool readDateTime(DateTime& value);
	template <typename Type>
	bool readEnum(Type& value) { u32 val = 0; bool ret = readUint32(val); value = (Type)val; return ret; }
	template<typename NumberType>
	void readNumber(NumberType& value)
	{
		read(&value, sizeof(NumberType));
	}
	template<typename ArrayType>
	void readArray(Array<ArrayType>& arr)
	{
		size_t s;
		readSizeT(s);
		arr.resize(s);
		for (size_t i = 0; i < s; i++)
		{
			arr[i].streamData(*this);
		}
	}
	template<typename ArrayType>
	void readPodArray(Array<ArrayType>& arr)
	{
		size_t s;
		readSizeT(s);
		arr.resize(s);
		for (size_t i = 0; i < s; i++)
		{
			read(&arr[i], sizeof(ArrayType));
		}
	}

	bool writeBool(bool value);
	bool writeInt8(i8 value);
	bool writeInt16(i16 value);
	bool writeInt32(i32 value);
	bool writeInt64(i64 value);
	bool writeUint8(u8 value);
	bool writeUint16(u16 value);
	bool writeUint32(u32 value);
	bool writeUint64(u64 value);
	bool writeUint128(u128 value);
	bool writeSizeT(size_t value);
	bool writeFloat(f32 value);
	bool writeDouble(f64 value);
	virtual bool writeString(const String& str);
	bool writeColor(const Color& value);
	bool writeVec2(const Vec2& value);
	bool writeVec3(const Vec3& value);
	bool writeBigVec3(const BigVec3& value);
	bool writeQuat(const Quat& value);
	bool writeMatrix(const Matrix& value);
	bool writeBox(const BBox& value);
	bool writeRay(const Ray& value);
	bool writeDateTime(const DateTime& value);
	template <typename Type>
	bool writeEnum(Type value) { bool ret = writeUint32((u32)value); return ret; }
	template<typename NumberType>
	void writeNumber(NumberType value)
	{
		write(&value, sizeof(NumberType));
	}
	template<typename ArrayType>
	void writeArray(Array<ArrayType>& arr)
	{
		writeSizeT(arr.size());
		for (size_t i = 0; i < arr.size(); i++)
		{
			arr[i].streamData(*this);
		}
	}
	template<typename ArrayType>
	void writePodArray(Array<ArrayType>& arr)
	{
		writeSizeT(arr.size());
		for (size_t i = 0; i < arr.size(); i++)
		{
			write(&arr[i], sizeof(ArrayType));
		}
	}
	template<typename ArrayType>
	void streamPodArray(Array<ArrayType>& arr)
	{
		if (isReading())
			readPodArray(arr);
		else
			writePodArray(arr);
	}

	inline Stream& operator >> (String& str) { readString(str); return *this; }
	inline Stream& operator >> (bool& value) { readBool(value); return *this; }
	inline Stream& operator >> (i8& value) { readInt8(value); return *this; }
	inline Stream& operator >> (i16& value) { readInt16(value); return *this; }
	inline Stream& operator >> (i32& value) { readInt32(value); return *this; }
	inline Stream& operator >> (i64& value) { readInt64(value); return *this; }
	inline Stream& operator >> (u8& value) { readInt8((i8&)value); return *this; }
	inline Stream& operator >> (u16& value) { readInt16((i16&)value); return *this; }
	inline Stream& operator >> (u32& value) { readInt32((i32&)value); return *this; }
	inline Stream& operator >> (u64& value) { readInt64((i64&)value); return *this; }
	inline Stream& operator >> (u128& value) { readUint128((u128&)value); return *this; }
	inline Stream& operator >> (f32& value) { readFloat(value); return *this; }
	inline Stream& operator >> (f64& value) { readDouble(value); return *this; }
	inline Stream& operator >> (Color& value) { readColor(value); return *this; }
	inline Stream& operator >> (Vec2& value) { readVec2(value); return *this; }
	inline Stream& operator >> (Vec3& value) { readVec3(value); return *this; }
	inline Stream& operator >> (BigVec3& value) { readBigVec3(value); return *this; }
	inline Stream& operator >> (Quat& value) { readQuat(value); return *this; }
	inline Stream& operator >> (Matrix& value) { readMatrix(value); return *this; }
	inline Stream& operator >> (BBox& value) { readBox(value); return *this; }
	inline Stream& operator >> (Ray& value) { readRay(value); return *this; }
	inline Stream& operator >> (DateTime& value) { readDateTime(value); return *this; }
	template <typename Type>
	inline Stream& operator >> (Type& value)
	{
		enum SizeCheckEnum { SizeCheckEnumItem = 1 };
		B_STATIC_ASSERT(sizeof(Type) == sizeof(SizeCheckEnum));
		u32 val;
		readUint32(val);
		value = (Type)val;
		return *this;
	}
	template <typename Type>
	inline Stream& operator >> (Array<Type>& value)
	{
		readArray(value);
		return *this;
	}

	inline Stream& operator << (const String& str) { writeString(str); return *this; }
	inline Stream& operator << (bool value) { writeBool(value); return *this; }
	inline Stream& operator << (i8 value) { writeInt8(value); return *this; }
	inline Stream& operator << (i16 value) { writeInt16(value); return *this; }
	inline Stream& operator << (i32 value) { writeInt32(value); return *this; }
	inline Stream& operator << (i64 value) { writeInt64(value); return *this; }
	inline Stream& operator << (u8 value) { writeInt8(value); return *this; }
	inline Stream& operator << (u16 value) { writeInt16(value); return *this; }
	inline Stream& operator << (u32 value) { writeInt32(value); return *this; }
	inline Stream& operator << (u64 value) { writeInt64(value); return *this; }
	inline Stream& operator << (u128 value) { writeUint128(value); return *this; }
	inline Stream& operator << (f32 value) { writeFloat(value); return *this; }
	inline Stream& operator << (f64 value) { writeDouble(value); return *this; }
	inline Stream& operator << (const char* str) { writeString(str); return *this; }
	inline Stream& operator << (const Color& value) { writeColor(value); return *this; }
	inline Stream& operator << (const Vec2& value) { writeVec2(value); return *this; }
	inline Stream& operator << (const Vec3& value) { writeVec3(value); return *this; }
	inline Stream& operator << (const BigVec3& value) { writeBigVec3(value); return *this; }
	inline Stream& operator << (const Quat& value) { writeQuat(value); return *this; }
	inline Stream& operator << (const Matrix& value) { writeMatrix(value); return *this; }
	inline Stream& operator << (const BBox& value) { writeBox(value); return *this; }
	inline Stream& operator << (const Ray& value) { writeRay(value); return *this; }
	inline Stream& operator << (const DateTime& value) { writeDateTime(value); return *this; }
	template <typename Type>
	inline Stream& operator << (const Type& value)
	{
		enum SizeCheckEnum { SizeCheckEnumItem = 1 };
		B_STATIC_ASSERT(sizeof(Type) == sizeof(SizeCheckEnum));
		writeUint32((u32)value);
		return *this;
	}
	template <typename Type>
	inline Stream& operator << (Array<Type>& value)
	{
		writeArray(value);
		return *this;
	}

	template <typename Type>
	Stream& operator | (Type& val)
	{
		if (reading)
			*this >> val;
		else
			*this << val;

		return *this;
	}

protected:
	bool opened = false;
	bool reading = false;
};

}