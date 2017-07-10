#pragma once
#include "base/string.h"
#include "base/array.h"
#include "base/dictionary.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/observer.h"
#include "base/math/ray.h"
#include "core/defines.h"
#include "logic/component.h"
#include "logic/component_updater.h"
#include "logic/component_updaters/ui/font_cache.h"

namespace engine
{
using namespace base;

class UiComponentUpdater : public ComponentUpdater
{
public:
	UiComponentUpdater();
	virtual ~UiComponentUpdater();
	ComponentUpdaterId getId() const override { return StdComponentUpdaterId_Ui; }
	void update(f32 deltaTime) override;

	// void drawQuad(const Rect& uvRect, const Rect& rect, const RectI pixelSize, const Color& color, AtlasItemSizingPolicy sizePolicy);
	// void drawQuadUvRotated(const Rect& uvRect, const Rect& rect, const RectI pixelSize, const Color& color, AtlasItemSizingPolicy sizePolicy);
	// void drawTextInternal(
		// Font* font,
		// const String& text,
		// const FontTextSize& textExtent,
		// const Vec2& pos,
		// bool useBaseline);

	// const u32 maxVertexCount = 50000;

	// void setProjection(CanvasProjectionMode projMode, CanvasAspectType aspectType);
	// void setCustomProjection(f32 width, f32 height, f32 aspect);
	// void pushClipRect(const Rect& rect);
	// void popClipRect(Rect* outRect = nullptr);
	// void setViewport(const Rect& rect);
	// Rect getViewport() const { return viewport; }
	// void setGpuProgram(ResourceId program);
	// void beginBatch(ResourceId atlas);
	// void endBatch();
	// void drawBatches();
	// void clearBatches();
	// void draw(
		// TextureAtlasItem* item,
		// const Rect& screenRect,
		// const Color& color,
		// AtlasItemSizingPolicy sizePolicy);
	// void draw(
		// TextureAtlasItem* item,
		// const Vec2& screenPos,
		// const Color& color,
		// AtlasItemSizingPolicy sizePolicy);
	// void draw(
		// const NineCellBox& box,
		// const Rect& screenRect,
		// const Color& color);
	// void drawTexture(const Vec2& screenPos, ResourceId texture);

	// // these functions will start batches by their own with the font's atlas
	// void drawText(
		// Font* font,
		// u8* utf8Text,
		// const Vec2& position,
		// const Color& color);

	// void drawText(
		// Font* font,
		// u8* utf8Text,
		// const Rect& rect,
		// const Color& color,
		// HorizontalTextAlignment horizontal,
		// VerticalTextAlignment vertical);

	// void drawText(
		// Font* font,
		// const String& text,
		// const Vec2& position,
		// const Color& color,
		// HorizontalTextAlignment horizontal = HorizontalTextAlignment::Right,
		// VerticalTextAlignment vertical = VerticalTextAlignment::Bottom,
		// bool useBaseline = false);
	// void drawText(
		// Font* font,
		// const String& text,
		// const Rect& rect,
		// const Color& color,
		// HorizontalTextAlignment horizontal = HorizontalTextAlignment::Left,
		// VerticalTextAlignment vertical = VerticalTextAlignment::Top,
		// bool useBaseline = false);
protected:
	FontCache fontCache;

};

}