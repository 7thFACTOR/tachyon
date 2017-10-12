// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "base/variant.h"
#include "base/platform.h"
#include "base/util.h"
#include "base/assert.h"
#include "base/file.h"

namespace base
{
Variant::Variant(const VariantType newType)
{
	type = newType;
	clear();
}

Variant::Variant(i8 other)
{
	clear();
	*this = other;
}

Variant::Variant(i16 other)
{
	clear();
	*this = other;
}

Variant::Variant(i32 other)
{
	clear();
	*this = other;
}

Variant::Variant(const i64& other)
{
	clear();
	*this = other;
}

Variant::Variant(const f32 other)
{
	clear();
	*this = other;
}

Variant::Variant(const double& other)
{
	clear();
	*this = other;
}

Variant::Variant(bool other)
{
	clear();
	*this = other;
}

Variant::Variant(const String& other)
{
	clear();
	*this = other;
}

Variant::Variant(const char* other)
{
	clear();
	*this = String(other);
}

Variant::Variant(const Vec2& other)
{
	clear();
	*this = other;
}

Variant::Variant(const Vec3& other)
{
	clear();
	*this = other;
}

Variant::Variant(const BigVec3& other)
{
	clear();
	*this = other;
}

Variant::Variant(void* other)
{
	clear();
	*this = other;
}

Variant::Variant(const Color& other)
{
	clear();
	*this = other;
}

Variant::Variant(const Quat& other)
{
	clear();
	*this = other;
}

Variant::Variant(const Variant& other)
{
	clear();
	*this = other;
}

Variant::~Variant()
{
	if (type == VariantType::String)
	{
		delete stringVal;
	}
	else if (type == VariantType::Matrix)
	{
		delete matrixVal;
	}
}

void Variant::setType(VariantType newType)
{
	if (type == VariantType::String)
	{
		B_SAFE_DELETE(stringVal);
	}
	else if (type == VariantType::Matrix)
	{
		B_SAFE_DELETE(matrixVal);
	}

	type = newType;

	switch(type)
	{
	case VariantType::String:
		stringVal = new String();
		break;
	case VariantType::Matrix:
		matrixVal = new Matrix();
		break;
	}
}

void Variant::deserialize(Stream& stream)
{
	type = VariantType::None;
	u8 tip = 0;
	stream >> tip;

	type = (VariantType)tip;

	switch (type)
	{
	case VariantType::None:
		break;
	case VariantType::Int8:
		stream >> int8Val;
		break;
	case VariantType::Int16:
		stream >> int16Val;
		break;
	case VariantType::Int32:
		stream >> int32Val;
		break;
	case VariantType::Int64:
		stream >> int64Val;
		break;
	case VariantType::Float:
		stream >> floatVal;
		break;
	case VariantType::Double:
		stream >> doubleVal;
		break;
	case VariantType::String:
		B_ASSERT(stringVal);
		stream >> *stringVal;
		break;
	case VariantType::Bool:
		stream >> bool8Val;
		break;
	case VariantType::Vec2:
		stream >> *(Vec2*)&vectorVal[0];
		break;
	case VariantType::Vec3:
		stream >> *(Vec3*)&vectorVal[0];
		break;
	case VariantType::BigVec3:
		stream >> *(BigVec3*)&worldVectorVal[0];
		break;
	case VariantType::Color:
		stream >> *(Color*)&colorVal[0];
		break;
	case VariantType::Quat:
		stream >> *(Quat*)&quatVal[0];
		break;
	case VariantType::Matrix:
		stream >> *matrixVal;
		break;
	};
}

void Variant::serialize(Stream& stream)
{
	stream << (u8)type;

	switch (type)
	{
	case VariantType::None:
		break;
	case VariantType::Int8:
		stream.writeInt8(int8Val);
		break;
	case VariantType::Int16:
		stream.writeInt16(int16Val);
		break;
	case VariantType::Int32:
		stream.writeInt32(int32Val);
		break;
	case VariantType::Int64:
		stream.writeInt64(int64Val);
		break;
	case VariantType::Float:
		stream.writeFloat(floatVal);
		break;
	case VariantType::Double:
		stream.writeDouble(doubleVal);
		break;
	case VariantType::String:
		B_ASSERT(stringVal);
		stream.writeString(*stringVal);
		break;
	case VariantType::Bool:
		stream.writeBool(bool8Val);
		break;
	case VariantType::Ptr:
		stream.writeInt64((i64)ptrVal);
		break;
	case VariantType::Vec2:
		stream.write(&vectorVal, sizeof(Vec2));
		break;
	case VariantType::Vec3:
		stream.write(&vectorVal, sizeof(Vec3));
		break;
	case VariantType::BigVec3:
		stream.write(&worldVectorVal, sizeof(BigVec3));
		break;
	case VariantType::Color:
		stream.write(&colorVal, sizeof(Color));
		break;
	case VariantType::Quat:
		stream.write(&quatVal, sizeof(Quat));
		break;
	case VariantType::Matrix:
		stream.write(matrixVal, sizeof(Matrix));
		break;
	};
}

void Variant::clear()
{
	memset(quatVal, 0, sizeof(quatVal[0]) * 4);
}

void Variant::toString(String& text) const
{
	switch (type)
	{
	case VariantType::None:
		text = "";
		break;
	case VariantType::Int8:
		text << int8Val;
		break;
	case VariantType::Int16:
		text << int16Val;
		break;
	case VariantType::Int32:
		text << int32Val;
		break;
	case VariantType::Int64:
		text << int64Val;
		break;
	case VariantType::Float:
		text << floatVal;
		break;
	case VariantType::Double:
		text << doubleVal;
		break;
	case VariantType::String:
		text = *stringVal;
		break;
	case VariantType::Bool:
		text = bool8Val ? "true" : "false";
		break;
	case VariantType::Ptr:
		text << (u64)ptrVal;
		break;
	case VariantType::Vec2:
		text << vectorVal[0] << ";" << vectorVal[1];
		break;
	case VariantType::Vec3:
		text << vectorVal[0] << ";" << vectorVal[1] << ";" << vectorVal[2];
		break;
	case VariantType::BigVec3:
		text << worldVectorVal[0] << ";" << worldVectorVal[1] << ";" << worldVectorVal[2];
		break;
	case VariantType::Color:
		text
			<< colorVal[0] << ";"
			<< colorVal[1] << ";"
			<< colorVal[2] << ";"
			<< colorVal[3];
		break;
	case VariantType::Quat:
		text
			<< quatVal[0] << ";"
			<< quatVal[1] << ";"
			<< quatVal[2] << ";"
			<< quatVal[3];
	case VariantType::Matrix:
		text
			<< (*matrixVal)[0] << ";" << (*matrixVal)[1] << ";" << (*matrixVal)[2] << ";" << (*matrixVal)[3] << ";"
			<< (*matrixVal)[4] << ";" << (*matrixVal)[5] << ";" << (*matrixVal)[6] << ";" << (*matrixVal)[7] << ";"
			<< (*matrixVal)[8] << ";" << (*matrixVal)[9] << ";" << (*matrixVal)[10] << ";" << (*matrixVal)[11] << ";"
			<< (*matrixVal)[12] << ";" << (*matrixVal)[13] << ";" << (*matrixVal)[14] << ";" << (*matrixVal)[15];
		break;
	};
}

String Variant::toString() const
{
	String text;

	toString(text);

	return text;
}

void Variant::setFromText(const String& text)
{
	setType(type);

	switch (type)
	{
	case VariantType::None:
		break;
	case VariantType::Int8:
		int8Val = text.asInt();
		break;
	case VariantType::Int16:
		int16Val = text.asInt();
		break;
	case VariantType::Int32:
		int32Val = text.asInt();
		break;
	case VariantType::Int64:
		int64Val = text.asInt();
		break;
	case VariantType::Float:
		floatVal = text.asF32();
		break;
	case VariantType::Double:
		doubleVal = text.asF64();
		break;
	case VariantType::String:
		*stringVal = text;
		break;
	case VariantType::Bool:
		bool8Val = text == "true";
		break;
	case VariantType::Vec2:
		sscanf(
			text.c_str(), "%f;%f",
			&vectorVal[0],
			&vectorVal[1]);
		break;
	case VariantType::Vec3:
		sscanf(
			text.c_str(), "%f;%f;%f",
			&vectorVal[0],
			&vectorVal[1],
			&vectorVal[2]);
		break;
	case VariantType::BigVec3:
		sscanf(
			text.c_str(), "%lg;%lg;%lg",
			&worldVectorVal[0],
			&worldVectorVal[1],
			&worldVectorVal[2]);
		break;
	case VariantType::Color:
		sscanf(
			text.c_str(), "%f;%f;%f;%f",
			&colorVal[0],
			&colorVal[1],
			&colorVal[2],
			&colorVal[3]);
		break;
	case VariantType::Quat:
		sscanf(
			text.c_str(), "%f;%f;%f;%f",
			&quatVal[0],
			&quatVal[1],
			&quatVal[2],
			&quatVal[3]);
		break;
	case VariantType::Matrix:
		sscanf(
			text.c_str(), "%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f",
			&(*matrixVal).m[0], &(*matrixVal).m[1], &(*matrixVal).m[2], &(*matrixVal).m[3],
			&(*matrixVal).m[4], &(*matrixVal).m[5], &(*matrixVal).m[6], &(*matrixVal).m[7],
			&(*matrixVal).m[8], &(*matrixVal).m[9], &(*matrixVal).m[10], &(*matrixVal).m[11],
			&(*matrixVal).m[12], &(*matrixVal).m[13], &(*matrixVal).m[14], &(*matrixVal).m[15]);
		break;
	};
}

Variant& Variant::operator = (i8 other)
{
	int8Val = other;
	type = VariantType::Int8;
	return *this;
}

Variant& Variant::operator = (i16 other)
{
	int16Val = other;
	type = VariantType::Int16;
	return *this;
}

Variant& Variant::operator = (i32 other)
{
	int32Val = other;
	type = VariantType::Int32;
	return *this;
}

Variant& Variant::operator = (const i64& other)
{
	int64Val = other;
	type = VariantType::Int64;
	return *this;
}

Variant& Variant::operator = (f32 other)
{
	floatVal = other;
	type = VariantType::Float;
	return *this;
}

Variant& Variant::operator = (const double& other)
{
	doubleVal = other;
	type = VariantType::Double;
	return *this;
}

Variant& Variant::operator = (bool other)
{
	bool8Val = other;
	type = VariantType::Bool;
	return *this;
}

Variant& Variant::operator = (const String& other)
{
	setType(VariantType::String);
	*stringVal = other;
	return *this;
}

Variant& Variant::operator = (const Vec2& other)
{
	vectorVal[0] = other.x;
	vectorVal[1] = other.y;
	type = VariantType::Vec2;
	return *this;
}

Variant& Variant::operator = (const Vec3& other)
{
	vectorVal[0] = other.x;
	vectorVal[1] = other.y;
	vectorVal[2] = other.z;
	type = VariantType::Vec3;
	return *this;
}

Variant& Variant::operator = (const BigVec3& other)
{
	worldVectorVal[0] = other.x;
	worldVectorVal[1] = other.y;
	worldVectorVal[2] = other.z;
	type = VariantType::BigVec3;
	return *this;
}

Variant& Variant::operator = (void* other)
{
	ptrVal = other;
	type = VariantType::Ptr;
	return *this;
}

Variant& Variant::operator = (const Color& other)
{
	colorVal[0] = other.r;
	colorVal[1] = other.g;
	colorVal[2] = other.b;
	colorVal[3] = other.a;
	type = VariantType::Color;
	return *this;
}

Variant& Variant::operator = (const Quat& other)
{
	quatVal[0] = other.x;
	quatVal[1] = other.y;
	quatVal[2] = other.z;
	quatVal[3] = other.w;
	type = VariantType::Quat;
	return *this;
}

Variant& Variant::operator = (const Matrix& other)
{
	setType(VariantType::Matrix);
	*matrixVal = other;
	return *this;
}

Variant& Variant::operator = (const Variant& other)
{
	setType(other.type);

	switch (type)
	{
	case VariantType::None:
		break;
	case VariantType::Int8:
		int8Val = other.int8Val;
		break;
	case VariantType::Int16:
		int16Val = other.int16Val;
		break;
	case VariantType::Int32:
		int32Val = other.int32Val;
		break;
	case VariantType::Int64:
		int64Val = other.int64Val;
		break;
	case VariantType::Float:
		floatVal = other.floatVal;
		break;
	case VariantType::Double:
		doubleVal = other.doubleVal;
		break;
	case VariantType::String:
		*stringVal = *other.stringVal;
		break;
	case VariantType::Bool:
		bool8Val = other.bool8Val;
		break;
	case VariantType::Ptr:
		ptrVal = other.ptrVal;
		break;
	case VariantType::Vec2:
		vectorVal[0] = other.vectorVal[0];
		vectorVal[1] = other.vectorVal[1];
		break;
	case VariantType::Vec3:
		vectorVal[0] = other.vectorVal[0];
		vectorVal[1] = other.vectorVal[1];
		vectorVal[2] = other.vectorVal[2];
		break;
	case VariantType::BigVec3:
		worldVectorVal[0] = other.worldVectorVal[0];
		worldVectorVal[1] = other.worldVectorVal[1];
		worldVectorVal[2] = other.worldVectorVal[2];
		break;
	case VariantType::Color:
		colorVal[0] = other.colorVal[0];
		colorVal[1] = other.colorVal[1];
		colorVal[2] = other.colorVal[2];
		colorVal[3] = other.colorVal[3];
		break;
	case VariantType::Quat:
		quatVal[0] = other.quatVal[0];
		quatVal[1] = other.quatVal[1];
		quatVal[2] = other.quatVal[2];
		quatVal[3] = other.quatVal[3];
		break;
	case VariantType::Matrix:
		*matrixVal = *other.matrixVal;
		break;
	};

	return *this;
}

bool Variant::operator == (const Variant& other) const
{
	if (type != other.type)
	{
		B_ASSERT(!"Different Variant types, cannot compare.");
		return false; // different types, cant check
	}

	switch (type)
	{
	case VariantType::None:
		break;
	case VariantType::Int8:
		return int8Val == other.int8Val;
		break;
	case VariantType::Int16:
		return int16Val == other.int16Val;
		break;
	case VariantType::Int32:
		return int32Val == other.int32Val;
		break;
	case VariantType::Int64:
		return int64Val == other.int64Val;
		break;
	case VariantType::Float:
		return fabs(floatVal - other.floatVal) < 0.0001f;
		break;
	case VariantType::Double:
		return fabs(doubleVal - other.doubleVal) < 0.0001f;
		break;
	case VariantType::String:
		if (other.stringVal)
		{
			return *stringVal == *other.stringVal;
		}
		break;
	case VariantType::Bool:
		return bool8Val == other.bool8Val;
		break;
	case VariantType::Ptr:
		return ptrVal == other.ptrVal;
		break;
	case VariantType::Vec2:
		return (*(Vec2*)&vectorVal).isEquivalent(*(Vec2*)&other.vectorVal);
		break;
	case VariantType::Vec3:
		return (*(Vec3*)&vectorVal).isEquivalent(*(Vec3*)&other.vectorVal);
		break;
	case VariantType::BigVec3:
		return (*(BigVec3*)&worldVectorVal).isEquivalent(*(BigVec3*)&other.worldVectorVal);
		break;
	case VariantType::Color:
		return *(Color*)&colorVal == *(Color*)&other.colorVal;
		break;
	case VariantType::Quat:
		return *(Quat*)&quatVal == *(Quat*)&other.quatVal;
		break;
	case VariantType::Matrix:
		if (other.matrixVal)
		{
			return *matrixVal == *other.matrixVal;
		}
		break;
	};

	return false;
}

bool Variant::operator >= (const Variant& other) const
{
	if (type != other.type)
	{
		B_ASSERT(!"Different Variant types, cannot compare.");
		return false; // different types, cant check
	}

	switch (type)
	{
	case VariantType::None:
		break;
	case VariantType::Int8:
		return int8Val >= other.int8Val;
		break;
	case VariantType::Int16:
		return int16Val >= other.int16Val;
		break;
	case VariantType::Int32:
		return int32Val >= other.int32Val;
		break;
	case VariantType::Int64:
		return int64Val >= other.int64Val;
		break;
	case VariantType::Float:
		return floatVal >= other.floatVal;
		break;
	case VariantType::Double:
		return doubleVal >= other.doubleVal;
		break;
	case VariantType::String:
		return *stringVal >= *other.stringVal;
		break;
	case VariantType::Bool:
		return bool8Val >= other.bool8Val;
		break;
	case VariantType::Ptr:
		return ptrVal >= other.ptrVal;
		break;
	case VariantType::Vec2:
		return *(Vec2*)&vectorVal >= *(Vec2*)&other.vectorVal;
		break;
	case VariantType::Vec3:
		return *(Vec3*)&vectorVal >= *(Vec3*)&other.vectorVal;
		break;
	case VariantType::BigVec3:
		return *(BigVec3*)&worldVectorVal >= *(BigVec3*)&other.worldVectorVal;
		break;
	case VariantType::Color:
		return *(Color*)&colorVal >= *(Color*)&other.colorVal;
		break;
	case VariantType::Quat:
		return *(Quat*)&quatVal >= *(Quat*)&other.quatVal;
		break;
	case VariantType::Matrix:
		// not supported
		break;
	};

	return false;
}

bool Variant::operator <= (const Variant& other) const
{
	if (type != other.type)
	{
		B_ASSERT(!"Different Variant types, cannot compare.");
		return false; // different types, cant check
	}

	switch (type)
	{
	case VariantType::None:
		break;
	case VariantType::Int8:
		return int8Val <= other.int8Val;
		break;
	case VariantType::Int16:
		return int16Val <= other.int16Val;
		break;
	case VariantType::Int32:
		return int32Val <= other.int32Val;
		break;
	case VariantType::Int64:
		return int64Val <= other.int64Val;
		break;
	case VariantType::Float:
		return floatVal <= other.floatVal;
		break;
	case VariantType::Double:
		return doubleVal <= other.doubleVal;
		break;
	case VariantType::String:
		return *stringVal <= *other.stringVal;
		break;
	case VariantType::Bool:
		return bool8Val <= other.bool8Val;
		break;
	case VariantType::Ptr:
		return ptrVal <= other.ptrVal;
		break;
	case VariantType::Vec2:
		return *(Vec2*)&vectorVal <= *(Vec2*)&other.vectorVal;
		break;
	case VariantType::Vec3:
		return *(Vec3*)&vectorVal <= *(Vec3*)&other.vectorVal;
		break;
	case VariantType::BigVec3:
		return *(BigVec3*)&worldVectorVal <= *(BigVec3*)&other.worldVectorVal;
		break;
	case VariantType::Color:
		return *(Color*)&colorVal <= *(Color*)&other.colorVal;
		break;
	case VariantType::Quat:
		return *(Quat*)&quatVal <= *(Quat*)&other.quatVal;
		break;
	case VariantType::Matrix:
		// not supported
		break;
	};

	return false;
}

bool Variant::operator > (const Variant& other) const
{
	if (type != other.type)
	{
		B_ASSERT(!"Different Variant types, cannot compare.");
		return false; // different types, cant check
	}

	switch (type)
	{
	case VariantType::None:
		break;
	case VariantType::Int8:
		return int8Val > other.int8Val;
		break;
	case VariantType::Int16:
		return int16Val > other.int16Val;
		break;
	case VariantType::Int32:
		return int32Val > other.int32Val;
		break;
	case VariantType::Int64:
		return int64Val > other.int64Val;
		break;
	case VariantType::Float:
		return floatVal > other.floatVal;
		break;
	case VariantType::Double:
		return doubleVal > other.doubleVal;
		break;
	case VariantType::String:
		return *stringVal > *other.stringVal;
		break;
	case VariantType::Bool:
		return bool8Val > other.bool8Val;
		break;
	case VariantType::Ptr:
		return ptrVal > other.ptrVal;
		break;
	case VariantType::Vec2:
		return *(Vec2*)&vectorVal > *(Vec2*)&other.vectorVal;
		break;
	case VariantType::Vec3:
		return *(Vec3*)&vectorVal > *(Vec3*)&other.vectorVal;
		break;
	case VariantType::BigVec3:
		return *(BigVec3*)&worldVectorVal > *(BigVec3*)&other.worldVectorVal;
		break;
	case VariantType::Color:
		return *(Color*)&colorVal > *(Color*)&other.colorVal;
		break;
	case VariantType::Quat:
		return *(Quat*)&quatVal > *(Quat*)&other.quatVal;
		break;
	case VariantType::Matrix:
		return *matrixVal > *other.matrixVal;
		break;
	};

	return false;
}

bool Variant::operator < (const Variant& other) const
{
	if (type != other.type)
	{
		B_ASSERT(!"Different Variant types, cannot compare.");
		return false; // different types, cant check
	}

	switch (type)
	{
	case VariantType::None:
		break;
	case VariantType::Int8:
		return int8Val < other.int8Val;
		break;
	case VariantType::Int16:
		return int16Val < other.int16Val;
		break;
	case VariantType::Int32:
		return int32Val < other.int32Val;
		break;
	case VariantType::Int64:
		return int64Val < other.int64Val;
		break;
	case VariantType::Float:
		return floatVal < other.floatVal;
		break;
	case VariantType::Double:
		return doubleVal < other.doubleVal;
		break;
	case VariantType::String:
		return *stringVal < *other.stringVal;
		break;
	case VariantType::Bool:
		return bool8Val < other.bool8Val;
		break;
	case VariantType::Ptr:
		return ptrVal < other.ptrVal;
		break;
	case VariantType::Vec2:
		return *(Vec2*)&vectorVal < *(Vec2*)&other.vectorVal;
		break;
	case VariantType::Vec3:
		return *(Vec3*)&vectorVal < *(Vec3*)&other.vectorVal;
		break;
	case VariantType::BigVec3:
		return *(BigVec3*)&worldVectorVal < *(BigVec3*)&other.worldVectorVal;
		break;
	case VariantType::Color:
		return *(Color*)&colorVal < *(Color*)&other.colorVal;
		break;
	case VariantType::Quat:
		return *(Quat*)&quatVal < *(Quat*)&other.quatVal;
		break;
	};

	return false;
}

bool Variant::operator != (const Variant& other) const
{
	if (type != other.type)
	{
		B_ASSERT(!"Different Variant types, cannot compare.");
		return true; // different types, cant check
	}

	switch (type)
	{
	case VariantType::None:
		break;
	case VariantType::Int8:
		return int8Val != other.int8Val;
		break;
	case VariantType::Int16:
		return int16Val != other.int16Val;
		break;
	case VariantType::Int32:
		return int32Val != other.int32Val;
		break;
	case VariantType::Int64:
		return int64Val != other.int64Val;
		break;
	case VariantType::Float:
		return fabs(floatVal - other.floatVal) > 0.0001f;
		break;
	case VariantType::Double:
		return fabs(doubleVal - other.doubleVal) > 0.0001f;
		break;
	case VariantType::String:
		return *stringVal != *other.stringVal;
		break;
	case VariantType::Bool:
		return bool8Val != other.bool8Val;
		break;
	case VariantType::Ptr:
		return ptrVal != other.ptrVal;
		break;
	case VariantType::Vec2:
		return *(Vec2*)&vectorVal != *(Vec2*)&other.vectorVal;
		break;
	case VariantType::Vec3:
		return *(Vec3*)&vectorVal != *(Vec3*)&other.vectorVal;
		break;
	case VariantType::BigVec3:
		return *(BigVec3*)&worldVectorVal != *(BigVec3*)&other.worldVectorVal;
		break;
	case VariantType::Color:
		return *(Color*)&colorVal != *(Color*)&other.colorVal;
		break;
	case VariantType::Quat:
		return *(Quat*)&quatVal != *(Quat*)&other.quatVal;
		break;
	};

	return false;
}

}