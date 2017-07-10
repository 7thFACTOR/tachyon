#pragma once
#include "base/array.h"
#include "base/defines.h"
#include "base/types.h"
#include "game/component.h"
#include "core/types.h"

namespace engine
{
using namespace base;
class VertexBuffer;
class ModelData;
class RenderPacket;

//! A model instanter object will render instances of a model resource
struct ModelInstancerComponent : Component
{
	enum
	{
		Type = ComponentType::ModelInstancer,
		DefaultCount = kMaxComponentCountPerType
	};

	//! the instance buffers
	Array<VertexBuffer*> instanceBuffers;
	//! the model data resource
	ResourceId model;
	u32 instanceAttributes;
	u32 levelOfDetail;
	Array<RenderPacket*> renderPackets;
};

}