#include "render/graphics2d.h"

namespace engine
{
void Graphics2D::beginBatch(TextureAtlasResource* atlas)
{

}

void Graphics2D::endBatch()
{

}

void Graphics2D::drawQuad(TextureAtlasItem* atlasItem, const Rect& rect, const RectF& uvRect, f32 depth)
{

}

void Graphics2D::drawText(const Vec2&, const String& text, ...)
{

}

void Graphics2D::drawTextBox(f32 x, f32 y, f32 width, f32 height, const String& text, ...)
{

}

void Graphics2D::drawTextBox(const Rect& rect, const String& text, ...)
{

}

void Graphics2D::createTextGeometryBuffer()
{

}

void Graphics2D::drawTextInternalAt(f32 x, f32 y, const String& text)
{

}

void Graphics2D::generateTextGeometry(
	const DrawTextSettings& settings,
	f32 x, f32 y,
	const String& text,
	u32& outVertexCount,
	u32& outIndexCount)
{

}

void Graphics2D::cacheBatch()
{

}


}