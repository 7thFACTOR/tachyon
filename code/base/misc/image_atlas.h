// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/math/rect.h"
#include "base/math/color.h"
#include "base/string.h"
#include "base/array.h"
#include "base/dictionary.h"

namespace base
{
enum class ImageAtlasPackPolicy
{
	Guillotine,
	MaxRects,
	ShelfBin,
	Skyline
};

struct ImageAtlasItem
{
	String name;
	u32* imageData = nullptr;
	u32 width = 0;
	u32 height = 0;
	RectI packedRect;
	RectF texCoords;
	bool wasRotated = false;
};

//! 32bit image atlas
class B_API ImageAtlas
{
public:
	ImageAtlas() {}
	ImageAtlas(u32 width, u32 height);

	void addItem(const String& name, u32* imageData, u32 width, u32 height);
	void addWhiteItem(u32 size = 8);
	bool pack(
		u32 itemBorderSize = 2,
		const Color& fillColor = Color(0.0f, 0.0f, 0.0f, 0.0f),
		ImageAtlasPackPolicy packing = ImageAtlasPackPolicy::Guillotine);
	void clearItems();
	void clearPixels();
	inline u32* getAtlasImageBuffer() const { return pixels.data(); }
	inline u32 getAtlasImageBufferSize() const { return pixels.size() * 4; }
	inline u32 getWidth() const { return atlasWidth; }
	inline u32 getHeight() const { return atlasHeight; }
	ImageAtlasItem* getItem(const String& name);
	void resize(u32 width, u32 height);
	inline const Dictionary<String, ImageAtlasItem*>& getItems() { return items; }
protected:
	Array<u32> pixels;
	u32 atlasWidth, atlasHeight;
	Dictionary<String, ImageAtlasItem*> items;
};

}