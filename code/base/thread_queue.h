// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/dictionary.h"
#include "base/string.h"
#include "base/platform.h"
#include "base/mutex.h"
#include "base/math/vec3.h"
#include "base/math/matrix.h"

namespace base
{
class Color;

//! This class is used to exchange computed data and messages between threads without frequent locking
class B_API ThreadCommandQueue
{
public:
	ThreadCommandQueue();
	virtual ~ThreadCommandQueue();

	bool lock();
	void unlock();
	bool waitForLock();
	bool beginWrite();
	void endWrite();
	bool writeCommandId(u16 id = 0);
	bool writeInt8(i8 param);
	bool writeInt16(i16 param);
	bool writeInt32(i32 param);
	bool writeInt64(i64 param);
	bool writeBool8(bool param);
	bool writeFloat(f32 param);
	bool writeDouble(f64 param);
	bool writeVoidPtr(void* param);
	bool writeData(void* param, size_t size);
	bool writeString(const String& param);
	bool writeMatrix(const Matrix& param);
	bool writeColor(const Color& param);
	bool writeVec3(const Vec3& param);

	bool beginRead();
	void endRead();
	u16 readCommandId();
	i8 readInt8();
	i16 readInt16();
	i32 readInt32();
	i64 readInt64();
	bool readBool8();
	f32 readFloat();
	f64 readDouble();
	void* readVoidPtr();
	void* readData(size_t& size);
	String readString();
	bool readMatrix(Matrix& dest);
	bool readColor(Color& dest);
	bool readVec3(Vec3& dest);
	void* getCurrentLocation() const;
	void* getBuffer() const;
	size_t advanceLocation(size_t size);
	void freeBuffer();
	size_t getCommandDataSize() const;
	size_t getTotalBufferSize() const;
	size_t getCommandCount() const;
	size_t getMaxAllowedBufferSize() const;
	void setMaxAllowedBufferSize(size_t size);

protected:
	bool checkSpace(size_t size);

	size_t maxAllowedBufferSize;
	u8* buffer;
	u8* location;
	size_t totalBufferSize, readCmdCount;
	size_t cmdDataSize, cmdCount;
	Mutex queueMutex;
};

//! This class is used to switch between two buffers used for fast command/message
//! writing and reading from worker threads
class B_API ThreadDoubleCommandQueue
{
public:
	ThreadDoubleCommandQueue();
	virtual ~ThreadDoubleCommandQueue();

	ThreadCommandQueue* beginFillQueue();
	void endFillQueue();
	ThreadCommandQueue* getFillQueue() const;
	ThreadCommandQueue* getConsumeQueue() const;
	u32 getReadBatchCount() const;
	void setQueueMaxAllowedSize(u32 sizeInBytes);
	void setReadBatchCount(u32 count);

protected:
	void switchQueues();

	ThreadCommandQueue* consumeQueue = nullptr;
	ThreadCommandQueue* fillQueue = nullptr;
	ThreadCommandQueue queues[2];
	//! how many messages a thread must read at once in a frame
	u32 readBatchCount = 0;
};

}