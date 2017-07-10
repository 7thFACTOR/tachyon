#pragma once


namespace engine
{
using namespace base;
struct TextureAtlasItem;

struct ResizableImage
{
	TextureAtlasItem* item = nullptr;
	Rect border;
};

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

}