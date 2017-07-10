#pragma once
#include "base/string.h"
#include "base/defines.h"
#include "base/assert.h"
#include "base/math/vec2.h"
#include "base/math/vec3.h"
#include "base/math/quat.h"
#include "base/math/color.h"
#include "base/math/matrix.h"
#include "base/serialize.h"
#include "base/string.h"

namespace base
{
enum class VariantType
{
	None,
	Int8,
	Int16,
	Int32,
	Int64,
	Float,
	Double,
	String,
	Bool,
	Vec2,
	Vec3,
	BigVec3,
	Color,
	Ptr,
	Quat,
	Matrix,
	
	Count
};

class B_API Variant : public Serializable
{
public:
	Variant(VariantType type = VariantType::None);
	Variant(i8 other);
	Variant(i16 other);
	Variant(i32 other);
	Variant(const i64& other);
	Variant(f32 other);
	Variant(const double& other);
	Variant(bool other);
	Variant(const String& other);
	Variant(const char* other);
	Variant(const Vec2& other);
	Variant(const Vec3& other);
	Variant(const BigVec3& other);
	Variant(void* other);
	Variant(const Color& other);
	Variant(const Quat& other);
	Variant(const Matrix& other);
	Variant(const Variant& other);
	~Variant();

	void toString(String& text) const;
	String toString() const;
	void setFromText(const String& text);
	void serialize(Stream& stream) override;
	void deserialize(Stream& stream) override;
	void clear();

	Variant& operator = (i8 other);
	Variant& operator = (i16 other);
	Variant& operator = (i32 other);
	Variant& operator = (const i64& other);
	Variant& operator = (f32 other);
	Variant& operator = (const double& other);
	Variant& operator = (bool other);
	Variant& operator = (const String& other);
	Variant& operator = (const Vec2& other);
	Variant& operator = (const Vec3& other);
	Variant& operator = (const BigVec3& other);
	Variant& operator = (void* other);
	Variant& operator = (const Color& other);
	Variant& operator = (const Quat& other);
	Variant& operator = (const Matrix& other);
	Variant& operator = (const Variant& other);

	bool operator == (const Variant& other) const;
	bool operator >= (const Variant& other) const;
	bool operator <= (const Variant& other) const;
	bool operator > (const Variant& other) const;
	bool operator < (const Variant& other) const;
	bool operator != (const Variant& other) const;

	operator i8() const
	{
		return int8Val;
	}

	operator i16() const
	{
		return int16Val;
	}

	operator i32() const
	{
		return int32Val;
	}

	operator i64() const
	{
		return int64Val;
	}

	operator f32() const
	{
		return floatVal;
	}

	operator f64() const
	{
		return doubleVal;
	}

	operator bool() const
	{
		return bool8Val;
	}

	operator String() const
	{
		if (!stringVal)
		{
			static String s_str;
			return s_str;
		}

		return *stringVal;
	}

	operator Vec2() const
	{
		return *(Vec2*)&vectorVal;
	}

	operator Vec3() const
	{
		return *(Vec3*)&vectorVal;
	}

	operator BigVec3() const
	{
		return *(BigVec3*)&worldVectorVal;
	}

	operator void*() const
	{
		return ptrVal;
	}

	operator Color() const
	{
		return *(Color*)&colorVal;
	}

	operator Quat() const
	{
		return *(Quat*)&quatVal;
	}

	operator Matrix() const
	{
		if (!matrixVal)
		{
			static Matrix s_mtx;
			return s_mtx;
		}

		return *matrixVal;
	}

	void setType(VariantType type);

	inline VariantType getType() const
	{
		return type;
	}

protected:
	VariantType type = VariantType::None;

	union
	{
		i8 int8Val;
		i16 int16Val;
		i32 int32Val;
		i64 int64Val;
		f32 floatVal;
		f64 doubleVal;
		bool bool8Val;
		void* ptrVal = nullptr;
		f32 vectorVal[3];
		f64 worldVectorVal[3];
		f32 colorVal[4];
		f32 quatVal[4];
		String* stringVal;
		Matrix* matrixVal;
	};
};

}