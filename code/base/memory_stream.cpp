// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "base/memory_stream.h"
#include <string.h>

namespace base
{
MemoryStream::MemoryStream()
{}

MemoryStream::MemoryStream(u8* readData, size_t readDataSize)
{
	openForRead(readData, readDataSize);
}

MemoryStream::~MemoryStream()
{
	close();
}

void MemoryStream::openForRead(u8* newData, size_t newDataSize)
{
	close();
	data = newData;
	dataSize = newDataSize;
	forRead = true;
	opened = true;
	currentPosition = 0;
}

void MemoryStream::openForWrite(size_t maxDataSize)
{
	close();
	maxWriteDataSize = maxDataSize;
	forRead = false;
	opened = true;
	currentPosition = 0;
}

void MemoryStream::close()
{
	if (opened)
	{
		if (forRead)
		{
			data = nullptr;
		}
		else
		{
			delete[] data;
		}

		dataSize = 0;
		currentPosition = 0;
		totalDataSize = 0;
		opened = false;
	}
}

bool MemoryStream::read(void* dstData, u64 dstSize)
{
	if (!opened)
	{
		return false;
	}

	if (forRead)
	{
		if (dataSize - currentPosition >= dstSize)
		{
			memcpy(dstData, data, dstSize);
			currentPosition += dstSize;

			return true;
		}

		return false;
	}

	B_ASSERT(!"MemoryStream opened for read, not for write");

	return false;
}

bool MemoryStream::write(const void* srcData, u64 srcSize)
{
	if (!opened)
	{
		return false;
	}

	if (!forRead)
	{
		if (totalDataSize - currentPosition >= srcSize)
		{
			memcpy(data + currentPosition, srcData, srcSize);
			currentPosition += srcSize;

			return true;
		}
		else
		{
			if (totalDataSize + srcSize <= maxWriteDataSize)
			{
				totalDataSize = totalDataSize + srcSize + writeDataGrowSize;
				//TODO: pool ? realloc?
				u8* newData = new u8[totalDataSize];
				memcpy(newData, data, dataSize - currentPosition);
				delete[] data;
				data = newData;
				dataSize = dataSize + srcSize;
				memcpy(data + currentPosition, srcData, srcSize);
				currentPosition += srcSize;

				return true;
			}
		}

		return false;
	}

	B_ASSERT(!"MemoryStream opened for read, not for write");

	return false;
}

bool MemoryStream::seek(SeekMode mode, u64 offset)
{
	if (!opened)
	{
		return false;
	}

	if (mode == SeekMode::Set)
	{
		currentPosition = offset;
	}
	else if (mode == SeekMode::Current)
	{
		currentPosition += offset;
	}

	return true;
}

bool MemoryStream::isEndOfStream() const
{
	if (!opened)
	{
		return true;
	}

	if (currentPosition >= dataSize)
	{
		return true;
	}

	return false;
}

}