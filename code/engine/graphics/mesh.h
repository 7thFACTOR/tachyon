#pragma once
#include "core/defines.h"
#include "base/types.h"
#include "graphics/types.h"

namespace engine
{
class GpuBuffer;

class E_API Mesh
{
public:
	Mesh()
		: vertexBuffer(nullptr)
		, indexBuffer(nullptr)
		, resourceId(nullResourceId)
	{}

	~Mesh() { freeBuffers(); }
	inline GpuBuffer* getVertexBuffer() const { return vertexBuffer; }
	inline GpuBuffer* getIndexBuffer() const { return indexBuffer; }
	inline ResourceId getResourceId() const { return resourceId; }
	void createBuffers();
	void freeBuffers();
	void setFromResource(ResourceId resId);

protected:
	GpuBuffer* vertexBuffer = nullptr;
	GpuBuffer* indexBuffer = nullptr;
	ResourceId resourceId = nullResourceId;
};

}