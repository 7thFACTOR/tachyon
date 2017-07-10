#include "../common.h"
#include "fbxsdk.h"
#include "base/math/vec3.h"
#include "base/util.h"
#include "base/logger.h"
#include "base/file.h"
#include "base/json.h"
#include "fbx_processor.h"
#include "../asset_compiler.h"
#include "core/resources/mesh_resource.h"
#include "graphics/types.h"
#include "../project.h"

using namespace FBXSDK_NAMESPACE;

namespace ac
{
using namespace base;
using namespace engine;

FbxProcessor::FbxProcessor()
{
	Array<String> extensions;

	extensions.append(".fbx");
	supportedAssetType = SupportedAssetType(ResourceType::None, extensions);
	supportedAssetType.importerOnly = true;
	supportedAssetType.autoImportWhenModified = true;
}

FbxProcessor::~FbxProcessor()
{}

bool FbxProcessor::import(const String& importFilename, JsonDocument& assetCfg)
{
	assetCfg.addValue("type", "fbx");
	FbxFileImporter fbxImporter;
	FbxImportOptions fbxOptions;

	if (!fbxImporter.import(importFilename, fbxOptions))
		return false;

	String basePath = getFilenamePath(importFilename);
	
	importMeshes(fbxImporter, basePath, assetCfg);
	importAnimations(fbxImporter, basePath, assetCfg);
	importMaterials(fbxImporter, basePath, assetCfg);

	fbxImporter.free();

	return true;
}

bool FbxProcessor::process(Asset& asset, JsonDocument& assetCfg)
{
	return import(asset.absFilename, assetCfg);
}

bool FbxProcessor::importMeshes(FbxFileImporter& fbxImporter, const String& basePath, JsonDocument& assetCfg)
{
	for (auto mesh : fbxImporter.meshes)
	{
		File file;
		String fbxName = getFilenameBasename(fbxImporter.sourceFbx);
		String meshFilename = mergePathBasenameExtension(basePath, fbxName + "_" + mesh->info.name, ".mesh");

		if (!file.open(meshFilename, FileOpenFlags::BinaryWrite))
			return false;

		BBox bbox = mesh->bbox;
		bool isLod = mesh->isLod;
		u32 elementCount = mesh->elements.size();
		ImportTransform transform;// = mesh.transform;
		Matrix bindShapeMatrix;
		u32 indexCount = mesh->geometryBuffer.indices.size();
		u32 vertexCount = mesh->geometryBuffer.vertices.size();
		u32 texCoordSetCount = mesh->geometryBuffer.texCoordSetCount;
		u32 jointCount = 0;
		MeshResourceFlags meshFlags;

		meshFlags |= MeshResourceFlags::HasPositions;
		meshFlags |= MeshResourceFlags::HasNormals;
		meshFlags |= MeshResourceFlags::HasBinormals;
		meshFlags |= MeshResourceFlags::HasTangents;
		meshFlags |= MeshResourceFlags::HasColors;

		file << meshFlags;
		file << RenderPrimitive::Triangles;
		file << (u32)indexCount;
		file << (u32)vertexCount;
		file << (u32)texCoordSetCount;
		file << (u32)elementCount;
		file << (u32)jointCount;//TODO: JOINTCOUNT (size_t)(joints ? joints->size() : 0);
		file << transform.pivot;
		file << bbox.min;
		file << bbox.max;
		file << bindShapeMatrix;

		u32 elementStartIndex;
		u32 elementIndexCount;
		u32 elementPrimCount;

		for (u32 i = 0; i < elementCount; i++)
		{
			elementStartIndex = mesh->elements[i]->startIndex;
			elementIndexCount = mesh->elements[i]->indexCount;
			elementPrimCount = mesh->elements[i]->primitiveCount;

			file << (u32)elementStartIndex;
			file << (u32)elementIndexCount;
			file << (u32)elementPrimCount;
		}

		//TODO: write joints

		ImportVertex vtx;

		B_LOG_DEBUG("Writing " << vertexCount << " vertices...");

		for (u32 i = 0; i < vertexCount; i++)
		{
			auto& vert = mesh->geometryBuffer.vertices[i];
			// read from data source
			vtx.position = toEngine(vert.position);
			vtx.normal = toEngine(vert.normal);
			vtx.bitangent = toEngine(vert.bitangent);
			vtx.tangent = toEngine(vert.tangent);
			vtx.color = vert.color;

			for (u32 j = 0; j < texCoordSetCount; j++)
			{
				vtx.texCoord[j].x = vert.texCoord[j].x;
				vtx.texCoord[j].y = vert.texCoord[j].y;
			}

			// write to mesh file

			if (!!(meshFlags & MeshResourceFlags::HasPositions))
			{
				file << vtx.position;
			}

			if (!!(meshFlags & MeshResourceFlags::HasNormals))
			{
				file << vtx.normal;
			}

			if (!!(meshFlags & MeshResourceFlags::HasBinormals))
			{
				file << vtx.bitangent;
			}

			if (!!(meshFlags & MeshResourceFlags::HasTangents))
			{
				file << vtx.tangent;
			}

			if (!!(meshFlags & MeshResourceFlags::HasColors))
			{
				file << vtx.color;
			}

			for (u32 j = 0; j < texCoordSetCount; j++)
			{
				file << 1.0f - vtx.texCoord[j].x;
				file << vtx.texCoord[j].y;
			}
		}

		B_LOG_DEBUG("Writing " << indexCount << " indices...");
		for (u32 i = 0; i < indexCount; i++)
		{
			file << mesh->geometryBuffer.indices[i];
		}

		file.close();
		B_LOG_DEBUG("Done writing mesh.");
		emittedAssets.append(meshFilename);
	}

	return true;
}

bool FbxProcessor::importAnimations(FbxFileImporter& fbxImporter, const String& basePath, JsonDocument& assetCfg)
{
	for (auto& anim : fbxImporter.animations)
	{
		String fbxName = getFilenameBasename(fbxImporter.sourceFbx);
		String animFilename = mergePathBasenameExtension(basePath, fbxName + "_" + anim.name, ".animation");
		JsonDocument doc;

		doc.beginDocument();
		doc.addValue("startTime", anim.startTime);
		doc.addValue("endTime", anim.endTime);
		doc.beginArray("layers");

		for (auto& layer : anim.layers)
		{
			doc.beginObject(); // layer
			doc.addValue("weight", layer.weight);
			doc.beginArray("nodes");

			for (auto& nodeCurves : layer.nodeCurves)
			{
				if (!nodeCurves.key->fbxNode)
					continue;

				doc.beginObject(); // node
				doc.addValue("name", nodeCurves.key->fbxNode->GetNameOnly());

				auto writeCurveJson = [](JsonDocument& animDoc, ImportAnimationCurve& animCurve, const String& curveName)
				{
					if (animCurve.keys.size())
					{
						animDoc.beginObject(curveName);
						animDoc.beginArray("keys");
				
						for (auto& key : animCurve.keys)
						{
							animDoc.beginObject();
							animDoc.addValue("time", key.time);
							animDoc.addValue("value", key.value);

							if (key.isTCB)
							{
								animDoc.addValue("isTCB", true);
								animDoc.addValue("tens", key.tens);
								animDoc.addValue("cont", key.cont);
								animDoc.addValue("bias", key.bias);
							}

							animDoc.endObject();
						}
					
						animDoc.endArray(); // keys
						animDoc.endObject(); // curve
					}
				};

				writeCurveJson(doc, nodeCurves.value.translationX, "translation.x");
				writeCurveJson(doc, nodeCurves.value.translationY, "translation.y");
				writeCurveJson(doc, nodeCurves.value.translationZ, "translation.z");
				writeCurveJson(doc, nodeCurves.value.rotationX, "rotation.x");
				writeCurveJson(doc, nodeCurves.value.rotationY, "rotation.y");
				writeCurveJson(doc, nodeCurves.value.rotationZ, "rotation.z");
				writeCurveJson(doc, nodeCurves.value.scaleX, "scale.x");
				writeCurveJson(doc, nodeCurves.value.scaleY, "scale.y");
				writeCurveJson(doc, nodeCurves.value.scaleZ, "scale.z");
				writeCurveJson(doc, nodeCurves.value.visibility, "visibility");

				doc.endObject(); // node
			}

			doc.endArray(); // curves
			doc.endObject(); // layer
		}

		doc.endArray(); // layers
		doc.endDocument();
		doc.save(animFilename);
		emittedAssets.append(animFilename);
	}

	return true;
}

bool FbxProcessor::importMaterials(FbxFileImporter& fbxImporter, const String& basePath, JsonDocument& assetCfg)
{
	for (auto& mtl : fbxImporter.materials)
	{
		String fbxName = getFilenameBasename(fbxImporter.sourceFbx);
		String mtlFilename = mergePathBasenameExtension(basePath, fbxName + "_" + mtl.name, ".material");
		JsonDocument doc;
		doc.beginDocument();
		doc.addValue("version", 1);
		doc.addValue("gpuProgram", "gpu_programs/default.gpu_program");
		doc.beginObject("states");
		doc.addValue("alphaBlendState", false);
		doc.addValue("depthWriteEnable", true);
		doc.addValue("depthTestEnable", true);
		doc.addValue("fillMode", "solid");
		doc.addValue("cullMode", "ccw");
		doc.endObject();

		doc.beginObject("constants");
		doc.addValue("diffuseColor", toString(mtl.diffuse));
		
		if (mtl.diffuseMap.fileName.notEmpty())
		{
			doc.addValue("diffuseSampler", mtl.diffuseMap.fileName);
			doc.addValue("uvDiffuseRepeat", toString(mtl.diffuseMap.tiling));
			doc.addValue("uvDiffuseOffset", toString(mtl.diffuseMap.offset));
		}

		if (mtl.normalMap.fileName.notEmpty())
		{
			doc.addValue("normalSampler", mtl.normalMap.fileName);
			doc.addValue("uvNormalRepeat", toString(mtl.normalMap.tiling));
			doc.addValue("uvNormalOffset", toString(mtl.normalMap.offset));
		}

		if (mtl.specularMap.fileName.notEmpty())
		{
			doc.addValue("specularSampler", mtl.specularMap.fileName);
			doc.addValue("uvSpecularRepeat", toString(mtl.specularMap.tiling));
			doc.addValue("uvSpecularOffset", toString(mtl.specularMap.offset));
		}

		if (mtl.emissiveMap.fileName.notEmpty())
		{
			doc.addValue("emissiveSampler", mtl.emissiveMap.fileName);
			doc.addValue("uvEmissiveRepeat", toString(mtl.emissiveMap.tiling));
			doc.addValue("uvEmissiveOffset", toString(mtl.emissiveMap.offset));
		}

		doc.endObject();

		doc.save(mtlFilename);
	}

	return true;
}

}