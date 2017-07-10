#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/assert.h"
#include "graphics/types.h"
#include "graphics/gpu_buffer_descriptor.h"
#include <string.h>

namespace engine
{
class E_API GpuBuffer
{
public:
	GpuBuffer() {}
	virtual ~GpuBuffer() {}
	virtual void initialize(u32 dataSize = 0, u8* data = nullptr) = 0;
	virtual u8* map(u32 offset, u32 size, GpuBufferMapType mapType) = 0;
	virtual void unmap() = 0;
	virtual void bind() = 0;
	virtual void unbind() = 0;
	virtual void resize(u32 newDataSize, u8* newData = nullptr) = 0;

	inline u8* mapFull(GpuBufferMapType mapType = GpuBufferMapType::Write)
	{
		u32 elemSize = desc.getStride();
		return map(0, elemSize * elementCount, mapType);
	}

	inline u8* mapRange(u32 firstElementIndex, u32 count, GpuBufferMapType mapType = GpuBufferMapType::Write)
	{
		u32 elemSize = desc.getStride();
		return map(elemSize * firstElementIndex, elemSize * count, mapType);
	}

	void write(u8* data, u32 dataSize)
	{
		B_ASSERT(data);
		B_ASSERT(dataSize <= desc.getStride() * elementCount);

		u8* writeMappedData = mapFull(GpuBufferMapType::Write);

		B_ASSERT(writeMappedData);
		memcpy(writeMappedData, data, dataSize);
		unmap();
	}

	inline void setupElements(const GpuBufferDescriptor& newDesc, u32 newElementCount)
	{
		desc = newDesc;
		elementCount = newElementCount;
	}

	inline u64 getTotalSize() const { return (u64)elementCount * desc.getStride(); }

public:
	GpuBufferType type = GpuBufferType::Vertices;
	GpuBufferDescriptor desc;
	GpuBufferUsageType usage = GpuBufferUsageType::Static;
	u32 elementCount = 0;
	RenderPrimitive primitiveType = RenderPrimitive::Triangles;
	bool instancing = false;

protected:
	bool mapped = false;
	u8* mappedData = nullptr;
	u32 mappedOffset = 0;
	u32 mappedDataSize = 0;
	u32 bufferSize = 0;
};

}