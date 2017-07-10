#pragma once
#include "core/resource.h"
#include "base/math/rect.h"

namespace engine
{
using namespace base;

struct TextureAtlasItem
{
	String name;
	bool rotated = false;
	RectF rect; // 0..1f range, uv coords
	RectI rectI; // coords in pixels
};

struct TextureAtlasResource : Resource
{
	ResourceId textureId = nullResourceId;
	Array<TextureAtlasItem> items;

	//TODO: optimize search
	TextureAtlasItem* findItem(const String& name)
	{
		for (auto& item : items)
		{
			if (name == item.name)
			{
				return &item;
			}
		}

		return nullptr;
	}
};

}