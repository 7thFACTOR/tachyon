// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/class_registry.h"
#include "base/string.h"
#include "base/stream.h"

namespace base
{
class B_API MemoryStream : public Stream
{
public:
	MemoryStream();
	MemoryStream(u8* readData, size_t readDataSize);
	virtual ~MemoryStream();
	void setWriteGrowSize(size_t growSize) { writeDataGrowSize = growSize; }
	void openForRead(u8* data, size_t dataSize);
	void openForWrite(size_t maxDataSize);
	void close();
	u64 getSize() const override { return dataSize; }
	u64 getPosition() const override { return currentPosition; }
	bool read(void* data, u64 dataSize) override;
	bool write(const void* data, u64 dataSize) override;
	bool seek(SeekMode mode, u64 offset) override;
	bool isEndOfStream() const override;
	u8* getData() const { return data; }
	size_t getDataSize() const { return dataSize; }

protected:
	bool forRead = true;
	u8* data = nullptr;
	size_t dataSize = 0;
	size_t totalDataSize = 0;
	size_t currentPosition = 0;
	size_t writeDataGrowSize = 1024;
	size_t maxWriteDataSize = ~0;
};

}