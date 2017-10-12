// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/assert.h"
#include "base/types.h"
#include "base/defines.h"
#include "base/array.h"

namespace base
{
//TODO: add Allocator support
template <typename Type>
class ChunkedArray
{
public:
	ChunkedArray();
	ChunkedArray(const ChunkedArray& other);
	ChunkedArray(size_t chunkCount, size_t countPerChunk);
	~ChunkedArray();

	Type* add(const Type& elem);
	Type* addEmpty();
	void eraseAt(size_t index);
	void erase(Type* what);
	void clear();
	void free();
	void resize(size_t chunkCount, size_t countPerChunk);

	inline size_t count() const
	{
		return totalCount;
	}
	
	Type& operator [](size_t index);
	const Type& operator [](size_t index) const;

	inline ChunkedArray& operator = (const ChunkedArray& other)
	{
		chunks = other.chunks; //TODO: create chunks and copy from other
		freeChunkItems = other.freeChunkItems;
		countPerChunk = other.countPerChunk;
		totalCount = other.totalCount;

		return *this;
	}

protected:
	bool addNewChunk();

	Array<Type*> chunks;
	Array<size_t> freeChunkItems;
	size_t countPerChunk;
	size_t totalCount = 0;
};

//////////////////////////////////////////////////////////////////////////

template <typename Type>
ChunkedArray<Type>::ChunkedArray()
	: countPerChunk(10)
	, totalCount(0)
{}

template <typename Type>
ChunkedArray<Type>::ChunkedArray(const ChunkedArray& other)
{
	resize(other.chunks.size(), other.countPerChunk);
}

template <typename Type>
ChunkedArray<Type>::ChunkedArray(size_t chunkCount, size_t countPerChunk)
{
	resize(chunkCount, countPerChunk);
}

template <typename Type>
Type* ChunkedArray<Type>::add(const Type& elem)
{
	for (size_t i = 0; i < freeChunkItems.size(); ++i)
	{
		if (freeChunkItems[i] > 0)
		{
			for (size_t j = 0; j < countPerChunk; ++j)
			{
				if (!chunks[i][j].used)
				{
					chunks[i][j] = elem;
					chunks[i][j].used = true;
					--freeChunkItems[i];
					return &chunks[i][j];
				}
			}
		}
	}

	// we need more chunks
	if (addNewChunk())
	{
		size_t chunkCount = chunks.size();

		chunks[chunkCount][0] = elem;
		chunks[chunkCount][0].used = true;
		--freeChunkItems[chunkCount];
		return &chunks[chunkCount][0];
	}

	return nullptr;
}

template <typename Type>
Type* ChunkedArray<Type>::addEmpty()
{
	return add(Type());
}

template <typename Type>
void ChunkedArray<Type>::eraseAt(size_t index)
{
	size_t relIndex = index % countPerChunk;
	size_t chunkIndex = (chunks.size() * countPerChunk) / index;
	
	chunks[chunkIndex][relIndex].used = false;
	++freeChunkItems[chunkIndex];
}

template <typename Type>
void ChunkedArray<Type>::erase(Type* what)
{
	for (size_t i = 0; i < chunks.size(); ++i)
	{
		if (what >= chunks.front()
			&& what < chunks.back())
		{
			++freeChunkItems[i];
			what->used = false;
		}
	}
}

template <typename Type>
void ChunkedArray<Type>::clear()
{
	// just make all items unused
	for (size_t i = 0; i < chunks.size(); ++i)
	{
		for (size_t j = 0; j < countPerChunk; ++j)
		{
			chunks[i][j].used = false;
		}
		
		freeChunkItems[i] = countPerChunk;
	}
}

template <typename Type>
void ChunkedArray<Type>::resize(size_t newChunkCount, size_t newCountPerChunk)
{
	free();
	chunks.resize(chunkCount);
	countPerChunk = newCountPerChunk;
	totalCount = newChunkCount * countPerChunk;

	for (size_t i = 0; i < chunks.size(); ++i)
	{
		chunks[i] = new Type[countPerChunk];
		freeChunkItems[i] = countPerChunk;
	}
}

template <typename Type>
bool ChunkedArray<Type>::addNewChunk()
{
	Type* items = new Type[countPerChunk];

	B_ASSERT(items);

	chunks.append(items);
	freeChunkItems.append(countPerChunk);
	totalCount = chunks.size() * countPerChunk;

	return true;
}

template <typename Type>
void ChunkedArray<Type>::free()
{
	for (size_t i = 0; i < chunks.size(); ++i)
	{
		delete [] chunks[i];
	}

	chunks.clear();
	freeChunkItems.clear();
	countPerChunk = 0;
	totalCount = 0;
}

template <typename Type>
ChunkedArray<Type>::~ChunkedArray()
{
	free();
}

template <typename Type>
Type& ChunkedArray<Type>::operator [](size_t index)
{
	size_t relIndex = index % countPerChunk;
	size_t chunkIndex = (chunks.size() * countPerChunk) / index;

	return chunks[chunkIndex][relIndex];
}

template <typename Type>
const Type& ChunkedArray<Type>::operator [](size_t index) const
{
	size_t relIndex = index % countPerChunk;
	size_t chunkIndex = (chunks.size() * countPerChunk) / index;

	return chunks[chunkIndex][relIndex];
}

}