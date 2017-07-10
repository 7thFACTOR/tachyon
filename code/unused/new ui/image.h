#pragma once

#include <core/defines.h>
#include <core/types.h>
#include <base/string.h>

namespace engine
{
class ENGINE_API Image
{
public:
	Image() {}
	Image(const String& filename);
	~Image();

	bool load(const String& filename);

	inline u8* getData() const { return data; }
	inline u32 getWidth() const { return width; }
	inline u32 getHeight() const { return height; }
	inline u32 getBitsPerPixel() const { return bpp; }

protected:
	void free();

	u8* data = nullptr;
	u32 dataSize = 0;
	u32 width = 0;
	u32 height = 0;
	u32 bpp = 0;
};

}