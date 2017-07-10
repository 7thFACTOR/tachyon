#pragma once
#include <core/defines.h>
#include <core/types.h>
#include <base/math/color.h>
#include <base/math/rect.h>
#include <render/gpu_buffer.h>

namespace engine
{
class Font;
struct FontTextSize;

enum class AtlasItemSizingPolicy
{
	Stretch,
	Repeat,
	Clamp
};

enum class HorizontalTextAlignment
{
	Left,
	Center,
	Right
};

enum class VerticalTextAlignment
{
	Top,
	Center,
	Bottom
};

enum class CanvasProjectionMode
{
	PixelSpace,
	UnitSpace,
	Custom
};
enum class CanvasAspectType
{
	Free,
	Scale14to9,
	Scale4to3,
	Custom
};

struct NineCellBox
{
	TextureAtlasItem* topLeft = nullptr;
	TextureAtlasItem* topMiddle = nullptr;
	TextureAtlasItem* topRight = nullptr;
	TextureAtlasItem* middleLeft = nullptr;
	TextureAtlasItem* middleCenter = nullptr;
	TextureAtlasItem* middleRight = nullptr;
	TextureAtlasItem* bottomLeft = nullptr;
	TextureAtlasItem* bottomMiddle = nullptr;
	TextureAtlasItem* bottomRight = nullptr;
};

struct CanvasBatch
{
	ResourceId atlas;
	Color color;
	GpuBufferRange vbRange;
};

//TODO: add 3D transform for widgets and drawing?
class ENGINE_API Canvas
{
public:
	Canvas();
	virtual ~Canvas();
	void setProjection(CanvasProjectionMode projMode, CanvasAspectType aspectType);
	void setCustomProjection(f32 width, f32 height, f32 aspect);
	void pushClipRect(const Rect& rect);
	void popClipRect(Rect* outRect = nullptr);
	void setViewport(const Rect& rect);
	Rect getViewport() const { return viewport; }
	void setGpuProgram(ResourceId program);
	void beginBatch(ResourceId atlas);
	void endBatch();
	void drawBatches();
	void clearBatches();
	void draw(
		TextureAtlasItem* item,
		const Rect& screenRect,
		const Color& color,
		AtlasItemSizingPolicy sizePolicy);
	void draw(
		TextureAtlasItem* item,
		const Vec2& screenPos,
		const Color& color,
		AtlasItemSizingPolicy sizePolicy);
	void draw(
		const NineCellBox& box,
		const Rect& screenRect,
		const Color& color);
	void drawTexture(const Vec2& screenPos, ResourceId texture);

	// these functions will start batches by their own with the font's atlas
	void drawText(
		Font* font,
		u8* utf8Text,
		const Vec2& position,
		const Color& color);

	void drawText(
		Font* font,
		u8* utf8Text,
		const Rect& rect,
		const Color& color,
		HorizontalTextAlignment horizontal,
		VerticalTextAlignment vertical);

	void drawText(
		Font* font,
		const String& text,
		const Vec2& position,
		const Color& color,
		HorizontalTextAlignment horizontal = HorizontalTextAlignment::Right,
		VerticalTextAlignment vertical = VerticalTextAlignment::Bottom,
		bool useBaseline = false);
	void drawText(
		Font* font,
		const String& text,
		const Rect& rect,
		const Color& color,
		HorizontalTextAlignment horizontal = HorizontalTextAlignment::Left,
		VerticalTextAlignment vertical = VerticalTextAlignment::Top,
		bool useBaseline = false);

protected:
	struct CanvasVertex
	{
		f32 x = 0.0f;
		f32 y = 0.0f;
		f32 z = 0.0f;
		f32 u = 0.0f;
		f32 v = 0.0f;
		f32 uItemOffset = 0.0f;
		f32 vItemOffset = 0.0f;
		f32 uItemSize = 0.0f;
		f32 vItemSize = 0.0f;
		Color color;
	};

	void drawQuad(const Rect& uvRect, const Rect& rect, const RectI pixelSize, const Color& color, AtlasItemSizingPolicy sizePolicy);
	void drawQuadUvRotated(const Rect& uvRect, const Rect& rect, const RectI pixelSize, const Color& color, AtlasItemSizingPolicy sizePolicy);
	void drawTextInternal(
		Font* font,
		const String& text,
		const FontTextSize& textExtent,
		const Vec2& pos,
		bool useBaseline);

	const u32 maxVertexCount = 50000;

	Map<ResourceId, CanvasBatch> batches;
	CanvasBatch currentBatch;
	Array<Rect> clipRectStack;
	Rect clipRect;
	Rect viewport;
	u32 vertexCount = 0;
	f32 vertexCountGrowFactor = 1.5f;
	CanvasVertex* vertices = nullptr;
	u32 currentVertexIndex = 0;
	GpuBuffer* vb = nullptr;
	ResourceId guiGpuProgram = invalidResourceId;
};

}