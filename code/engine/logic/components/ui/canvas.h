// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "core/defines.h"
#include "core/types.h"
#include "base/math/color.h"
#include "base/math/rect.h"
#include "render/gpu_buffer.h"
#include "logic/component.h"
#include "core/resource_ref.h"
#include "render/resources/texture_atlas_resource.h"
#include "logic/components/ui/types.h"

namespace engine
{
enum class CanvasScalingMode
{
	PixelSize,
	ScreenSize,
	Custom
};

enum class CanvasRenderMode
{
	ScreenOverlay,
	ScreenCamera,
	WorldSpace
};

struct CanvasBatch
{
	ResourceId atlas;
	Color color;
	GpuBufferRange vbRange;
};

//TODO: add 3D transform for widgets and drawing?
struct CanvasComponent : Component
{
	static const ComponentTypeId typeId = (ComponentTypeId)StdComponentTypeId::Canvas;

	CanvasRenderMode renderMode;
	CanvasScalingMode scalingMode;
	Dictionary<ResourceRef<TextureAtlasResource>, CanvasBatch> batches;
	CanvasBatch currentBatch;
	Array<Rect> clipRectStack;
	Rect clipRect;
	Rect viewport;
	u32 vertexCount = 0;
	f32 vertexCountGrowFactor = 1.5f;
	CanvasVertex* vertices = nullptr;
	u32 currentVertexIndex = 0;
	GpuBuffer* vb = nullptr;
	ResourceId uiGpuProgram = nullResourceId;
};

}