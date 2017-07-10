#include <string.h>
#include <stdio.h>
#include <wchar.h>
#include "base/stream.h"
#include "base/util.h"
#include "base/platform.h"
#include "base/assert.h"
#include "base/memory.h"
#include "base/logger.h"
#include "base/math/bbox.h"
#include "base/math/ray.h"
#include "base/math/matrix.h"

namespace base
{
Stream::Stream()
	: opened(false)
{}

Stream::~Stream()
{}

bool Stream::readString(String& str)
{
	u32 len = 0;

	readUint32(len);
	str.resize(len);

	if (!len)
	{
		return true;
	}

	return read((void*)str.c_str(), len);
}

bool Stream::readLine(String& line)
{
	char chr;
	i32 i = 0;

	line = "";

	if (isEndOfStream())
	{
		return false;
	}

	do
	{
		read(&chr, 1);

		if (chr)
		{
			line += chr;
		}

		++i;
	} while (chr && chr != '\r' && chr != '\n' && chr != 0xFF && (!isEndOfStream()));

	return true;
}

void Stream::readAllText(String& str)
{
	str = "";

	u64 fileSize = getSize();

	char* newStr = new char[fileSize + 1];
	B_ASSERT(newStr);

	if (!newStr)
		return;

	memset(newStr, 0, fileSize + 1);
	read(newStr, fileSize);
	str = newStr;
	delete[] newStr;
}

bool Stream::writeInt16(i16 value)
{
	return write(&value, sizeof(i16));
}

bool Stream::writeInt32(i32 value)
{
	return write(&value, sizeof(i32));
}

bool Stream::writeInt64(i64 value)
{
	return write(&value, sizeof(i64));
}

bool Stream::writeUint8(u8 value)
{
	return write(&value, sizeof(u8));
}

bool Stream::writeUint16(u16 value)
{
	return write(&value, sizeof(u16));
}

bool Stream::writeUint32(u32 value)
{
	return write(&value, sizeof(u32));
}

bool Stream::writeUint64(u64 value)
{
	return write(&value, sizeof(u64));
}

bool Stream::writeUint128(u128 value)
{
	return write(&value, sizeof(u128));
}

bool Stream::writeSizeT(size_t value)
{
	return write(&value, sizeof(size_t));
}

bool Stream::writeFloat(f32 value)
{
	return write(&value, sizeof(f32));
}

bool Stream::writeDouble(f64 value)
{
	return write(&value, sizeof(f64));
}

bool Stream::writeInt8(i8 value)
{
	return write(&value, sizeof(i8));
}

bool Stream::writeBool(bool value)
{
	return write(&value, sizeof(bool));
}

void Stream::skip(u64 byteCount)
{
	seek(SeekMode::Current, byteCount);
}

bool Stream::writeString(const String& str)
{
	writeUint32(str.length());
	write((void*)str.c_str(), str.length());

	return true;
}

bool Stream::isOpened() const
{
	return opened;
}

bool Stream::readBool(bool& value)
{
	return read(&value, sizeof(bool));
}

bool Stream::readInt16(i16& value)
{
	return read(&value, sizeof(i16));
}

bool Stream::readInt32(i32& value)
{
	return read(&value, sizeof(i32));
}

bool Stream::readInt64(i64& value)
{
	return read(&value, sizeof(u64));
}

bool Stream::readUint8(u8& value)
{
	return read(&value, sizeof(u8));
}

bool Stream::readUint16(u16& value)
{
	return read(&value, sizeof(u16));
}

bool Stream::readUint32(u32& value)
{
	return read(&value, sizeof(u32));
}

bool Stream::readUint64(u64& value)
{
	return read(&value, sizeof(u64));
}

bool Stream::readUint128(u128& value)
{
	return read(&value, sizeof(u128));
}

bool Stream::readSizeT(size_t& value)
{
	return read(&value, sizeof(size_t));
}

bool Stream::readFloat(f32& value)
{
	return read(&value, sizeof(f32));
}

bool Stream::readDouble(f64& value)
{
	return read(&value, sizeof(f64));
}

bool Stream::readInt8(i8& value)
{
	return read(&value, sizeof(i8));
}

bool Stream::readColor(Color& value)
{
	return read(&value, sizeof(Color));
}

bool Stream::readVec2(Vec2& value)
{
	return read(&value, sizeof(Vec2));
}

bool Stream::readVec3(Vec3& value)
{
	return read(&value, sizeof(Vec3));
}

bool Stream::readBigVec3(BigVec3& value)
{
	return read(&value, sizeof(BigVec3));
}

bool Stream::readQuat(Quat& value)
{
	return read(&value, sizeof(Quat));
}

bool Stream::readMatrix(Matrix& value)
{
	return read(&value, sizeof(Matrix));
}

bool Stream::readBox(BBox& value)
{
	return read(&value, sizeof(BBox));
}

bool Stream::readRay(Ray& value)
{
	Vec3 dir, start;

	//TODO: warning on alignment
	read(&start.x, sizeof(Vec3));
	read(&dir.x, sizeof(Vec3));
	value.setStart(start);
	value.setDirection(dir);

	return true;
}

bool Stream::readDateTime(DateTime& value)
{
	readUint16(value.year);
	readUint16(value.month);
	readUint16(value.day);
	readUint16(value.weekDay);
	readUint16(value.hour);
	readUint16(value.minute);
	readUint16(value.second);
	readUint16(value.millisecond);

	return true;
}

bool Stream::writeColor(const Color& value)
{
	return write((void*)&value.r, sizeof(Color));
}

bool Stream::writeVec2(const Vec2& value)
{
	return write((void*)&value.x, sizeof(Vec2));
}

bool Stream::writeVec3(const Vec3& value)
{
	return write((void*)&value.x, sizeof(Vec3));
}

bool Stream::writeBigVec3(const BigVec3& value)
{
	return write((void*)&value.x, sizeof(BigVec3));
}

bool Stream::writeQuat(const Quat& value)
{
	return write((void*)&value.x, sizeof(Quat));
}

bool Stream::writeMatrix(const Matrix& value)
{
	return write((void*)value.m, sizeof(f32) * 16);
}

bool Stream::writeBox(const BBox& value)
{
	write((void*)&value.getMinCorner().x, sizeof(Vec3));
	write((void*)&value.getMaxCorner().x, sizeof(Vec3));

	return true;
}

bool Stream::writeRay(const Ray& value)
{
	write((void*)&value.getStartPoint().x, sizeof(Vec3));
	write((void*)&value.getDirection().x, sizeof(Vec3));

	return true;
}

bool Stream::writeDateTime(const DateTime& value)
{
	writeUint16(value.year);
	writeUint16(value.month);
	writeUint16(value.day);
	writeUint16(value.weekDay);
	writeUint16(value.hour);
	writeUint16(value.minute);
	writeUint16(value.second);
	writeUint16(value.millisecond);

	return true;
}

}