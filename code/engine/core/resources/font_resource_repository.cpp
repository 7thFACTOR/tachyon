// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "core/resources/font_resource_repository.h"
#include "base/defines.h"
#include "base/assert.h"
#include "base/logger.h"

namespace engine
{
bool FontRepository::load(Stream& stream, ResourceId resId)
{
	FontResource& font = *resources[resId];

	font.dynamic = true;
	u64 size;

	stream >> size;

	if (size)
	{
		font.fontFileData = new u8[size];
		font.fontFileDataSize = size;
		stream.read(font.fontFileData, size);
	}

	return true;
}

void FontRepository::unload(ResourceId resId)
{
	FontResource& font = *resources[resId];

	delete[] font.fontFileData;
	delete[] font.glyphs;
	delete[] font.kerningPairs;
}

}