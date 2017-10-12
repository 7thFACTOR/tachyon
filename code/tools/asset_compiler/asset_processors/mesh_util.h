// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "../common.h"
#include "base/math/color.h"
#include "base/math/vec3.h"
#include "base/math/quat.h"
#include "base/math/matrix.h"
#include "base/math/bbox.h"
#include "base/array.h"
#include "base/string.h"
#include "base/dictionary.h"
#include "graphics/texcoord.h"

namespace FBXSDK_NAMESPACE
{
	class FbxNode;
}

namespace ac
{
using namespace base;
using namespace FBXSDK_NAMESPACE;

const u32 maxTexCoordSets = 4;

struct ImportVertex
{
	Vec3 position, normal, tangent, bitangent;
	Color color;
	TexCoord texCoord[maxTexCoordSets];
};

struct ImportMeshInfo
{
	String name;
	String parentName;
	String dccTool;
	String author;
	String comments;
};

struct ImportGeometryBuffer
{
	size_t texCoordSetCount = 0;
	Array<ImportVertex> vertices;
	Array<u32> indices;
};

struct ImportMeshElement
{
	u32 startIndex = 0;
	u32 indexCount = 0;
	u32 primitiveCount = 0;
};

struct ImportTransform
{
	Vec3 translation;
	Vec3 scale;
	Vec3 pivot;
	Quat rotation;
};

struct ImportAnimationKey
{
	f32 time = 0;
	f32 value = 0;
	bool isTCB = false;
	f32 tens = 0;
	f32 cont = 0;
	f32 bias = 0;
};

struct ImportAnimationCurve
{
	Array<ImportAnimationKey> keys;
};

struct ImportAnimationLayerNodeCurves
{
	ImportAnimationCurve translationX;
	ImportAnimationCurve translationY;
	ImportAnimationCurve translationZ;
	ImportAnimationCurve rotationX;
	ImportAnimationCurve rotationY;
	ImportAnimationCurve rotationZ;
	ImportAnimationCurve scaleX;
	ImportAnimationCurve scaleY;
	ImportAnimationCurve scaleZ;
	ImportAnimationCurve visibility;
};

struct ImportAnimationLayer
{
	f32 weight = 1;
	Dictionary<struct ImportMesh*, ImportAnimationLayerNodeCurves> nodeCurves;
};

struct ImportAnimationClip
{
	f32 startTime, endTime;
	String name;
	Array<ImportAnimationLayer> layers;
};

struct ImportTexture
{
	String fileName;
	f32 blendFactor = 1;
	Vec3 tiling;
	Vec3 offset;
};

struct ImportMaterial
{
	String name;
	Color diffuse;
	Color ambient;
	Color specular;
	f32 shininess;
	f32 opacity;
	ImportTexture diffuseMap;
	ImportTexture normalMap;
	ImportTexture emissiveMap;
	ImportTexture specularMap;
	ImportTexture reflectionMap;
};

struct ImportMesh
{
	ImportMesh();
	virtual ~ImportMesh();

	void free();

	FbxNode* fbxNode = nullptr;
	ImportMeshInfo info;
	ImportTransform transform;
	BBox bbox;
	bool isLod = false;
	Array<ImportMeshElement*> elements;
	ImportGeometryBuffer geometryBuffer;
};

extern void logDebugMatrix(const Matrix& m);
extern void logDebugVec(const Vec3& v);
extern String f32ToString(f32 value);
extern Vec3 toEngine(const Vec3& v);

}