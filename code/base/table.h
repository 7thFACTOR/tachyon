#pragma once
#include "base/types.h"
#include "base/defines.h"
#include "base/assert.h"
#include "base/logger.h"
#include <string.h>

namespace base
{
template <class Type, bool isPodType>
class Table
{
public:
	Table();
	Table(const Table& other)
	{
		*this = other;
	}

	Table(u32 width, u32 height);
	~Table();

	Type& value(u32 x, u32 y) const;
	u32 getLength() const;
	u32 getWidth() const;
	u32 getHeight() const;
	Type* getData() const;
	void fill(const Type& value);
	void setValue(u32 x, u32 y, const Type& value);
	void resize(u32 width, u32 height);
	void free();
	bool validIndices(u32 x, u32 y) const;

	const Table& operator = (const Table& other);
	Type& operator [](size_t index);

protected:
	static const bool isPod = isPodType;

	Type *data = nullptr;
	u32 *offsets = nullptr;
	u32 width = 0;
	u32 height = 0;
	u32 length = 0;
};

//////////////////////////////////////////////////////////////////////////

template <class Type, bool isPodType>
Table<Type, isPodType>::Table()
{
	data = nullptr;
	offsets = nullptr;
	width = height = length = 0;
}

template <class Type, bool isPodType>
Table<Type, isPodType>::Table(u32 newWidth, u32 newHeight)
{
	data = nullptr;
	offsets = nullptr;
	width = height = length = 0;
	resize(newWidth, newHeight);
}

template <class Type, bool isPodType>
Table<Type, isPodType>::~Table()
{
	free();
}

template <class Type, bool isPodType>
void Table<Type, isPodType>::free()
{
	if (data)
	{
		delete [] data;
	}

	if (offsets)
	{
		delete [] offsets;
	}

	data = nullptr;
	offsets = nullptr;
	width = height = length = 0;
}

template <class Type, bool isPodType>
Type& Table<Type, isPodType>::value(u32 x, u32 y) const
{
	B_ASSERT_MSG(x < width && y < height, "Table value indices out of range !");
	return data[offsets[y] + x];
}

template <class Type, bool isPodType>
bool Table<Type, isPodType>::validIndices(u32 x, u32 y) const
{
	return x < width && y < height;
}

template <class Type, bool isPodType>
u32 Table<Type, isPodType>::getLength() const
{
	return length;
}

template <class Type, bool isPodType>
void Table<Type, isPodType>::setValue(u32 x, u32 y, const Type& value)
{
	B_ASSERT_MSG(x < width && y < height, "Table setValue out of range !");
	data[offsets[y] + x] = value;
}

template <class Type, bool isPodType>
void Table<Type, isPodType>::resize(u32 newWidth, u32 newHeight)
{
	free();

	if (newWidth == 0 || newHeight == 0)
		return;

	width = newWidth;
	height = newHeight;
	length = newWidth * newHeight;
	data = new Type [length + 1];
	B_ASSERT(data);

	offsets = new u32 [newHeight];
	B_ASSERT(offsets);

	for (u32 i = 0; i < newHeight; ++i)
	{
		offsets[i] = newWidth * i;
	}
}

template <class Type, bool isPodType>
u32 Table<Type, isPodType>::getWidth() const
{
	return width;
}

template <class Type, bool isPodType>
u32 Table<Type, isPodType>::getHeight() const
{
	return height;
}

template <class Type, bool isPodType>
Type* Table<Type, isPodType>::getData() const
{
	return data;
}

template <class Type, bool isPodType>
void Table<Type, isPodType>::fill(const Type& value)
{
	if (length <= 0)
	{
		return;
	}

	i32 i = length - 1;

	while (i >= 0)
	{
		data[i--] = value;
	}
}

template <class Type, bool isPodType>
const Table<Type, isPodType>& Table<Type, isPodType>::operator = (
	const Table<Type,
	isPodType>& other)
{
	resize(other.width, other.height);

	if (isPodType)
	{
		memcpy(data, other.data, length);
	}
	else
	{
		for (u32 i = 0; i < length; ++i)
		{
			data[i] = other.data[i];
		}
	}

	return *this;
}

template <class Type, bool isPodType>
Type& Table<Type, isPodType>::operator [](size_t index)
{
	B_ASSERT_MSG(index >= 0 && index < length, "[] table 2D operator, index out of range !");
	return data[index];
}

typedef Table<u32, true> TableU32;
typedef Table<i32, true> TableI32;
typedef Table<f32, true> TableF32;
typedef Table<f64, true> TableF64;

}