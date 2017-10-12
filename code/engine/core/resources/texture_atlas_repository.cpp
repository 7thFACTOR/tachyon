// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "core/resources/texture_atlas_repository.h"
#include "base/defines.h"
#include "base/assert.h"
#include "base/logger.h"

namespace engine
{
bool TextureAtlasRepository::load(Stream& stream, ResourceId resId)
{
	TextureAtlasResource& atlas = *resources[resId];
	u32 itemCount = 0;

	stream >> atlas.textureId;
	stream >> itemCount;

	atlas.items.resize(itemCount);

	for (u32 i = 0; i < itemCount; i++)
	{
		auto& item = atlas.items[i];

		stream >> item.name;
		stream >> item.rotated;
		stream >> item.rect.x;
		stream >> item.rect.y;
		stream >> item.rect.width;
		stream >> item.rect.height;
	}

	return true;
}

void TextureAtlasRepository::unload(ResourceId resId)
{
	auto atlas = resources[resId];

	atlas->items.clear();
}

}