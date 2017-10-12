// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include <string.h>
#include "base/thread_queue.h"
#include "base/logger.h"
#include "base/math/util.h"
#include "base/math/vec3.h"
#include "base/math/matrix.h"
#include "base/math/color.h"

namespace base
{
const u32 threadCommandQueueInitialAllocSize = 1024 * 64;
const u32 threadCommandQueueAllocInAdvanceAmount = 2048;
const u32 threadCommandQueueMaxBufferSize = 1024 * 1024;

ThreadCommandQueue::ThreadCommandQueue()
{
	maxAllowedBufferSize = threadCommandQueueMaxBufferSize;
	buffer = new u8[threadCommandQueueInitialAllocSize];
	location = buffer;
	totalBufferSize = threadCommandQueueInitialAllocSize;
	cmdDataSize = 0;
	cmdCount = 0;
}

ThreadCommandQueue::~ThreadCommandQueue()
{
	B_SAFE_DELETE_ARRAY(buffer);
}

bool ThreadCommandQueue::lock()
{
	if (queueMutex.isAcquired())
	{
		return false;
	}

	queueMutex.acquire();

	return true;
}

void ThreadCommandQueue::unlock()
{
	queueMutex.release();
}

bool ThreadCommandQueue::waitForLock()
{
	queueMutex.tryAcquire();

	return true;
}

bool ThreadCommandQueue::beginWrite()
{
	cmdDataSize = 0;
	cmdCount = 0;
	location = buffer;

	return true;
}

void ThreadCommandQueue::endWrite()
{}

bool ThreadCommandQueue::checkSpace(size_t size)
{
	if (totalBufferSize < cmdDataSize + size)
	{
		if (cmdDataSize + size > maxAllowedBufferSize)
			return false;

		u8* newBuffer = new u8[cmdDataSize + size];
		B_ASSERT(newBuffer);

		if (!newBuffer)
		{
			return false;
		}

		size_t offset = location - buffer;

		if (buffer)
		{
			memcpy(newBuffer, buffer, totalBufferSize);
			delete [] buffer;
		}

		buffer = newBuffer;
		totalBufferSize = cmdDataSize + size + threadCommandQueueAllocInAdvanceAmount;
		cmdDataSize += size;
		location = buffer + offset;
	}
	else
	{
		cmdDataSize += size;
	}

	return true;
}

bool ThreadCommandQueue::writeCommandId(u16 id)
{
	if (id)
	{
		if (!checkSpace(sizeof(u16)))
			return false;

		*((u16*)location) = id;
		location += sizeof(u16);
	}

	++cmdCount;

	return true;
}

bool ThreadCommandQueue::writeInt8(i8 param)
{
	if (!checkSpace(sizeof(i8)))
		return false;

	*((i8*)location) = param;
	location += sizeof(i8);

	return true;
}

bool ThreadCommandQueue::writeInt16(i16 param)
{
	if (!checkSpace(sizeof(i16)))
		return false;

	*((i16*)location) = param;
	location += sizeof(i16);

	return true;
}

bool ThreadCommandQueue::writeInt32(i32 param)
{
	if (!checkSpace(sizeof(i32)))
		return false;

	*((i32*)location) = param;
	location += sizeof(i32);

	return true;
}

bool ThreadCommandQueue::writeInt64(i64 param)
{
	if (!checkSpace(sizeof(i64)))
		return false;

	*((i64*)location) = param;
	location += sizeof(i64);

	return true;
}

bool ThreadCommandQueue::writeBool8(bool param)
{
	if (!checkSpace(sizeof(u8)))
		return false;

	*((u8*)location) = param ? 1 : 0;
	location += sizeof(u8);

	return true;
}

bool ThreadCommandQueue::writeFloat(f32 param)
{
	if (!checkSpace(sizeof(f32)))
		return false;

	*((f32*)location) = param;
	location += sizeof(f32);

	return true;
}

bool ThreadCommandQueue::writeDouble(f64 param)
{
	if (!checkSpace(sizeof(f64)))
		return false;

	*((f64*)location) = param;
	location += sizeof(f64);

	return true;
}

bool ThreadCommandQueue::writeVoidPtr(void* param)
{
	if (!checkSpace(sizeof(void*)))
		return false;

	*((void**)location) = param;
	location += sizeof(void*);

	return true;
}

bool ThreadCommandQueue::writeData(void* param, size_t size)
{
	if (!checkSpace(size + sizeof(size)))
		return false;

	*((size_t*)location) = size;
	location += sizeof(size);
	memcpy(location, param, size);
	location += size;

	return true;
}

bool ThreadCommandQueue::writeString(const String& param)
{
	if (!checkSpace(param.length()) + 1/* the last \0 */)
		return false;

	memcpy(location, param.c_str(), param.length());
	location += param.length();
	*((i8*)location) = 0;
	++location;

	return true;
}

bool ThreadCommandQueue::writeMatrix(const Matrix& param)
{
	if (!checkSpace(sizeof(Matrix)))
		return false;

	*((Matrix*)location) = param;
	location += sizeof(Matrix);

	return true;
}

bool ThreadCommandQueue::writeColor(const Color& param)
{
	if (!checkSpace(sizeof(Color)))
		return false;

	*((Color*)location) = param;
	location += sizeof(Color);

	return true;
}

bool ThreadCommandQueue::writeVec3(const Vec3& param)
{
	if (!checkSpace(sizeof(Vec3)))
		return false;

	*((Vec3*)location) = param;
	location += sizeof(Vec3);

	return true;
}

bool ThreadCommandQueue::beginRead()
{
	location = buffer;
	readCmdCount = 0;

	return true;
}

void ThreadCommandQueue::endRead()
{}

u16 ThreadCommandQueue::readCommandId()
{
	if (location >= buffer + cmdDataSize)
		return 0;

	u16 param = *((u16*)location);
	location += sizeof(u16);

	++readCmdCount;

	return param;
}

i8 ThreadCommandQueue::readInt8()
{
	if (location >= buffer + cmdDataSize)
		return 0;

	i8 param = *((i8*)location);
	location += sizeof(i8);

	return param;
}

i16 ThreadCommandQueue::readInt16()
{
	if (location >= buffer + cmdDataSize)
		return 0;

	i16 param = *((i16*)location);
	location += sizeof(i16);

	return param;
}

i32 ThreadCommandQueue::readInt32()
{
	if (location >= buffer + cmdDataSize)
		return 0;

	i32 param = *((i32*)location);
	location += sizeof(i32);

	return param;
}

i64 ThreadCommandQueue::readInt64()
{
	if (location >= buffer + cmdDataSize)
		return 0;

	i64 param = *((i64*)location);
	location += sizeof(i64);

	return param;
}

bool ThreadCommandQueue::readBool8()
{
	if (location >= buffer + cmdDataSize)
		return false;

	bool param = 1 == *((u8*)location);
	location += sizeof(u8);

	return param;
}

f32 ThreadCommandQueue::readFloat()
{
	if (location >= buffer + cmdDataSize)
		return 0;

	f32 param = *((f32*)location);
	location += sizeof(f32);

	return param;
}

f64 ThreadCommandQueue::readDouble()
{
	if (location >= buffer + cmdDataSize)
		return 0;

	f64 param = *((f64*)location);
	location += sizeof(f64);

	return param;
}

void* ThreadCommandQueue::readVoidPtr()
{
	if (location >= buffer + cmdDataSize)
		return nullptr;

	void* param = *((void**)location);
	location += sizeof(void*);

	return param;
}

void* ThreadCommandQueue::readData(size_t& outSize)
{
	outSize = 0;

	if (location >= buffer + cmdDataSize)
		return nullptr;

	outSize = *((u32*)location);
	location += sizeof(u32);
	u8* param = location;
	location += outSize;

	return param;
}

String ThreadCommandQueue::readString()
{
	if (location >= buffer + cmdDataSize)
		return "";

	size_t nSize = strlen((char*)location);
	String param = (char*)location;
	location += nSize + 1;

	return param;
}

bool ThreadCommandQueue::readMatrix(Matrix& dest)
{
	if (location >= buffer + cmdDataSize)
		return false;

	dest = *((Matrix*)location);
	location += sizeof(Matrix);

	return true;
}

bool ThreadCommandQueue::readColor(Color& dest)
{
	if (location >= buffer + cmdDataSize)
		return false;

	dest = *((Color*)location);
	location += sizeof(Color);

	return true;
}

bool ThreadCommandQueue::readVec3(Vec3& dest)
{
	if (location >= buffer + cmdDataSize)
		return false;

	dest = *((Vec3*)location);
	location += sizeof(Vec3);

	return true;
}

void ThreadCommandQueue::freeBuffer()
{
	B_SAFE_DELETE_ARRAY(buffer);
	totalBufferSize = 0;
	cmdDataSize = 0;
	cmdCount = 0;
	location = nullptr;
}

size_t ThreadCommandQueue::getCommandDataSize() const
{
	return cmdDataSize;
}

size_t ThreadCommandQueue::getTotalBufferSize() const
{
	return totalBufferSize;
}

size_t ThreadCommandQueue::getCommandCount() const
{
	return cmdCount;
}

size_t ThreadCommandQueue::getMaxAllowedBufferSize() const
{
	return maxAllowedBufferSize;
}

void ThreadCommandQueue::setMaxAllowedBufferSize(size_t size)
{
	maxAllowedBufferSize = size;
	//TODO: cut buffer if new smaller size ?
}

void* ThreadCommandQueue::getCurrentLocation() const
{
	return location;
}

void* ThreadCommandQueue::getBuffer() const
{
	return buffer;
}

size_t ThreadCommandQueue::advanceLocation(size_t size)
{
	location += size;

	if ((size_t)(location - buffer) > totalBufferSize)
	{
		location = buffer + totalBufferSize;
	}

	return buffer + totalBufferSize - location;
}

//---

ThreadDoubleCommandQueue::ThreadDoubleCommandQueue()
{
	consumeQueue = &queues[0];
	fillQueue = &queues[1];
}

ThreadDoubleCommandQueue::~ThreadDoubleCommandQueue()
{}

void ThreadDoubleCommandQueue::switchQueues()
{
	consumeQueue->waitForLock();
	fillQueue->waitForLock();

	ThreadCommandQueue* tmp = fillQueue;
	
	fillQueue = consumeQueue;
	consumeQueue = tmp;

	consumeQueue->unlock();
	fillQueue->unlock();
}

ThreadCommandQueue* ThreadDoubleCommandQueue::beginFillQueue()
{
	fillQueue->beginWrite();

	return fillQueue;
}

void ThreadDoubleCommandQueue::endFillQueue()
{
	switchQueues();
}

ThreadCommandQueue* ThreadDoubleCommandQueue::getConsumeQueue() const
{
	return consumeQueue;
}

ThreadCommandQueue* ThreadDoubleCommandQueue::getFillQueue() const
{
	return fillQueue;
}

void ThreadDoubleCommandQueue::setQueueMaxAllowedSize(u32 sizeInBytes)
{
	queues[0].setMaxAllowedBufferSize(sizeInBytes);
	queues[1].setMaxAllowedBufferSize(sizeInBytes);
}

void ThreadDoubleCommandQueue::setReadBatchCount(u32 count)
{
	readBatchCount = count;
}

u32 ThreadDoubleCommandQueue::getReadBatchCount() const
{
	return readBatchCount;
}

}