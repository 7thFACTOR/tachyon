#pragma once
#include "opengl_globals.h"
#include "render/resources/texture_resource.h"
#include "render/texture.h"

namespace engine
{
class OpenGLTexture : public Texture
{
public:
	OpenGLTexture();
	~OpenGLTexture();

	bool upload() override;
	void free() override;
	void setData(void* data, size_t dataSize) override;
	void resize(u32 width, u32 height, u32 depth = 1) override;
	void setResource(ResourceId resId) override;
	void setParameters(const TextureParameters& value) override;
	const TextureParameters& getParameters() const override;
	u32 getWidth() const override { return width; }
	u32 getHeight() const override { return height; }
	u32 getDepth() const override { return depth; }

	GLuint oglTexId;
	GLuint oglDepthRBO;
	GLuint oglStencilRBO;
	GLuint oglTexType;

protected:
	ResourceId texture = nullResourceId;
	TextureParameters parameters;
	bool autoSizeRenderTarget = true;
	f32 renderTargetAutoSizeScale = 1;
	bool renderTarget = false;
	bool hasRenderTargetDepth = true;
	bool hasRenderTargetColor = true;
	bool hasRenderTargetStencil = true;
	u32 width = 0;
	u32 height = 0;
	u32 depth = 0;
};

}