#include <string.h>
#include "gui/image.h>
#include "../3rdparty/stb_image/stb_image.h"

namespace horus
{
Image::Image(const std::string& filename)
{
	load(filename);
}

Image::~Image()
{
	free();
}

bool Image::load(const std::string& filename)
{
	free();
	int w = 0;
	int h = 0;
	int components = 0;
	int wantedComponents = 4;
	stbi_uc* imgData = stbi_load(filename.c_str(), &w, &h, &components, wantedComponents);

	if (!imgData || !w || !h || !components)
	{
		return false;
	}

	width = w;
	height = h;
	dataSize = w*h*wantedComponents;
	bpp = 32;
	data = new u8[dataSize];
	memcpy(data, imgData, dataSize);
	stbi_image_free(imgData);
	
	return true;
}

void Image::free()
{
	delete[] data;
	data = nullptr;
	dataSize = 0;
	width = height = 0;
	bpp = 0;
}

}