#pragma once
#include "base/defines.h"
#include "core/defines.h"
#include "graphics/types.h"
#include "core/resources/texture_resource.h"

namespace engine
{
class Texture;
typedef u32 RenderFrameLayerPriority;


class E_API RenderFrameLayer
{
public:
	virtual void render(RenderFrameCompositor* compositor) = 0;

	RenderFrameLayerPriority priority = 0;
};

class E_API RenderFrameCompositor
{
public:
	Array<RenderFrameLayer*> layers;
	Dictionary<String, Texture*> renderTargets;
};

}