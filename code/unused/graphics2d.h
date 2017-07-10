#pragma once
#include "base/types.h"
#include "base/math/vec2.h"
#include "base/math/rect.h"
#include "base/math/color.h"
#include "render/types.h"
#include "render/mesh.h"

namespace engine
{
using namespace base;

struct TextureAtlasResource;
struct TextureAtlasItem;

class ENGINE_API Graphics2D
{
public:
	void beginBatch(TextureAtlasResource* atlas);
	void drawQuad(TextureAtlasItem* atlasItem, const Rect& rect);
	void endBatch();

protected:
	void cacheBatch();

	ResourceId gpuProgram;
	String gpuProgramName;
	Mesh textGeometry;
	DrawTextSettings textSettings;
	Vec2 currentTextCaretPosition;
	u8* vertices;
	u8* indices;
	u32 maxVertexCount;
	u32 indexCount;
	u32 currentVertexCount;
	u32 startVertexIndex;
};

}