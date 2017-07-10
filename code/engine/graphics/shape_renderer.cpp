#include <stdio.h>
#include <string.h>
#include "base/assert.h"
#include "base/util.h"
#include "graphics/types.h"
#include "graphics/graphics.h"
#include "graphics/shape_renderer.h"
#include "core/globals.h"
#include "base/logger.h"
#include "core/resources/gpu_program_resource.h"
#include "core/resources/texture_resource.h"
#include "core/resources/material_resource.h"
#include "core/resource_repository.h"
#include "base/math/plane.h"
#include "base/math/conversion.h"
#include "base/math/intersection.h"
#include "base/math/util.h"
#include "graphics/texcoord.h"
#include "input/window.h"
#include "graphics/gpu_buffer.h"
#include "graphics/mesh.h"
#include "graphics/gpu_program.h"

namespace engine
{
//TODO: maybe make all these dynamic grow
static const u32 maxTextCharCount = 1024;
static const u32 maxShapeVertexCount = 250000;//TODO: make this per platform?
static const u32 shapeVertexCountReallocIncrement = 10000;

struct ShapeVertex
{
	Vec3 position, normal;
	Color color;
	TexCoord texCoord;
};

ShapeRenderer::ShapeRenderer()
{
	currentNormal = Vec3(0.0f, 1.0f, 0.0f);
	currentColor = Color::white;
	currentTexCoord.set(0, 0);

	mappedVertices = nullptr;
	instanceBuffer = nullptr;

	maxVertexCount = maxShapeVertexCount;
	currentVertexCount = 0;
	
	gpuProgramSolidName = "gpu_programs/shape_renderer_solid.gpu_program";
	gpuProgramTexturedName = "gpu_programs/shape_renderer_textured.gpu_program";
	gpuProgramSolidInstancedName = "gpu_programs/shape_renderer_solid_instanced.gpu_program";
	gpuProgramTexturedInstancedName = "gpu_programs/shape_renderer_textured_instanced.gpu_program";
	texturingEnabled = false;
}

ShapeRenderer::~ShapeRenderer()
{}

void ShapeRenderer::begin()
{
	if (gpuProgramSolidId == nullResourceId)
	{
		gpuProgramSolidId = loadResource(gpuProgramSolidName);
		gpuProgramTexturedId = loadResource(gpuProgramTexturedName);
		gpuProgramSolidInstancedId = loadResource(gpuProgramSolidInstancedName);
		gpuProgramTexturedInstancedId = loadResource(gpuProgramTexturedInstancedName);
		createMesh();
	}

	mappedVertices = (u8*)mesh.getVertexBuffer()->map(0, maxVertexCount * sizeof(ShapeVertex), GpuBufferMapType::Write);
	B_ASSERT(mappedVertices);
}

void ShapeRenderer::end()
{
	mesh.getVertexBuffer()->unmap();
	mappedVertices = nullptr;

	for (auto& batch : shapeRenderBatches)
	{
		GpuProgram* program = getGraphics().gpuPrograms[batch.gpuProgramId];

		if (!program)
		{
			continue;
		}

		getGraphics().rasterizerState = batch.rasterizerState;
		getGraphics().depthStencilState = batch.depthStencilState;
		getGraphics().blendState = batch.blendState;
		getGraphics().commitBlendState();
		getGraphics().commitRasterizerState();
		getGraphics().commitDepthStencilState();
		getGraphics().setWorldMatrix(batch.worldMatrix);
		getGraphics().setProjectionMatrix(batch.projectionMatrix);
		getGraphics().setViewMatrix(batch.viewMatrix);
		program->bind();
		program->setDefaultConstants();
		program->setIntUniform("useLight", batch.lighting ? 1 : 0);

		if (batch.texturing && batch.textureId != nullResourceId)
		{
			Texture* tex = getGraphics().textures[batch.textureId];

			if (tex)
			{
				program->setSamplerUniform(tex, "diffuseSampler", 0);
				program->setVec2Uniform("uvDiffuseRepeat", batch.textureScale);
				program->setVec2Uniform("uvDiffuseOffset", batch.textureOffset);
			}
		}

		mesh.getIndexBuffer()->primitiveType = batch.primitiveType;
		getGraphics().drawMesh(
			program,
			&mesh,
			batch.instanceBuffer,
			batch.vertexOffset,
			0,
			batch.vertexCount,
			batch.vertexCount);//TODO: if fan or strips are used, the indexCount will be different
	}

	shapeRenderBatches.resize(0);
	currentVertexCount = 0;
}

void ShapeRenderer::beginPrimitives(RenderPrimitive primitiveType)
{
	ShapeRenderBatch batch;
	
	batch.primitiveType = primitiveType;
	batch.vertexOffset = currentVertexCount;

	if (texturingEnabled)
	{
		batch.gpuProgramId = gpuProgramTexturedId;
	}
	else
	{
		batch.gpuProgramId = gpuProgramSolidId;
	}

	if (instanceBuffer && texturingEnabled)
	{
		batch.gpuProgramId = gpuProgramTexturedInstancedId;
	}
	else if (instanceBuffer && !texturingEnabled)
	{
		batch.gpuProgramId = gpuProgramSolidInstancedId;
	}

	batch.depthStencilState = getGraphics().depthStencilState;
	batch.rasterizerState = getGraphics().rasterizerState;
	batch.blendState = getGraphics().blendState;
	batch.textureId = currentTextureId;
	batch.texturing = texturingEnabled;
	batch.textureScale = textureScale;
	batch.textureOffset = textureOffset;
	batch.instanceBuffer = instanceBuffer;
	batch.worldMatrix = getGraphics().getWorldMatrix();
	batch.projectionMatrix = getGraphics().getProjectionMatrix();
	batch.viewMatrix = getGraphics().getViewMatrix();
	batch.lighting = lightEnabled;
	shapeRenderBatches.append(batch);
}

void ShapeRenderer::endPrimitives()
{
	auto& batch = shapeRenderBatches.back();
	batch.vertexCount = currentVertexCount - batch.vertexOffset;
	B_ASSERT(batch.vertexCount >= 0);
}

u32 ShapeRenderer::getUsedVertexCount() const
{
	return currentVertexCount;
}

u32 ShapeRenderer::getMaxVertexCount() const
{
	return maxVertexCount;
}

void ShapeRenderer::addVertex(const Vec3& value)
{
	size_t vertexOffset = currentVertexCount * sizeof(ShapeVertex);
	u8* startAddr = mappedVertices;
	u8* endAddr = maxVertexCount * sizeof(ShapeVertex) + startAddr;
	u8* ptr = startAddr + vertexOffset;

	if (endAddr <= ptr)
	{
		//TODO: maybe resize mesh buffers ?
		B_ASSERT(!"Too many shape vertices");
		return;
	}

	*((Vec3*)ptr) = value;
	ptr += sizeof(Vec3);
	*((Vec3*)ptr) = currentNormal;
	ptr += sizeof(Vec3);
	*((Color*)ptr) = currentColor;
	ptr += sizeof(Color);
	*((TexCoord*)ptr) = currentTexCoord;
	ptr += sizeof(TexCoord);
	++currentVertexCount;
}

void ShapeRenderer::setNormal(const Vec3& value)
{
	currentNormal = value;
}

void ShapeRenderer::setColor(const Color& value)
{
	currentColor = value;
}

void ShapeRenderer::setTexCoord(const TexCoord& value)
{
	currentTexCoord = value;
}

void ShapeRenderer::drawWireBox(f32 width, f32 height, f32 depth)
{
	f32 w2 = width / 2.0f;
	f32 h2 = height / 2.0f;
	f32 d2 = depth / 2.0f;

	// bottom
	Vec3 v1[] = { 
		Vec3(-w2, -h2, d2),
		Vec3(-w2, -h2, -d2),
		Vec3(w2, -h2, -d2),
		Vec3(w2, -h2, d2) };
	// top
	Vec3 v2[] = {
		Vec3(-w2, h2, -d2),
		Vec3(-w2, h2, d2),
		Vec3(w2, h2, d2),
		Vec3(w2, h2, -d2) };
	// back
	Vec3 v3[] = {
		Vec3(-w2, -h2, -d2),
		Vec3(-w2, h2, -d2),
		Vec3(w2, h2, -d2),
		Vec3(w2, -h2, -d2) };
	// front
	Vec3 v4[] = {
		Vec3(w2, -h2, d2),
		Vec3(w2, h2, d2),
		Vec3(-w2, h2, d2),
		Vec3(-w2, -h2, d2) };
	// left
	Vec3 v5[] = {
		Vec3(-w2, -h2, d2),
		Vec3(-w2, h2, d2),
		Vec3(-w2, h2, -d2),
		Vec3(-w2, -h2, -d2) };
	// right
	Vec3 v6[] = { 
		Vec3(w2, -h2, -d2),
		Vec3(w2, h2, -d2),
		Vec3(w2, h2, d2),
		Vec3(w2, -h2, d2) };
	TexCoord uvs[] = { 
		TexCoord(0, 0),
		TexCoord(0, 1),
		TexCoord(1, 1),
		TexCoord(1, 0) };

	beginPrimitives(RenderPrimitive::LineStrip);
	//bottom
	addVertex(v1[0]);
	addVertex(v1[1]);
	addVertex(v1[2]);
	addVertex(v1[3]);
	addVertex(v1[0]);
	endPrimitives();

	beginPrimitives(RenderPrimitive::LineStrip);
	//top
	addVertex(v2[0]);
	addVertex(v2[1]);
	addVertex(v2[2]);
	addVertex(v2[3]);
	addVertex(v2[0]);
	endPrimitives();

	beginPrimitives(RenderPrimitive::LineStrip);
	//back
	addVertex(v3[0]);
	addVertex(v3[1]);
	addVertex(v3[2]);
	addVertex(v3[3]);
	addVertex(v3[0]);
	endPrimitives();

	beginPrimitives(RenderPrimitive::LineStrip);
	//front
	addVertex(v4[0]);
	addVertex(v4[1]);
	addVertex(v4[2]);
	addVertex(v4[3]);
	addVertex(v4[0]);
	endPrimitives();

	beginPrimitives(RenderPrimitive::LineStrip);
	//left
	addVertex(v5[0]);
	addVertex(v5[1]);
	addVertex(v5[2]);
	addVertex(v5[3]);
	addVertex(v5[0]);
	endPrimitives();

	beginPrimitives(RenderPrimitive::LineStrip);
	//right
	addVertex(v6[0]);
	addVertex(v6[1]);
	addVertex(v6[2]);
	addVertex(v6[3]);
	addVertex(v6[0]);
	endPrimitives();
}

void ShapeRenderer::drawSolidBox(f32 width, f32 height, f32 depth)
{
	f32 w2 = width / 2.0f;
	f32 h2 = height / 2.0f;
	f32 d2 = depth / 2.0f;

	// bottom
	Vec3 v1[] = {
		Vec3(-w2, -h2, d2),
		Vec3(w2, -h2, d2),
		Vec3(w2, -h2, -d2),
		Vec3(-w2, -h2, -d2) };
	// top
	Vec3 v2[] = {
		Vec3(-w2, h2, d2),
		Vec3(w2, h2, d2),
		Vec3(w2, h2, -d2),
		Vec3(-w2, h2, -d2) };
	// back
	Vec3 v3[] = {
		Vec3(-w2, h2, -d2),
		Vec3(w2, h2, -d2),
		Vec3(w2, -h2, -d2),
		Vec3(-w2, -h2, -d2) };
	// front
	Vec3 v4[] = {
		Vec3(-w2, h2, d2),
		Vec3(w2, h2, d2),
		Vec3(w2, -h2, d2),
		Vec3(-w2, -h2, d2) };
	// left
	Vec3 v5[] = {
		Vec3(-w2, -h2, d2),
		Vec3(-w2, h2, d2),
		Vec3(-w2, h2, -d2),
		Vec3(-w2, -h2, -d2) };
	// right
	Vec3 v6[] = {
		Vec3(w2, -h2, d2),
		Vec3(w2, h2, d2),
		Vec3(w2, h2, -d2),
		Vec3(w2, -h2, -d2) };

	TexCoord uvs[] = {
		TexCoord(0, 0),
		TexCoord(0, 1),
		TexCoord(1, 0),
		TexCoord(1, 1) };

	beginPrimitives(RenderPrimitive::TriangleStrip);
	//bottom
	setNormal({0, -1, 0});
	setTexCoord(uvs[0]);
	addVertex(v1[2]);
	setTexCoord(uvs[1]);
	addVertex(v1[1]);
	setTexCoord(uvs[2]);
	addVertex(v1[3]);
	setTexCoord(uvs[3]);
	addVertex(v1[0]);
	endPrimitives();

	beginPrimitives(RenderPrimitive::TriangleStrip);
	//top
	setNormal({ 0, 1, 0 });
	setTexCoord(uvs[0]);
	addVertex(v2[3]);
	setTexCoord(uvs[1]);
	addVertex(v2[0]);
	setTexCoord(uvs[2]);
	addVertex(v2[2]);
	setTexCoord(uvs[3]);
	addVertex(v2[1]);
	endPrimitives();

	beginPrimitives(RenderPrimitive::TriangleStrip);
	//back
	setNormal({ 0, 0, -1 });
	setTexCoord(uvs[0]);
	addVertex(v3[3]);
	setTexCoord(uvs[1]);
	addVertex(v3[0]);
	setTexCoord(uvs[2]);
	addVertex(v3[2]);
	setTexCoord(uvs[3]);
	addVertex(v3[1]);
	endPrimitives();

	beginPrimitives(RenderPrimitive::TriangleStrip);
	//front
	setNormal({ 0, 0, 1 });
	setTexCoord(uvs[0]);
	addVertex(v4[2]);
	setTexCoord(uvs[1]);
	addVertex(v4[1]);
	setTexCoord(uvs[2]);
	addVertex(v4[3]);
	setTexCoord(uvs[3]);
	addVertex(v4[0]);
	endPrimitives();

	beginPrimitives(RenderPrimitive::TriangleStrip);
	//left
	setNormal({ -1, 0, 0 });
	setTexCoord(uvs[2]);
	addVertex(v5[3]);
	setTexCoord(uvs[0]);
	addVertex(v5[0]);
	setTexCoord(uvs[3]);
	addVertex(v5[2]);
	setTexCoord(uvs[1]);
	addVertex(v5[1]);
	endPrimitives();

	beginPrimitives(RenderPrimitive::TriangleStrip);
	//right
	setNormal({ 1, 0, 0 });
	setTexCoord(uvs[1]);
	addVertex(v6[2]);
	setTexCoord(uvs[3]);
	addVertex(v6[1]);
	setTexCoord(uvs[0]);
	addVertex(v6[3]);
	setTexCoord(uvs[2]);
	addVertex(v6[0]);
	endPrimitives();
}

void ShapeRenderer::drawWireEllipsoid(f32 radius1, f32 radius2, f32 radius3, u32 dx, u32 dy)
{
	getGraphics().rasterizerState.fillMode = FillMode::Wireframe;
	drawSolidEllipsoid(radius1, radius2, radius3, dx, dy);
	getGraphics().rasterizerState.fillMode = FillMode::Solid;
}

void ShapeRenderer::drawSolidEllipsoid(f32 radius1, f32 radius2, f32 radius3, u32 dx, u32 dy)
{
	if (!dx)
	{
		dx = 1;
	}

	if (!dy)
	{
		dy = 1;
	}

	f32 xStep = twoPi / (f32)dx;
	f32 yStep = pi / (f32)dy;
	Vec3 normal1, normal2, normal3, normal4;
	Vec3 v1, v2, v3, v4;
	TexCoord uv1, uv2, uv3, uv4;
	f32 uStep = 1.0f / (f32)dx;
	f32 vStep = 1.0f / (f32)dy;

	//TODO: optimize use a single tri-strip
	beginPrimitives(RenderPrimitive::Triangles);

	//TODO: wrong drawing of sphere. fix!
	for (f32 x = twoPi, u = 0.0f; x >= 0; x -= xStep, u += uStep)
	{
		for (f32 y = -halfPi, v = 0.0f; y < halfPi; y += yStep, v += vStep)
		{
			bool isTopCap = fabs(y - (halfPi - yStep)) < 0.0001f;
			bool isBtmCap = y <= -halfPi;

			//TODO: optimize trig functions use a LUT
			v1.x = radius1 * sinf(x) * cosf(y);
			v1.z = radius2 * cosf(x) * cosf(y);
			v1.y = radius3 * sinf(y);

			v2.x = radius1 * sinf(x) * cosf(y + yStep);
			v2.z = radius2 * cosf(x) * cosf(y + yStep);
			v2.y = radius3 * sinf(y + yStep);

			v3.x = radius1 * sinf(x + xStep) * cosf(y + yStep);
			v3.z = radius2 * cosf(x + xStep) * cosf(y + yStep);
			v3.y = radius3 * sinf(y + yStep);

			v4.x = radius1 * sinf(x + xStep) * cosf(y);
			v4.z = radius2 * cosf(x + xStep) * cosf(y);
			v4.y = radius3 * sinf(y);

			uv4.set(u, v);
			uv3.set(u, v + vStep);
			uv2.set(u + uStep, v + vStep);
			uv1.set(u + uStep, v);

			normal1 = v1;
			normal1.normalize();
			normal2 = v2;
			normal2.normalize();
			normal3 = v3;
			normal3.normalize();
			normal4 = v4;
			normal4.normalize();

			if (isTopCap)
			{
				// tri1
				setNormal(normal4);
				setTexCoord(uv4);
				addVertex(v4);

				setNormal(normal2);
				setTexCoord(uv2);
				addVertex(v2);

				setNormal(normal1);
				setTexCoord(uv1);
				addVertex(v1);
			}
			else
			if (isBtmCap)
			{
				// tri1
				setNormal(normal3);
				setTexCoord(uv3);
				addVertex(v3);

				setNormal(normal2);
				setTexCoord(uv2);
				addVertex(v2);

				setNormal(normal1);
				setTexCoord(uv1);
				addVertex(v1);
			}
			else
			{
			// tri1
			setNormal(normal4);
			setTexCoord(uv4);
			addVertex(v4);
			
			setNormal(normal2);
			setTexCoord(uv2);
			addVertex(v2);
			
			setNormal(normal1);
			setTexCoord(uv1);
			addVertex(v1);

			// tri2
			setNormal(normal4);
			setTexCoord(uv4);
			addVertex(v4);

			setNormal(normal3);
			setTexCoord(uv3);
			addVertex(v3);

			setNormal(normal2);
			setTexCoord(uv2);
			addVertex(v2);
			}
		}
	}

	endPrimitives();
}

void ShapeRenderer::drawWireCylinder(f32 radius1, f32 radius2, f32 height, u32 dx, u32 dy)
{
	// cylinder has center in 0,0,0
	f32 u, step;

	if (dx < 3)
	{
		dx = 3;
	}

	if (dy < 3)
	{
		dy = 3;
	}

	step = (f32) twoPi / dx;

	// draw top cap
	beginPrimitives(RenderPrimitive::Lines);

	for (u = 0; u < twoPi; u += step)
	{
		addVertex({ 0, height / 2, 0 });
		addVertex({ radius1 * sinf(u), height / 2, radius1 * cosf(u) });
	}

	endPrimitives();

	// draw bottom cap
	beginPrimitives(RenderPrimitive::Lines);
	
	for (u = 0; u < twoPi; u += step)
	{
		addVertex({ 0, -height / 2.0f, 0 });
		addVertex({ radius2 * sinf(u), -height / 2.0f, radius2 * cosf(u) });
	}

	endPrimitives();

	// draw medians
	for (f32 v = 0; v <= 1.0f; v += (f32) 1.0f / dy)
	{
		f32 r = radius1 + v * (radius2 - radius1);
		f32 h = height / 2.0f - height * v;

		beginPrimitives(RenderPrimitive::LineStrip);

		for (u = 0; u < twoPi; u += step)
		{
			addVertex({ r * sinf(u), h, r * cosf(u) });
		}

		addVertex({ r * sinf(twoPi), h, r * cosf(twoPi) });
		endPrimitives();
	}

	f32 r = radius2;
	f32 h = -height;
	
	// last
	beginPrimitives(RenderPrimitive::LineStrip);
	
	for (u = 0; u < twoPi; u += step)
	{
		addVertex({ r * sinf(u), h, r * cosf(u) });
	}

	addVertex({ r * sinf(twoPi), h, r * cosf(twoPi) });
	endPrimitives();

	// draw generators
	beginPrimitives(RenderPrimitive::Lines);
	
	for (u = 0; u < twoPi; u += step)
	{
		addVertex({ radius1 * sinf(u), height / 2.0f, radius1 * cosf(u) });
		addVertex({ radius2 * sinf(u), -height / 2.0f, radius2 * cosf(u) });
	}

	addVertex({ radius1 * sinf(twoPi), height / 2.0f, radius1 * cosf(twoPi) });
	addVertex({ radius2 * sinf(twoPi), -height / 2.0f, radius2 * cosf(twoPi) });
	endPrimitives();
}

void ShapeRenderer::drawSolidCylinder(f32 radius1, f32 radius2, f32 height, u32 dx, u32 dy)
{
	//TODO: draw solid cylinder
	B_ASSERT_NOT_IMPLEMENTED;
}

void ShapeRenderer::drawSimpleWireEllipsoid(f32 radius1, f32 radius2, f32 radius3, u32 density)
{
	f32 u, step;

	if (!density)
	{
		density = 1;
	}

	step = (f32)twoPi / density;

	// XY
	beginPrimitives(RenderPrimitive::LineStrip);

	for (u = 0; u < twoPi; u += step)
	{
		addVertex({ radius3 * sinf(u), radius3 * cosf(u), 0 });
	}

	addVertex({ radius3 * sinf(twoPi), radius3 * cosf(twoPi), 0 });
	endPrimitives();

	// ZX
	beginPrimitives(RenderPrimitive::LineStrip);
	
	for (u = 0; u < twoPi; u += step)
	{
		addVertex({ radius3 * sinf(u), 0, radius3 * cosf(u) });
	}
	
	addVertex({ radius3 * sinf(twoPi), 0, radius3 * cosf(twoPi) });
	endPrimitives();

	// YZ
	beginPrimitives(RenderPrimitive::LineStrip);
	
	for (u = 0; u < twoPi; u += step)
	{
		addVertex({ 0, radius3 * sinf(u), radius3 * cosf(u) });
	}

	addVertex({ 0, radius3 * sinf(twoPi), radius3 * cosf(twoPi) });
	endPrimitives();
}

void ShapeRenderer::drawSimpleCircle(f32 radius1, f32 radius2, u32 segments)
{
	f32 u, step;

	if (!segments)
	{
		segments = 1;
	}

	step = (f32)twoPi / segments;

	// XY
	beginPrimitives(RenderPrimitive::LineStrip);

	for (u = 0; u < twoPi; u += step)
	{
		addVertex({ radius1 * sinf(u), radius2 * cosf(u), 0 });
	}

	endPrimitives();
}

void ShapeRenderer::drawSimpleWireCylinder(f32 radius1, f32 radius2, f32 height, u32 points)
{
	// cylinder has center in 0,0,0
	f32 u, step;

	if (!points)
	{
		points = 10;
	}

	step = (f32) twoPi / points;

	// draw top cap
	beginPrimitives(RenderPrimitive::Lines);
	
	for (u = 0; u < twoPi; u += step)
	{
		addVertex({ 0, height / 2.0f, 0 });
		addVertex({ radius1 * sinf(u), height / 2.0f, radius1 * cosf(u) });
	}

	endPrimitives();

	// draw bottom cap
	beginPrimitives(RenderPrimitive::Lines);
	
	for (u = 0; u < twoPi; u += step)
	{
		addVertex({ 0, -height / 2.0f, 0 });
		addVertex({ radius2 * sinf(u), -height / 2.0f, radius2 * cosf(u) });
	}

	endPrimitives();

	// draw 4 generators
	beginPrimitives(RenderPrimitive::Lines);
	
	for (u = 0; u < twoPi; u += (f32) twoPi / 4)
	{
		addVertex({ radius1 * sinf(u), height / 2.0f, radius1 * cosf(u) });
		addVertex({ radius2 * sinf(u), -height / 2.0f, radius2 * cosf(u) });
	}

	addVertex({ radius1 * sinf(twoPi), height / 2.0f, radius1 * cosf(twoPi) });
	addVertex({ radius2 * sinf(twoPi), -height / 2.0f, radius2 * cosf(twoPi) });
	endPrimitives();
}

void ShapeRenderer::drawCross(f32 size)
{
	f32 halfSize = size / 2.0f;

	beginPrimitives(RenderPrimitive::Lines);
	// X
	addVertex({ -halfSize, 0, 0 });
	addVertex({ halfSize, 0, 0 });
	// Y
	addVertex({ 0, -halfSize, 0 });
	addVertex({ 0, halfSize, 0 });
	// Z
	addVertex({ 0, 0, -halfSize });
	addVertex({ 0, 0, halfSize });
	endPrimitives();
}

void ShapeRenderer::drawSimpleAxis(f32 size)
{
	beginPrimitives(RenderPrimitive::Lines);
	// X
	setColor(Color::red);
	addVertex({ 0, 0, 0 });
	addVertex({ size, 0, 0 });
	// Y
	setColor(Color::green);
	addVertex({ 0, 0, 0 });
	addVertex({ 0, size, 0 });
	// Z
	setColor(Color::blue);
	addVertex({ 0, 0, 0 });
	addVertex({ 0, 0, size });
	endPrimitives();
}

void ShapeRenderer::setBlendMode(BlendMode mode)
{
	getGraphics().blendState.renderTarget[0].enable = mode != BlendMode::Opaque;

	switch (mode)
	{
	case BlendMode::Opaque:
		getGraphics().blendState.renderTarget[0].blendOp = BlendOperation::None;
		getGraphics().blendState.renderTarget[0].blendOpAlpha = BlendOperation::None;
		getGraphics().blendState.renderTarget[0].srcBlend = BlendType::One;
		getGraphics().blendState.renderTarget[0].destBlend = BlendType::Zero;
		getGraphics().blendState.renderTarget[0].srcBlendAlpha = BlendType::One;
		getGraphics().blendState.renderTarget[0].destBlendAlpha = BlendType::Zero;
		break;
	case BlendMode::Additive:
		getGraphics().blendState.renderTarget[0].blendOp = BlendOperation::Add;
		getGraphics().blendState.renderTarget[0].blendOpAlpha = BlendOperation::Add;
		getGraphics().blendState.renderTarget[0].srcBlend = BlendType::One;
		getGraphics().blendState.renderTarget[0].destBlend = BlendType::One;
		getGraphics().blendState.renderTarget[0].srcBlendAlpha = BlendType::Zero;
		getGraphics().blendState.renderTarget[0].destBlendAlpha = BlendType::Zero;
		break;
	case BlendMode::Alpha:
		getGraphics().blendState.renderTarget[0].blendOp = BlendOperation::Add;
		getGraphics().blendState.renderTarget[0].blendOpAlpha = BlendOperation::Add;
		getGraphics().blendState.renderTarget[0].srcBlend = BlendType::SrcAlpha;
		getGraphics().blendState.renderTarget[0].destBlend = BlendType::InvSrcAlpha;
		getGraphics().blendState.renderTarget[0].srcBlendAlpha = BlendType::Zero;
		getGraphics().blendState.renderTarget[0].destBlendAlpha = BlendType::Zero;
		break;
	case BlendMode::PremultipliedAlpha:
		getGraphics().blendState.renderTarget[0].blendOp = BlendOperation::Add;
		getGraphics().blendState.renderTarget[0].blendOpAlpha = BlendOperation::Add;
		getGraphics().blendState.renderTarget[0].srcBlend = BlendType::One;
		getGraphics().blendState.renderTarget[0].destBlend = BlendType::InvSrcAlpha;
		getGraphics().blendState.renderTarget[0].srcBlendAlpha = BlendType::Zero;
		getGraphics().blendState.renderTarget[0].destBlendAlpha = BlendType::Zero;
		break;
	}
}

void ShapeRenderer::setInstanceBuffer(GpuBuffer* newInstanceBuffer)
{
	instanceBuffer = newInstanceBuffer;
}

void ShapeRenderer::setTranslation(const Vec3& translation)
{
	Matrix m = getGraphics().getWorldMatrix();
	
	m.setTranslation(translation);
	getGraphics().setWorldMatrix(m);
}

void ShapeRenderer::setRotation(const Quat& rotation)
{
	Matrix m = getGraphics().getWorldMatrix();
	Matrix m2;
	
	toMatrix(rotation, m2);
	m2.setTranslation(m.getTranslation());
	m2.setScale(m.getScale());
	getGraphics().setWorldMatrix(m2);
}

void ShapeRenderer::setScale(const Vec3& scale)
{
	Matrix m = getGraphics().getWorldMatrix();

	m.setScale(scale);
	getGraphics().setWorldMatrix(m);
}

void ShapeRenderer::setTransform(const Vec3& translation, const Quat& rotation, const Vec3& scale)
{
	Matrix mt, mr, ms;

	mt.setTranslation(translation);
	ms.setScale(scale);
	toMatrix(rotation, mr);
	getGraphics().setWorldMatrix(mr*ms*mt);
}

void ShapeRenderer::resetTransform()
{
	getGraphics().setWorldMatrix(Matrix());
}

void ShapeRenderer::drawGrid(
	f32 gridSize,
	u32 majorLineCount,
	u32 minorLineCount,
	const Color& majorLineColor,
	const Color& minorLineColor,
	const Color& middleLineColor)
{
	DepthStencilState dss = getGraphics().depthStencilState;
	RasterizerState rs = getGraphics().rasterizerState;

	getGraphics().depthStencilState.depthTest = true;
	getGraphics().depthStencilState.depthWrite = true;
	getGraphics().depthStencilState.depthFunc = CompareFunction::LessEqual;
	getGraphics().rasterizerState.cullMode = CullMode::None;
	setBlendMode(BlendMode::Alpha);
	texturingEnabled = false;

	f32 gx, gz, cellsize, cellMajorSize;
	
	gx = -gridSize / 2.0f;
	gz = -gridSize / 2.0f;
	cellsize = (f32) gridSize / minorLineCount;
	cellMajorSize = (f32) gridSize / majorLineCount;

	beginPrimitives(RenderPrimitive::Lines);
	setColor(minorLineColor);

	for (u32 i = 0; i <= minorLineCount; ++i)
	{
		addVertex({ gx, 0.0f, gz });
		addVertex({ gx, 0.0f, gz + gridSize });
		gx += cellsize;
	}

	endPrimitives();

	gx = -gridSize / 2.0f;
	beginPrimitives(RenderPrimitive::Lines);
	setColor(minorLineColor);

	for (u32 i = 0; i <= minorLineCount; ++i)
	{
		addVertex({ gx, 0.0f, gz });
		addVertex({ gx + gridSize, 0.0f, gz });
		gz += cellsize;
	}

	endPrimitives();

	// major lines
	gx = -gridSize / 2.0f;
	gz = -gridSize / 2.0f;
	beginPrimitives(RenderPrimitive::Lines);
	setColor(majorLineColor);

	for (u32 i = 0; i <= majorLineCount; ++i)
	{
		addVertex({ gx, 0.0f, gz });
		addVertex({ gx, 0.0f, gz + gridSize });
		gx += cellMajorSize;
	}

	endPrimitives();

	gx = -gridSize / 2.0f;
	beginPrimitives(RenderPrimitive::Lines);
	setColor(majorLineColor);

	for (u32 i = 0; i <= majorLineCount; ++i)
	{
		addVertex({ gx, 0.0f, gz });
		addVertex({ gx + gridSize, 0.0f, gz });
		gz += cellMajorSize;
	}

	endPrimitives();

	gx = -gridSize / 2.0f;
	beginPrimitives(RenderPrimitive::Lines);
	setColor(middleLineColor);
	addVertex({ gx, 0.0f, 0.0f });
	addVertex({ -gx, 0.0f, 0.0f });
	addVertex({ 0.0f, 0.0f, gx });
	addVertex({ 0.0f, 0.0f, -gx });
	endPrimitives();

	getGraphics().depthStencilState = dss;
	getGraphics().rasterizerState = rs;
}

void ShapeRenderer::createMesh()
{
	if (mesh.getVertexBuffer())
	{
		// already created
		return;
	}

	mesh.createBuffers();
	GpuBufferDescriptor descVb;
	GpuBufferDescriptor descIb;

	descVb.addPositionElement();
	descVb.addNormalElement();
	descVb.addColorElement();
	descVb.addTexCoordElement();
	descIb.addIndexElement();

	mesh.getVertexBuffer()->usage = GpuBufferUsageType::Dynamic;
	mesh.getVertexBuffer()->setupElements(descVb, maxShapeVertexCount);
	mesh.getVertexBuffer()->initialize(maxShapeVertexCount * descVb.getStride());

	B_ASSERT(sizeof(ShapeVertex) == mesh.getVertexBuffer()->desc.getStride());

	mesh.getIndexBuffer()->setupElements(descIb, maxShapeVertexCount);
	mesh.getIndexBuffer()->initialize(maxShapeVertexCount * descIb.getStride());

	u32* indices = (u32*)mesh.getIndexBuffer()->mapFull();

	B_ASSERT(indices);

	if (indices)
	{
		for (size_t i = 0; i < maxShapeVertexCount; ++i)
		{
			indices[i] = i;
		}

		mesh.getIndexBuffer()->unmap();
	}
}

}