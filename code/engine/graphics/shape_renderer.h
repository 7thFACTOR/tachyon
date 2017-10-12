// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/types.h"
#include "base/defines.h"
#include "base/dictionary.h"
#include "base/string.h"
#include "graphics/types.h"
#include "graphics/texcoord.h"
#include "base/math/color.h"
#include "base/math/matrix.h"
#include "base/math/vec3.h"
#include "graphics/mesh.h"

namespace base
{
	class Ray;
	class BBox;
}

namespace engine
{
using namespace base;
class Mesh;
class ShapeRenderer;

class E_API ShapeRenderer
{
public:
	ShapeRenderer();
	~ShapeRenderer();

	void begin();
	void end();
	inline void setTexture(ResourceId textureId) { currentTextureId = textureId; }
	void setBlendMode(BlendMode mode);
	void setInstanceBuffer(GpuBuffer* instanceBuffer);
	void setTranslation(const Vec3& translation);
	void setRotation(const Quat& rotation);
	void setScale(const Vec3& scale);
	void setTransform(const Vec3& translation, const Quat& rotation, const Vec3& scale);
	void resetTransform();
	void beginPrimitives(RenderPrimitive primitivesType);
	void endPrimitives();
	u32 getUsedVertexCount() const;
	u32 getMaxVertexCount() const;
	void addVertex(const Vec3& value);
	void setNormal(const Vec3& value);
	void setColor(const Color& value);
	void setTexCoord(const TexCoord& value);
	void drawWireBox(f32 width, f32 height, f32 depth);
	void drawSolidBox(f32 width, f32 height, f32 depth);
	void drawWireEllipsoid(f32 radius1, f32 radius2, f32 radius3, u32 dx = 10, u32 dy = 10);
	void drawSolidEllipsoid(f32 radius1, f32 radius2, f32 radius3, u32 dx = 10, u32 dy = 10);
	void drawSimpleWireEllipsoid(f32 radius1, f32 radius2, f32 radius3, u32 density = 10);
	void drawSimpleCircle(f32 radius1, f32 radius2, u32 segments = 10);
	void drawWireCylinder(f32 radius1, f32 radius2, f32 height, u32 dx = 10, u32 dy = 10);
	void drawSolidCylinder(f32 radius1, f32 radius2, f32 height, u32 dx = 10, u32 dy = 10);
	void drawSimpleWireCylinder(f32 radius1, f32 radius2, f32 height, u32 points = 10);
	void drawCross(f32 size);
	void drawSimpleAxis(f32 size);
	void drawGrid(
		f32 gridSize,
		u32 majorLineCount,
		u32 minorLineCount,
		const Color& majorLineColor,
		const Color& minorLineColor,
		const Color& middleLineColor);
public:
	bool texturingEnabled = true;
	bool lightEnabled = true;
	Vec2 textureScale = {1, 1};
	Vec2 textureOffset;

protected:
	void createMesh();

	struct ShapeRenderBatch
	{
		size_t vertexOffset = 0;
		size_t vertexCount = 0;
		RenderPrimitive primitiveType = RenderPrimitive::Triangles;
		RasterizerState rasterizerState;
		DepthStencilState depthStencilState;
		BlendState blendState;
		ResourceId gpuProgramId = nullResourceId;
		ResourceId textureId = nullResourceId;
		bool texturing = false;
		bool lighting = false;
		Vec2 textureScale;
		Vec2 textureOffset;
		GpuBuffer* instanceBuffer = nullptr;
		Matrix worldMatrix;
		Matrix projectionMatrix;
		Matrix viewMatrix;
	};

	Array<ShapeRenderBatch> shapeRenderBatches;
	ResourceId gpuProgramSolidId = nullResourceId;
	ResourceId gpuProgramTexturedId = nullResourceId;
	ResourceId gpuProgramSolidInstancedId = nullResourceId;
	ResourceId gpuProgramTexturedInstancedId = nullResourceId;
	String gpuProgramSolidName;
	String gpuProgramTexturedName;
	String gpuProgramSolidInstancedName;
	String gpuProgramTexturedInstancedName;
	Mesh mesh;
	ResourceId currentTextureId = nullResourceId;
	Vec3 currentNormal;
	Vec3 currentTangent;
	Vec3 currentBitangent;
	Color currentColor = Color::white;
	TexCoord currentTexCoord;
	u8* mappedVertices = nullptr;
	size_t maxVertexCount = 0;
	size_t currentVertexCount = 0;
	GpuBuffer* instanceBuffer = nullptr;
};

}