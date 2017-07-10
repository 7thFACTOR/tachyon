#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "core/types.h"
#include "core/defines.h"
#include "base/math/color.h"
#include "render/types.h"
#include "render/geometry_buffer.h"

namespace engine
{
using namespace base;
struct TextureAtlasItem;

class ENGINE_API SpriteBatch
{
public:
	SpriteBatch();
	~SpriteBatch();

	void begin();
	void end();
	void setColor(const Color& color);
	void draw(
		ResourceId texture,
		i32 regionX, i32 regionY, i32 regionW, i32 regionH,
		i32 screenX, i32 screenY, i32 screenW = 0, i32 screenH = 0);
	void draw(
		ResourceId atlas,
		TextureAtlasItem* sprite,
		i32 screenX, i32 screenY, i32 screenW = 0, i32 screenH = 0);
	void setGpuProgram(ResourceId program);
	void setBlending(BlendMode mode);

protected:
	Geometry m_buffer;
	BlendMode m_blending;
	ResourceId m_currentTexture;
	ResourceId m_gpuProgram;
};

}