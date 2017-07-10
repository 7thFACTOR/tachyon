#include <string.h>
#include "gui/canvas.h"
#include "render/resources/texture_atlas_resource.h"
#include "gui/font.h"
#include "render/graphics.h"
#include "core/globals.h"
#include "core/resource_repository.h"
#include "gui/theme.h"
#include "base/misc/cached_font.h"

namespace engine
{
using namespace base;

Canvas::Canvas()
{
	vertexCount = maxVertexCount;
	vertices = new CanvasVertex[vertexCount];
	vb = getGraphics().createGpuBuffer();
}

Canvas::~Canvas()
{}

void Canvas::pushClipRect(const Rect& rect)
{
	clipRectStack.append(clipRect);
	clipRect = rect;
	getGraphics().getRasterizerState().scissorTest = true;
	getGraphics().commitRasterizerState();
	getGraphics().setScissor(rect);
}

void Canvas::popClipRect(Rect* outRect)
{
	if (clipRectStack.popLast(clipRect))
	{
		if (outRect)
		{
			*outRect = clipRect;
		}

		if (clipRectStack.isEmpty())
		{
			getGraphics().getRasterizerState().scissorTest = false;
			getGraphics().commitRasterizerState();
		}

		getGraphics().setScissor(clipRect);
	}
}

void Canvas::setViewport(const Rect& rect)
{
	viewport = rect;
	getGraphics().setViewport(rect);
}

void Canvas::setGpuProgram(ResourceId program)
{
	guiGpuProgram = program;
	getResources().load(program);
}

void Canvas::beginBatch(ResourceId atlas)
{
	currentBatch.atlas = atlas;
	currentBatch.vbRange.baseVertexIndex = currentVertexIndex;
	currentBatch.vbRange.indexOffset = currentVertexIndex;
	currentBatch.vbRange.indexCount = currentBatch.vbRange.vertexCount = 0;
}

void Canvas::endBatch()
{
	currentBatch.vbRange.indexCount = currentVertexIndex - currentBatch.vbRange.indexOffset;

	batches.add(currentBatch.atlas, currentBatch);
}

void Canvas::drawBatches()
{
	for (auto batch : batches)
	{
		batch.value.atlas

		getGraphics().

	}
}

void Canvas::clearBatches()
{
	batches.clear();
}

void Canvas::draw(
	TextureAtlasItem* item,
	const Rect& screenRect,
	const Color& color,
	AtlasItemSizingPolicy sizePolicy)
{
	if (item->rotated)
		drawQuadUvRotated(item->rect, screenRect, item->rectI, color, sizePolicy);
	else
		drawQuad(item->rect, screenRect, item->rectI, color, sizePolicy);
}

void Canvas::draw(
	TextureAtlasItem* item,
	const Vec2& screenPos,
	const Color& color,
	AtlasItemSizingPolicy sizePolicy)
{
	if (item->rotated)
		drawQuadUvRotated(
		item->rect,
		Rect(
		screenPos.x,
		screenPos.y,
		item->rectI.width,
		item->rectI.height),
		item->rectI,
		color, sizePolicy);
	else
		drawQuad(
		item->rect,
		Rect(
		screenPos.x,
		screenPos.y,
		item->rectI.width,
		item->rectI.height),
		item->rectI,
		color, sizePolicy);
}

void Canvas::draw(const NineCellBox& box, const Rect& screenRect, const Color& color)
{
	// top row
	draw(
		box.topLeft,
		Rect(
		screenRect.x, screenRect.y,
		box.topLeft->rectI.width, box.topLeft->rectI.height),
		color, AtlasItemSizingPolicy::Clamp);
	draw(
		box.topMiddle,
		Rect(
		screenRect.x + box.topLeft->rectI.width,
		screenRect.y,
		screenRect.width - box.topLeft->rectI.width - box.topRight->rectI.width,
		box.topMiddle->rectI.height), color, AtlasItemSizingPolicy::Stretch);
	draw(
		box.topRight,
		Rect(
		screenRect.right() - box.topRight->rectI.width,
		screenRect.y,
		box.topRight->rectI.width,
		box.topRight->rectI.height), color, AtlasItemSizingPolicy::Clamp);

	// middle row
	draw(
		box.middleLeft,
		Rect(
		screenRect.x,
		screenRect.y + box.topLeft->rectI.height,
		box.middleLeft->rectI.width,
		screenRect.height - box.topLeft->rectI.height - box.bottomLeft->rectI.height),
		color, AtlasItemSizingPolicy::Stretch);

	draw(
		box.middleCenter,
		Rect(
		screenRect.x + box.topLeft->rectI.width,
		screenRect.y + box.topLeft->rectI.height,
		screenRect.width - box.middleLeft->rectI.width - box.middleRight->rectI.width,
		screenRect.height - box.topMiddle->rectI.height - box.bottomMiddle->rectI.height), color, AtlasItemSizingPolicy::Stretch);

	draw(
		box.middleRight,
		Rect(
		screenRect.right() - box.middleRight->rectI.width,
		screenRect.y + box.topRight->rectI.height,
		box.middleRight->rectI.width,
		screenRect.height - box.topRight->rectI.height - box.bottomRight->rectI.height),
		color, AtlasItemSizingPolicy::Clamp);

	// bottom row
	draw(
		box.bottomLeft,
		Rect(
		screenRect.x,
		screenRect.bottom() - box.bottomLeft->rectI.height,
		box.bottomLeft->rectI.width, box.bottomLeft->rectI.height),
		color, AtlasItemSizingPolicy::Clamp);
	draw(
		box.bottomMiddle,
		Rect(
		screenRect.x + box.bottomLeft->rectI.width,
		screenRect.bottom() - box.bottomMiddle->rectI.height,
		screenRect.width - box.bottomLeft->rectI.width - box.bottomRight->rectI.width,
		box.bottomMiddle->rectI.height), color, AtlasItemSizingPolicy::Stretch);
	draw(
		box.bottomRight,
		Rect(
		screenRect.right() - box.bottomRight->rectI.width,
		screenRect.bottom() - box.bottomRight->rectI.height,
		box.bottomRight->rectI.width,
		box.bottomRight->rectI.height), color, AtlasItemSizingPolicy::Clamp);
}

Vec2 computeTiling(const Rect& uvRect, const RectI& pixelSize, const Rect& rect)
{
	return Vec2(
		rect.width / (f32)pixelSize.width,
		rect.height / (f32)pixelSize.height);
}

void Canvas::drawQuad(
	const Rect& uvRect,
	const Rect& rect,
	const RectI pixelSize,
	const Color& color,
	AtlasItemSizingPolicy sizePolicy)
{
	u32 i = currentVertexIndex;
	Vec2 tiling = computeTiling(uvRect, pixelSize, rect);

	vertices[i].x = rect.x;
	vertices[i].y = rect.y;
	vertices[i].u = 0;
	vertices[i].v = 0;
	vertices[i].uItemOffset = uvRect.x;
	vertices[i].vItemOffset = uvRect.y;
	vertices[i].uItemSize = uvRect.width;
	vertices[i].vItemSize = uvRect.height;
	vertices[i].color = color;
	i++;

	vertices[i].x = rect.right();
	vertices[i].y = rect.y;
	vertices[i].u = tiling.x;
	vertices[i].v = 0;
	vertices[i].uItemOffset = uvRect.x;
	vertices[i].vItemOffset = uvRect.y;
	vertices[i].uItemSize = uvRect.width;
	vertices[i].vItemSize = uvRect.height;
	vertices[i].color = color;
	i++;

	vertices[i].x = rect.x;
	vertices[i].y = rect.bottom();
	vertices[i].u = 0;
	vertices[i].v = tiling.y;
	vertices[i].uItemOffset = uvRect.x;
	vertices[i].vItemOffset = uvRect.y;
	vertices[i].uItemSize = uvRect.width;
	vertices[i].vItemSize = uvRect.height;
	vertices[i].color = color;
	i++;

	// 2nd tri

	vertices[i].x = rect.right();
	vertices[i].y = rect.y;
	vertices[i].u = tiling.x;
	vertices[i].v = 0;
	vertices[i].uItemOffset = uvRect.x;
	vertices[i].vItemOffset = uvRect.y;
	vertices[i].uItemSize = uvRect.width;
	vertices[i].vItemSize = uvRect.height;
	vertices[i].color = color;
	i++;

	vertices[i].x = rect.right();
	vertices[i].y = rect.bottom();
	vertices[i].u = tiling.x;
	vertices[i].v = tiling.y;
	vertices[i].uItemOffset = uvRect.x;
	vertices[i].vItemOffset = uvRect.y;
	vertices[i].uItemSize = uvRect.width;
	vertices[i].vItemSize = uvRect.height;
	vertices[i].color = color;
	i++;

	vertices[i].x = rect.x;
	vertices[i].y = rect.bottom();
	vertices[i].u = 0;
	vertices[i].v = tiling.y;
	vertices[i].uItemOffset = uvRect.x;
	vertices[i].vItemOffset = uvRect.y;
	vertices[i].uItemSize = uvRect.width;
	vertices[i].vItemSize = uvRect.height;
	vertices[i].color = color;
	i++;

	currentVertexIndex = i;

	//TODO: resize if small buffer
}

void Canvas::drawQuadUvRotated(
	const Rect& uvRect,
	const Rect& rect,
	const RectI pixelSize,
	const Color& color,
	AtlasItemSizingPolicy sizePolicy)
{
	u32 i = currentVertexIndex;
	Vec2 tiling = computeTiling(uvRect, pixelSize, rect);

	Vec2 t0(0, 0);
	Vec2 t1(1, 0);
	Vec2 t2(1, 1);
	Vec2 t3(0, 1);

	vertices[i].x = rect.x;
	vertices[i].y = rect.y;
	vertices[i].u = t3.x;
	vertices[i].v = t3.y;
	vertices[i].uItemOffset = uvRect.x;
	vertices[i].vItemOffset = uvRect.y;
	vertices[i].uItemSize = uvRect.width;
	vertices[i].vItemSize = uvRect.height;
	vertices[i].color = color;
	i++;

	vertices[i].x = rect.right();
	vertices[i].y = rect.y;
	vertices[i].u = t0.x;
	vertices[i].v = t0.y;
	vertices[i].uItemOffset = uvRect.x;
	vertices[i].vItemOffset = uvRect.y;
	vertices[i].uItemSize = uvRect.width;
	vertices[i].vItemSize = uvRect.height;
	vertices[i].color = color;
	i++;

	vertices[i].x = rect.x;
	vertices[i].y = rect.bottom();
	vertices[i].u = t2.x;
	vertices[i].v = t2.y;
	vertices[i].uItemOffset = uvRect.x;
	vertices[i].vItemOffset = uvRect.y;
	vertices[i].uItemSize = uvRect.width;
	vertices[i].vItemSize = uvRect.height;
	vertices[i].color = color;
	i++;

	// 2nd tri

	vertices[i].x = rect.right();
	vertices[i].y = rect.y;
	vertices[i].u = t0.x;
	vertices[i].v = t0.y;
	vertices[i].uItemOffset = uvRect.x;
	vertices[i].vItemOffset = uvRect.y;
	vertices[i].uItemSize = uvRect.width;
	vertices[i].vItemSize = uvRect.height;
	vertices[i].color = color;
	i++;

	vertices[i].x = rect.right();
	vertices[i].y = rect.bottom();
	vertices[i].u = t1.x;
	vertices[i].v = t1.y;
	vertices[i].uItemOffset = uvRect.x;
	vertices[i].vItemOffset = uvRect.y;
	vertices[i].uItemSize = uvRect.width;
	vertices[i].vItemSize = uvRect.height;
	vertices[i].color = color;
	i++;

	vertices[i].x = rect.x;
	vertices[i].y = rect.bottom();
	vertices[i].u = t2.x;
	vertices[i].v = t2.y;
	vertices[i].uItemOffset = uvRect.x;
	vertices[i].vItemOffset = uvRect.y;
	vertices[i].uItemSize = uvRect.width;
	vertices[i].vItemSize = uvRect.height;
	vertices[i].color = color;
	i++;

	currentVertexIndex = i;

	//TODO: resize if small buffer
}

void Canvas::drawTexture(const Vec2& screenPos, ResourceId texture)
{
	//TODO
}

void Canvas::drawText(
	Font* font,
	u8* utf8Text,
	const Vec2& position,
	const Color& color)
{
	//TODO
}

void Canvas::drawText(
	Font* font,
	u8* utf8Text,
	const Rect& rect,
	const Color& color,
	HorizontalTextAlignment horizontal,
	VerticalTextAlignment vertical)
{
	//TODO
}

void Canvas::drawText(
	Font* font,
	const String& text,
	const Vec2& position,
	const Color& color,
	HorizontalTextAlignment horizontal,
	VerticalTextAlignment vertical,
	bool useBaseline)
{
	if (!font)
	{
		return;
	}

	FontTextSize textExtent = font->getCachedFont().computeTextSize(text);
	Vec2 pos = position;

	switch (vertical)
	{
	case VerticalTextAlignment::Top:
		pos.y = position.y - textExtent.height - font->getCachedFont().getTextSizeForAllGlyphs().maxDescent;
		break;
	case VerticalTextAlignment::Center:
		pos.y = position.y - textExtent.height / 2.0f;
		break;
	case VerticalTextAlignment::Bottom:
		pos.y = position.y;
		break;
	default:
		break;
	}

	switch (horizontal)
	{
	case HorizontalTextAlignment::Left:
		pos.x = position.x - textExtent.width;
		break;
	case HorizontalTextAlignment::Center:
		pos.x = position.x - textExtent.width / 2.0f;
		break;
	case HorizontalTextAlignment::Right:
		pos.x = position.x;
		break;
	default:
		break;
	}

	drawTextInternal(font, text, textExtent, pos, color, useBaseline);
}

void Canvas::drawText(
	Font* font,
	const String& text,
	const Rect& rect,
	const Color& color,
	HorizontalTextAlignment horizontal,
	VerticalTextAlignment vertical,
	bool useBaseline)
{
	B_ASSERT(font);

	if (!font)
	{
		return;
	}

	FontTextSize textExtent = font->computeTextSize(text);
	Vec2 pos;

	switch (vertical)
	{
	case VerticalTextAlignment::Top:
		pos.y = rect.y;
		break;
	case VerticalTextAlignment::Center:
		pos.y = rect.y + rect.height / 2.0f - textExtent.height / 2.0f;
		break;
	case VerticalTextAlignment::Bottom:
		pos.y = rect.bottom() - textExtent.height;
		break;
	default:
		break;
	}

	switch (horizontal)
	{
	case HorizontalTextAlignment::Left:
		pos.x = rect.x;
		break;
	case HorizontalTextAlignment::Center:
		pos.x = rect.x + (rect.width - textExtent.width) / 2.0f;
		break;
	case HorizontalTextAlignment::Right:
		pos.x = rect.right() - textExtent.width;
		break;
	default:
		break;
	}

	drawTextInternal(font, text, textExtent, pos, color, useBaseline);
}

void Canvas::drawTextInternal(
	Font* font,
	const String& text,
	const FontTextSize& textExtent,
	const Vec2& atPos,
	const Color& color,
	bool useBaseline)
{
	B_ASSERT(font);
	
	if (!font)
	{
		return;
	}

	Vec2 pos = atPos;
	CachedFont& cachedFont = font->getCachedFont();
	beginBatch(font->getAtlas());
	u32 lastChr = 0;

	for (auto chr : text)
	{
		auto glyph = font->getGlyph((u8)chr);
		auto atlasItem = font->getGlyphAtlasItem((u8)chr);

		if (chr == '\n')
		{
			pos.x = atPos.x;
			pos.y += textExtent.height;
			continue;
		}

		if (!glyph || !atlasItem)
		{
			continue;
		}

		Vec2 chrPos(pos.x + glyph->bearingX, pos.y - glyph->advanceY + textExtent.height - ((useBaseline) ? 0 : textExtent.maxDescent));
		draw(atlasItem, chrPos, color, AtlasItemSizingPolicy::Stretch);
		auto kern = font->getKerning(lastChr, chr);
		pos.x += glyph->advanceX + kern;
		lastChr = chr;
	}

	endBatch();
}


}