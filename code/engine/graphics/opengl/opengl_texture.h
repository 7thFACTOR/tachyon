#pragma once
#include "base/defines.h"
#include "core/defines.h"
#include "graphics/types.h"
#include "graphics/texture.h"
#include "core/resources/texture_resource.h"

namespace engine
{
class E_API OpenglTexture : public Texture
{
public:
	OpenglTexture();
	~OpenglTexture();
	void setData(u32 lod, u32 depth, u8* data, size_t dataSize) override;
	void resize(u32 width, u32 height, u32 depth = 1) override;
	void setFromResource(ResourceId resId) override;
	void setParameters(const TextureParameters& value) override;
	bool upload() override;

public:
	void freeGL();

	GLuint oglTexId = 0;
	GLuint oglDepthRBO = 0;
	GLuint oglStencilRBO = 0;
	GLuint oglTexType = 0;
};

}