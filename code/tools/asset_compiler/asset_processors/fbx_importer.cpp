// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "fbxsdk.h"
#include "fbx_importer.h"
#include "base/logger.h"
#include "base/assert.h"
#include "base/json.h"
#include "base/platform.h"
#include "base/util.h"
#include "base/math/util.h"
#include "base/thread.h"
#include "base/mutex.h"
#include "mesh_util.h"
#include "base/file.h"

using namespace FBXSDK_NAMESPACE;

namespace ac
{
using namespace base;

static Mutex lockFbxUsage;

#ifdef IOS_REF
#undef IOS_REF
#define IOS_REF (*(fbxMgr->GetIOSettings()))
#endif

FbxDouble3 fbxGetMaterialProperty(
	const FbxSurfaceMaterial* material,
	const char* propertyName,
	const char* factorPropertyName,
	FbxFileTexture** tex)
{
	FbxDouble3 lResult(0, 0, 0);
	const FbxProperty lProperty = material->FindProperty(propertyName);
	const FbxProperty lFactorProperty = material->FindProperty(factorPropertyName);

	if (lProperty.IsValid() && lFactorProperty.IsValid())
	{
		lResult = lProperty.Get<FbxDouble3>();
		double lFactor = lFactorProperty.Get<double>();

		if (lFactor != 1)
		{
			lResult[0] *= lFactor;
			lResult[1] *= lFactor;
			lResult[2] *= lFactor;
		}
	}

	if (lProperty.IsValid())
	{
		const int lTextureCount = lProperty.GetSrcObjectCount();

		if (lTextureCount)
		{
			*tex = lProperty.GetSrcObject<FbxFileTexture>(0);
		}
	}

	return lResult;
}

void fbxGatherNodes(FbxNode* parent, Array<FbxNode*>& nodes)
{
	nodes.append(parent);

	for (size_t i = 0; i < parent->GetChildCount(); i++)
	{
		fbxGatherNodes(parent->GetChild(i), nodes);
	}
}

FbxFileImporter::FbxFileImporter()
{
	fbxMgr = FbxManager::Create();
	// create an IOSettings object
	FbxIOSettings* ios = FbxIOSettings::Create(fbxMgr, IOSROOT);
	fbxMgr->SetIOSettings(ios);

	// Load plugins from the executable directory
	FbxString lPath = FbxGetApplicationDirectory();
#if defined(FBXSDK_ENV_WIN)
	FbxString lExtension = "fbx.dll";
#elif defined(FBXSDK_ENV_MAC)
	FbxString lExtension = "fbx.dylib";
#elif defined(FBXSDK_ENV_LINUX)
	FbxString lExtension = "fbx.so";
#endif
	fbxMgr->LoadPluginsDirectory(lPath.Buffer(), lExtension.Buffer());

	// Create the entity that will hold the scene.
	fbxScene = FbxScene::Create(fbxMgr, "");
}

FbxFileImporter::~FbxFileImporter()
{
	free();
}

bool FbxFileImporter::loadFbxScene(
	FbxManager* sdkManager,
	FbxDocument* scene,
	const String& filename)
{
	AutoLock<Mutex> locker(lockFbxUsage);

	int lFileMajor, lFileMinor, lFileRevision;
	int lSDKMajor,  lSDKMinor,  lSDKRevision;
	//int lFileFormat = -1;
	int i, lAnimStackCount;
	bool lStatus;
	char lPassword[1024];
	
	strcpy(lPassword, options.password.c_str());

	// Get the file version number generate by the FBX SDK.
	FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

	// Create an importer.
	FbxImporter* lImporter = FbxImporter::Create(sdkManager, "");

	// Initialize the importer by providing a filename.
	const bool lImportStatus =
		lImporter->Initialize(
			filename.c_str(), -1, sdkManager->GetIOSettings());
	lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

	if (!lImportStatus)
	{
		B_LOG_ERROR("Call to KFbxImporter::Initialize() failed.");
		B_LOG_ERROR("Error returned: " << lImporter->GetStatus().GetErrorString());

		if (lImporter->GetStatus() != FbxStatus::eSuccess)
		{
			B_LOG_ERROR("FBX version number for this FBX SDK is " << lSDKMajor << "." << lSDKMinor << "." << lSDKRevision);
			B_LOG_ERROR("FBX version number for file " << filename << " is " << lFileMajor << "." << lFileMinor << "." << lFileRevision);
		}

		return false;
	}

	if (lImporter->IsFBX())
	{
		// Set the import states. By default, the import states are always set to
		// true. The code below shows how to change these states.
		IOS_REF.SetBoolProp(IMP_FBX_MATERIAL, true);
		IOS_REF.SetBoolProp(IMP_FBX_TEXTURE, true);
		IOS_REF.SetBoolProp(IMP_FBX_LINK, true);
		IOS_REF.SetBoolProp(IMP_FBX_SHAPE, true);
		IOS_REF.SetBoolProp(IMP_FBX_GOBO, true);
		IOS_REF.SetBoolProp(IMP_FBX_ANIMATION, true);
		IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
		// Import the scene.
		lStatus = lImporter->Import(scene);

		FbxAxisSystem::OpenGL.ConvertScene(scene->GetScene());

		if (lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
		{
			B_LOG_ERROR("Please enter password!");

			FbxString lString(lPassword);

			IOS_REF.SetStringProp(IMP_FBX_PASSWORD, lString);
			IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

			lStatus = lImporter->Import(scene);

			if (lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
			{
				B_LOG_ERROR("Password is wrong, import aborted.");
			}
		}
	}

	// Destroy the importer.
	lImporter->Destroy();

	return lStatus;
}

bool FbxFileImporter::import(const String& fbxFilename, const FbxImportOptions& newOptions)
{
	AutoLock<Mutex> lockFbxUsage(lockFbxUsage);

	options = newOptions;
	sourceFbx = fbxFilename;

	if (!loadFbxScene(fbxMgr, fbxScene, fbxFilename))
		return false;

	B_LOG_DEBUG("Geometries found: " << fbxScene->GetGeometryCount());

	Array<FbxSurfaceMaterial*> addedMaterials;
	FbxStringList lUVNames;
	ImportMesh *importMesh = nullptr;
	
	for (size_t m = 0; m < fbxScene->GetGeometryCount(); ++m)
	{
		FbxGeometry* geometry = fbxScene->GetGeometry(m);
		bool parent = true;

		if (geometry->GetNode()->GetParent()
			&& strcmp(geometry->GetNode()->GetParent()->GetName(), "RootNode"))
		{
			B_LOG_DEBUG("Child node: '" << geometry->GetNode()->GetName() << " with parent '" << geometry->GetNode()->GetParent()->GetName() << "'");
			parent = false;
		}

		importMesh = new ImportMesh();
		B_ASSERT(importMesh);

		if (!importMesh)
			return false;

		meshes.append(importMesh);
		importMesh->info.name = geometry->GetNode()->GetName();
		importMesh->fbxNode = geometry->GetNode();
		B_LOG_DEBUG("Mesh: " << importMesh->info.name);

		FbxMesh* fbxMesh = geometry->GetNode()->GetMesh();

		i32 polygonCount = fbxMesh->GetPolygonCount();
		FbxVector4* controlPoints = fbxMesh->GetControlPoints(); 
		i32 vertexId = 0;

		B_LOG_DEBUG("Polygons: " << polygonCount);

		FbxMatrix mtx = geometry->GetNode()->EvaluateLocalTransform();
		auto geomPos = geometry->GetNode()->GetGeometricTranslation(FbxNode::eSourcePivot);

		FbxVector4 pos;
		FbxQuaternion rot;
		FbxVector4 scl;
		FbxVector4 shear;
		double sign;

		mtx.GetElements(pos, rot, shear, scl, sign);

		B_LOG_DEBUG("GeomPos: " << geomPos[0] << " " << geomPos[1] << " " << geomPos[2]);

		importMesh->transform.translation.set(pos[0], pos[1], pos[2]);
		importMesh->transform.rotation.x = rot[0];
		importMesh->transform.rotation.y = rot[1];
		importMesh->transform.rotation.z = rot[2];
		importMesh->transform.rotation.w = rot[3];
		importMesh->transform.scale.set(scl[0], scl[1], scl[2]);

		geometry->ComputeBBox();

		importMesh->bbox.min = Vec3(geometry->BBoxMin.Get()[0], geometry->BBoxMin.Get()[1], geometry->BBoxMin.Get()[2]);
		importMesh->bbox.max = Vec3(geometry->BBoxMax.Get()[0], geometry->BBoxMax.Get()[1], geometry->BBoxMax.Get()[2]);

		B_LOG_DEBUG("Position: " << importMesh->transform.translation.x << " " << importMesh->transform.translation.y << " " << importMesh->transform.translation.z);
		B_LOG_DEBUG("Rotation: " << rot[0] << " " << rot[1] << " " << rot[2]);
		B_LOG_DEBUG("Scale: " << importMesh->transform.scale.x << " " << importMesh->transform.scale.y << " " << importMesh->transform.scale.z);

		// Count the polygon count of each material
		FbxLayerElementArrayTemplate<int>* lMaterialIndices = nullptr;
		FbxGeometryElement::EMappingMode lMaterialMappingMode = FbxGeometryElement::eNone;

		if (fbxMesh->GetElementMaterial())
		{
			B_LOG_DEBUG("Found element material");
			lMaterialIndices = &fbxMesh->GetElementMaterial()->GetIndexArray();
			lMaterialMappingMode = fbxMesh->GetElementMaterial()->GetMappingMode();
			
			if (lMaterialIndices && lMaterialMappingMode == FbxGeometryElement::eByPolygon)
			{
				B_ASSERT(lMaterialIndices->GetCount() == polygonCount);

				if (lMaterialIndices->GetCount() == polygonCount)
				{
					// Count the faces of each material
					for (int lPolygonIndex = 0; lPolygonIndex < polygonCount; ++lPolygonIndex)
					{
						const int lMaterialIndex = lMaterialIndices->GetAt(lPolygonIndex);
						
						if (importMesh->elements.size() < lMaterialIndex + 1)
						{
							Array<ImportMeshElement*> elements = importMesh->elements;
							importMesh->elements.resize(lMaterialIndex + 1);
							importMesh->elements.fill(0, importMesh->elements.size(), nullptr);
							
							for (int c = 0; c < elements.size(); ++c)
							{
								importMesh->elements[c] = elements[c];
							}
						}

						B_ASSERT(lMaterialIndex >= 0);

						if (importMesh->elements[lMaterialIndex] == nullptr)
						{
							importMesh->elements[lMaterialIndex] = new ImportMeshElement();
							
							FbxSurfaceMaterial* mtl = fbxMesh->GetNode()->GetMaterial(lMaterialIndex);
							
							if (mtl && addedMaterials.end() == addedMaterials.find(mtl))
							{
								addedMaterials.append(mtl);

								FbxFileTexture* diffuseTex = nullptr;
								FbxFileTexture* normalTex = nullptr;
								FbxFileTexture* specularTex = nullptr;
								FbxFileTexture* emissiveTex = nullptr;

								fbxGetMaterialProperty(
									mtl,
									FbxSurfaceMaterial::sDiffuse,
									FbxSurfaceMaterial::sDiffuseFactor,
									&diffuseTex);

								fbxGetMaterialProperty(
									mtl,
									FbxSurfaceMaterial::sNormalMap,
									FbxSurfaceMaterial::sDiffuseFactor,
									&normalTex);

								fbxGetMaterialProperty(
									mtl,
									FbxSurfaceMaterial::sSpecular,
									FbxSurfaceMaterial::sSpecularFactor,
									&specularTex);

								fbxGetMaterialProperty(
									mtl,
									FbxSurfaceMaterial::sEmissive,
									FbxSurfaceMaterial::sEmissiveFactor,
									&emissiveTex);

								B_LOG_DEBUG(String("Mtl#") << lMaterialIndex << " " << mtl->GetName()
									<< " (Multilayer: " << (mtl->MultiLayer == 1) << ")"
									<< " Diffuse: " << (diffuseTex ? diffuseTex->GetFileName() : "<none>")
									<< " Normal: " << (normalTex ? normalTex->GetFileName() : "<none>")
									<< " Specular: " << (specularTex ? specularTex->GetFileName() : "<none>")
									<< " Emissive: " << (emissiveTex ? emissiveTex->GetFileName() : "<none>"));
							}
						}

						importMesh->elements[lMaterialIndex]->primitiveCount++;
					}

					// Record the offset (how many vertices)
					const i32 lMaterialCount = importMesh->elements.size();
					i32 lOffset = 0;

					for (i32 lIndex = 0; lIndex < lMaterialCount; ++lIndex)
					{
						B_LOG_DEBUG("Offset: " << lOffset);
						importMesh->elements[lIndex]->startIndex = lOffset;
						lOffset += importMesh->elements[lIndex]->primitiveCount * 3;
						// This will be used as counter in the following procedures, reset to zero
						importMesh->elements[lIndex]->primitiveCount = 0;
					}
					B_ASSERT(lOffset == polygonCount * 3);
				}
			}
		}

		// All faces will use the same material.
		if (importMesh->elements.isEmpty())
		{
			importMesh->elements.resize(1);
			importMesh->elements[0] = new ImportMeshElement();
			importMesh->elements[0]->indexCount = polygonCount * 3;
		}

		// Congregate all the data of a mesh to be cached in VBOs.
		// If normal or UV is by polygon vertex, record all vertex attributes by polygon vertex.
		bool hasNormal = fbxMesh->GetElementNormalCount() > 0;
		bool hasUV = fbxMesh->GetElementUVCount() > 0;
		bool hasTangents = fbxMesh->GetElementTangentCount() > 0;
		bool hasBitangents = fbxMesh->GetElementBinormalCount() > 0;
		bool hasColors = fbxMesh->GetElementVertexColorCount() > 0;

		bool allByControlPoint = true;
		FbxGeometryElement::EMappingMode lNormalMappingMode = FbxGeometryElement::eNone;
		FbxGeometryElement::EMappingMode lUVMappingMode = FbxGeometryElement::eNone;
		FbxGeometryElement::EMappingMode lTangentMappingMode = FbxGeometryElement::eNone;
		FbxGeometryElement::EMappingMode lBinormalMappingMode = FbxGeometryElement::eNone;

		if (hasNormal)
		{
			lNormalMappingMode = fbxMesh->GetElementNormal(0)->GetMappingMode();
		
			if (lNormalMappingMode == FbxGeometryElement::eNone)
			{
				hasNormal = false;
			}

			if (hasNormal && lNormalMappingMode != FbxGeometryElement::eByControlPoint)
			{
				allByControlPoint = false;
			}
		}

		if (hasUV)
		{
			lUVMappingMode = fbxMesh->GetElementUV(0)->GetMappingMode();

			if (lUVMappingMode == FbxGeometryElement::eNone)
			{
				hasUV = false;
			}
			
			if (hasUV && lUVMappingMode != FbxGeometryElement::eByControlPoint)
			{
				allByControlPoint = false;
			}
		}
		
		if (hasTangents)
		{
			lTangentMappingMode = fbxMesh->GetElementUV(0)->GetMappingMode();

			if (lTangentMappingMode == FbxGeometryElement::eNone)
			{
				hasTangents = false;
			}

			if (hasTangents && lTangentMappingMode != FbxGeometryElement::eByControlPoint)
			{
				allByControlPoint = false;
			}
		}

		if (hasBitangents)
		{
			lBinormalMappingMode = fbxMesh->GetElementUV(0)->GetMappingMode();

			if (lBinormalMappingMode == FbxGeometryElement::eNone)
			{
				hasBitangents = false;
			}

			if (hasBitangents && lBinormalMappingMode != FbxGeometryElement::eByControlPoint)
			{
				allByControlPoint = false;
			}
		}

		// Allocate the array memory, by control point or by polygon vertex.
		int lPolygonVertexCount = fbxMesh->GetControlPointsCount();

		if (!allByControlPoint)
		{
			lPolygonVertexCount = polygonCount * 3;
		}

		importMesh->geometryBuffer.indices.resize(polygonCount * 3);
		importMesh->geometryBuffer.vertices.resize(lPolygonVertexCount);

		lUVNames.Clear();
		fbxMesh->GetUVSetNames(lUVNames);

		if (lUVNames.GetCount() > maxTexCoordSets)
		{
			B_LOG_WARNING("Max number of UV sets exceeded (MaxUvSets: " << maxTexCoordSets << ", mesh has " << lUVNames.GetCount() << " sets)");
			continue;
		}
		else
		{
			B_LOG_DEBUG("# of UV sets in the mesh: " << lUVNames.GetCount());
			importMesh->geometryBuffer.texCoordSetCount = lUVNames.GetCount();
		}

		// Populate the array with vertex attribute, if by control point.
		const FbxVector4* lControlPoints = fbxMesh->GetControlPoints();
		FbxVector4 lCurrentVertex;
		FbxVector4 lCurrentNormal;
		FbxVector4 lCurrentTangent;
		FbxVector4 lCurrentBitangent;
		FbxVector2 lCurrentUV;
		FbxColor lCurrentColor;
		const FbxGeometryElementNormal* lNormalElement = nullptr;
		FbxGeometryElementUV* lUVElement[maxTexCoordSets];
		const FbxGeometryElementTangent* lTangentElement = nullptr;
		const FbxGeometryElementBinormal* lBinormalElement = nullptr;
		const FbxGeometryElementVertexColor* lColorElement = nullptr;

		for (int uvset = 0; uvset < maxTexCoordSets; ++uvset)
		{
			lUVElement[uvset] = nullptr;
		}

		if (hasNormal)
		{
			lNormalElement = fbxMesh->GetElementNormal(0);
		}

		if (hasUV)
		{
			for (int uvset = 0; uvset < minValue((int)maxTexCoordSets, lUVNames.GetCount()); ++uvset)
			{
				lUVElement[uvset] = fbxMesh->GetElementUV(lUVNames[uvset]);
			}
		}

		if (hasTangents)
		{
			lTangentElement = fbxMesh->GetElementTangent(0);

			if (lTangentElement)
			{
				B_LOG_DEBUG("Mesh has vertex tangents.");
			}
		}

		if (hasBitangents)
		{
			lBinormalElement = fbxMesh->GetElementBinormal(0);

			if (lBinormalElement)
			{
				B_LOG_DEBUG("Mesh has vertex binormals.");
			}
		}

		if (hasColors)
		{
			lColorElement = fbxMesh->GetElementVertexColor(0);

			if (lColorElement)
			{
				B_LOG_DEBUG("Mesh has vertex colors.");
			}
		}
		else
		{
			B_LOG_DEBUG("No vertex colors found.");
		}

		if (allByControlPoint)
		{
			B_LOG_DEBUG("All by control point");

			for (i32 lIndex = 0; lIndex < lPolygonVertexCount; ++lIndex)
			{
				// Save the vertex position.
				lCurrentVertex = lControlPoints[lIndex];

				ImportVertex vtx;

				vtx.position.x = -lCurrentVertex[0] + geomPos[0];
				vtx.position.y = -lCurrentVertex[1] + geomPos[1];
				vtx.position.z = -lCurrentVertex[2] + geomPos[2];

				// Save the normal.
				if (hasNormal)
				{
					i32 lNormalIndex = lIndex;

					if (lNormalElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
					{
						lNormalIndex = lNormalElement->GetIndexArray().GetAt(lIndex);
					}

					lCurrentNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);
					vtx.normal.x = lCurrentNormal[0];
					vtx.normal.y = lCurrentNormal[1];
					vtx.normal.z = lCurrentNormal[2];
				}

				// Save the UV.
				if (hasUV)
				{
					i32 lUVIndex = lIndex;

					for (i32 uvset = 0; uvset < maxTexCoordSets; ++uvset)
					{
						FbxGeometryElementUV* pUVs = lUVElement[uvset];

						if (!pUVs)
						{
							continue;
						}

						if (pUVs->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
						{
							lUVIndex = pUVs->GetIndexArray().GetAt(lIndex);
						}

						lCurrentUV = pUVs->GetDirectArray().GetAt(lUVIndex);
						vtx.texCoord[uvset].x = lCurrentUV[0];
						vtx.texCoord[uvset].y = lCurrentUV[1];
					}
				}

				if (hasTangents)
				{
					i32 lTangentIndex = lIndex;

					if (lTangentElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
					{
						lTangentIndex = lTangentElement->GetIndexArray().GetAt(lIndex);
					}

					lCurrentTangent = lTangentElement->GetDirectArray().GetAt(lTangentIndex);
					vtx.tangent.x = lCurrentTangent[0];
					vtx.tangent.y = lCurrentTangent[1];
					vtx.tangent.z = lCurrentTangent[2];
				}

				if (hasBitangents)
				{
					i32 lBinormalIndex = lIndex;

					if (lBinormalElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
					{
						lBinormalIndex = lBinormalElement->GetIndexArray().GetAt(lIndex);
					}

					lCurrentBitangent = lBinormalElement->GetDirectArray().GetAt(lBinormalIndex);
					vtx.bitangent.x = lCurrentBitangent[0];
					vtx.bitangent.y = lCurrentBitangent[1];
					vtx.bitangent.z = lCurrentBitangent[2];
				}

				if (hasColors)
				{
					i32 lColorIndex = lIndex;

					if (lColorElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
					{
						lColorIndex = lColorElement->GetIndexArray().GetAt(lIndex);
					}

					lCurrentColor = lColorElement->GetDirectArray().GetAt(lColorIndex);
					vtx.color.r = lCurrentColor.mRed;
					vtx.color.g = lCurrentColor.mGreen;
					vtx.color.b = lCurrentColor.mBlue;
					vtx.color.a = lCurrentColor.mAlpha;
				}

				importMesh->geometryBuffer.vertices[lIndex] = vtx;
			}
		}
		else
		{
			B_LOG_DEBUG("Data is By Polygon Vertex");

			if (hasColors)
			{
				lColorElement = fbxMesh->GetElementVertexColor(0);

				if (lColorElement)
				{
					B_LOG_DEBUG("Mesh has vertex colors.");
				}
			}
			else
			{
				B_LOG_DEBUG("No vertex colors found.");
			}
		}

		i32 lVertexCount = 0;

		for (i32 lPolygonIndex = 0; lPolygonIndex < polygonCount; ++lPolygonIndex)
		{
			// The material for current face.
			i32 lMaterialIndex = 0;

			if (lMaterialIndices && lMaterialMappingMode == FbxGeometryElement::eByPolygon)
			{
				lMaterialIndex = lMaterialIndices->GetAt(lPolygonIndex);
			}

			// Where should I save the vertex attribute index, according to the material
			const i32 lIndexOffset = importMesh->elements[lMaterialIndex]->startIndex + importMesh->elements[lMaterialIndex]->primitiveCount * 3;

			for (i32 lVerticeIndex = 0; lVerticeIndex < 3; ++lVerticeIndex)
			{
				const i32 lControlPointIndex = fbxMesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex);

				if (allByControlPoint)
				{
					importMesh->geometryBuffer.indices[lIndexOffset + lVerticeIndex] = static_cast<unsigned int>(lControlPointIndex);
				}
				// Populate the array with vertex attribute, if by polygon vertex.
				else
				{
					importMesh->geometryBuffer.indices[lIndexOffset + lVerticeIndex] = static_cast<unsigned int>(lVertexCount);

					lCurrentVertex = lControlPoints[lControlPointIndex];
					
					importMesh->geometryBuffer.vertices[lVertexCount].position.x = static_cast<f32>(lCurrentVertex[0]) + geomPos[0];
					importMesh->geometryBuffer.vertices[lVertexCount].position.y = static_cast<f32>(lCurrentVertex[1]) + geomPos[1];
					importMesh->geometryBuffer.vertices[lVertexCount].position.z = static_cast<f32>(lCurrentVertex[2]) + geomPos[2];

					if (hasNormal)
					{
						fbxMesh->GetPolygonVertexNormal(lPolygonIndex, lVerticeIndex, lCurrentNormal);
						importMesh->geometryBuffer.vertices[lVertexCount].normal.x = static_cast<f32>(lCurrentNormal[0]);
						importMesh->geometryBuffer.vertices[lVertexCount].normal.y = static_cast<f32>(lCurrentNormal[1]);
						importMesh->geometryBuffer.vertices[lVertexCount].normal.z = static_cast<f32>(lCurrentNormal[2]);
					}

					if (hasTangents)
					{
						i32 lTangentIndex = lControlPointIndex;

						if (lTangentElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
						{
							lTangentIndex = lTangentElement->GetIndexArray().GetAt(lTangentIndex);
						}

						lCurrentTangent = lTangentElement->GetDirectArray().GetAt(lVertexCount);
						auto& tan = importMesh->geometryBuffer.vertices[lVertexCount].tangent;
						tan.x = lCurrentTangent[0];
						tan.y = lCurrentTangent[1];
						tan.z = lCurrentTangent[2];
					}

					if (hasBitangents)
					{
						i32 lBinormalIndex = lControlPointIndex;

						if (lBinormalElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
						{
							lBinormalIndex = lBinormalElement->GetIndexArray().GetAt(lBinormalIndex);
						}

						lCurrentBitangent = lBinormalElement->GetDirectArray().GetAt(lVertexCount);
						auto& bitan = importMesh->geometryBuffer.vertices[lVertexCount].bitangent;
						bitan.x = lCurrentBitangent[0];
						bitan.y = lCurrentBitangent[1];
						bitan.z = lCurrentBitangent[2];
					}

					if (hasUV)
					{
						bool bUnmapped;

						for (i32 uvset = 0; uvset < lUVNames.GetCount(); ++uvset)
						{
							fbxMesh->GetPolygonVertexUV(lPolygonIndex, lVerticeIndex, lUVNames[uvset], lCurrentUV, bUnmapped);
							importMesh->geometryBuffer.vertices[lVertexCount].texCoord[uvset].x = static_cast<f32>(lCurrentUV[0]);
							importMesh->geometryBuffer.vertices[lVertexCount].texCoord[uvset].y = static_cast<f32>(lCurrentUV[1]);
						}
					}

					if (hasColors)
					{
						i32 lColorIndex = lControlPointIndex;

						if (lColorElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
						{
							lColorIndex = lColorElement->GetIndexArray().GetAt(lControlPointIndex);
						}

						lCurrentColor = lColorElement->GetDirectArray().GetAt(lColorIndex);
						importMesh->geometryBuffer.vertices[lVertexCount].color.r = lCurrentColor.mRed;
						importMesh->geometryBuffer.vertices[lVertexCount].color.g = lCurrentColor.mGreen;
						importMesh->geometryBuffer.vertices[lVertexCount].color.b = lCurrentColor.mBlue;
						importMesh->geometryBuffer.vertices[lVertexCount].color.a = lCurrentColor.mAlpha;
					}
				}

				++lVertexCount;
			}

			importMesh->elements[lMaterialIndex]->primitiveCount++;
		}

		for (size_t p = 0; p < importMesh->elements.size(); ++p)
		{
			importMesh->elements[p]->indexCount = importMesh->elements[p]->primitiveCount * 3;
		}
	}

	// animations
	auto rootNode = fbxScene->GetRootNode();
	Array<FbxNode*> nodes;

	fbxGatherNodes(rootNode, nodes);

	int numStacks = fbxScene->GetSrcObjectCount<FbxAnimStack>();

	B_LOG_DEBUG("Fbx animation stacks: " << numStacks);
	B_LOG_INDENT

	for (size_t i = 0; i < numStacks; i++)
	{
		ImportAnimationClip anim;

		FbxAnimStack* animStack = fbxScene->GetSrcObject<FbxAnimStack>(i);
		B_LOG_DEBUG("Fbx animation stack #" << i);
		B_LOG_INDENT

		int numAnimLayers = animStack->GetMemberCount<FbxAnimLayer>();
		auto animTimeSpan = animStack->GetLocalTimeSpan();
		FbxTime timePerFrame;

		timePerFrame.SetTime(0, 0, 0, 1, 0, fbxScene->GetGlobalSettings().GetTimeMode());
		const FbxTime startTime = animTimeSpan.GetStart();
		const FbxTime endTime = animTimeSpan.GetStop();

		const f32 startTimeSeconds = static_cast<f32>(startTime.GetSecondDouble());
		const f32 endTimeSeconds = static_cast<f32>(endTime.GetSecondDouble());

		anim.startTime = startTimeSeconds;
		anim.endTime = endTimeSeconds;
		anim.name = animStack->GetNameOnly();

		B_LOG_INFO("Animation range: " << startTimeSeconds << "-" << endTimeSeconds);

		B_LOG_DEBUG("Fbx animation stack layers " << numAnimLayers);
		B_LOG_INDENT

		for (size_t j = 0; j < numAnimLayers; j++)
		{
			ImportAnimationLayer layer;

			FbxAnimLayer* animLayer = animStack->GetMember<FbxAnimLayer>(j);
			B_LOG_DEBUG("Nodes " << nodes.size());

			for (auto node : nodes)
			{
				B_LOG_DEBUG("Node " << (const char*)node->GetNameOnly() << " keys:");
				B_LOG_DEBUG("Euler rotation order: " << node->RotationOrder);

				FbxAnimCurve* animCurveTransX = node->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X);
				FbxAnimCurve* animCurveTransY = node->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y);
				FbxAnimCurve* animCurveTransZ = node->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z);
				FbxAnimCurve* animCurveRotX = node->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X);
				FbxAnimCurve* animCurveRotY = node->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y);
				FbxAnimCurve* animCurveRotZ = node->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z);
				FbxAnimCurve* animCurveSclX = node->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X);
				FbxAnimCurve* animCurveSclY = node->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y);
				FbxAnimCurve* animCurveSclZ = node->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z);
				FbxAnimCurve* animCurveVis = node->Visibility.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X);

				if (animCurveTransX || animCurveTransY || animCurveTransZ
					|| animCurveRotX || animCurveRotY || animCurveRotZ
					|| animCurveSclX || animCurveSclY || animCurveSclZ
					|| animCurveVis)
				{
					ImportAnimationLayerNodeCurves curves;

					importAnimationCurve(animCurveTransX, curves.translationX, "Translation.X");
					importAnimationCurve(animCurveTransY, curves.translationY, "Translation.Y");
					importAnimationCurve(animCurveTransZ, curves.translationZ, "Translation.Z");
					importAnimationCurve(animCurveRotX, curves.rotationX, "Rotation.X");
					importAnimationCurve(animCurveRotY, curves.rotationY, "Rotation.Y");
					importAnimationCurve(animCurveRotZ, curves.rotationZ, "Rotation.Z");
					importAnimationCurve(animCurveSclX, curves.scaleX, "Scale.X");
					importAnimationCurve(animCurveSclY, curves.scaleY, "Scale.Y");
					importAnimationCurve(animCurveSclZ, curves.scaleZ, "Scale.Z");
					importAnimationCurve(animCurveVis, curves.visibility, "Visibility");
					
					if (curves.translationX.keys.notEmpty()
						|| curves.translationY.keys.notEmpty()
						|| curves.translationZ.keys.notEmpty()
						|| curves.rotationX.keys.notEmpty()
						|| curves.rotationY.keys.notEmpty()
						|| curves.rotationZ.keys.notEmpty()
						|| curves.scaleX.keys.notEmpty()
						|| curves.scaleY.keys.notEmpty()
						|| curves.scaleZ.keys.notEmpty()
						|| curves.visibility.keys.notEmpty())
					{
						layer.nodeCurves[findMeshByFbxNode(node)] = curves;
					}
					else
					{
						B_LOG_DEBUG("No keys for this layer");
					}
				}
				else
				{
					B_LOG_DEBUG("No curves for this layer");
				}
			}
		
			anim.layers.append(layer);
		}

		animations.append(anim);

		B_LOG_UNINDENT
		B_LOG_UNINDENT
	}

	B_LOG_UNINDENT

	u32 materialCount = fbxScene->GetMaterialCount();

	B_LOG_INFO("Materials: " << materialCount);

	for (u32 i = 0; i < materialCount; i++)
	{
		auto fbxMtl = fbxScene->GetMaterial(i);
		ImportMaterial mtl;

		mtl.name = fbxMtl->GetNameOnly();
		B_LOG_INFO("Material: " << mtl.name);
		B_LOG_INFO(" type: " << fbxMtl->ClassId.GetName());

		auto getMtlPropColorValue = [](FbxSurfaceMaterial* mtl, const char* prop, const char* factorProp)
		{
			FbxDouble3 result(0, 0, 0);
			const FbxProperty lProperty = mtl->FindProperty(prop);
			const FbxProperty lFactorProperty = mtl->FindProperty(factorProp);
			
			if (lProperty.IsValid() && lFactorProperty.IsValid())
			{
				result = lProperty.Get<FbxDouble3>();
				double lFactor = lFactorProperty.Get<FbxDouble>();

				if (lFactor != 1) {
					result[0] *= lFactor;
					result[1] *= lFactor;
					result[2] *= lFactor;
				}
			}
			
			Color tcolor;
			
			tcolor.set(result[0], result[1], result[2]);
			
			return tcolor;
		};

		auto getMtlPropFloatValue = [](FbxSurfaceMaterial* mtl, const char* prop)
		{
			FbxDouble result = 0;
			const FbxProperty lProperty = mtl->FindProperty(prop);

			if (lProperty.IsValid())
			{
				result = lProperty.Get<FbxDouble>();
			}

			return result;
		};

		auto getMtlPropMapValue = [](FbxSurfaceMaterial* mtl, const char* prop)
		{
			FbxTexturePtr result = nullptr;
			const FbxProperty lProperty = mtl->FindProperty(prop);

			if (lProperty.IsValid())
			{
				result = lProperty.GetSrcObject<FbxTexture>();
			}

			return result;
		};

		mtl.diffuse = getMtlPropColorValue(fbxMtl, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor);
		mtl.ambient = getMtlPropColorValue(fbxMtl, FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor);
		mtl.specular = getMtlPropColorValue(fbxMtl, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor);
		mtl.shininess = getMtlPropFloatValue(fbxMtl, FbxSurfaceMaterial::sShininess);
		mtl.opacity = getMtlPropFloatValue(fbxMtl, FbxSurfaceMaterial::sTransparencyFactor);
		
		auto diffuseTex = getMtlPropMapValue(fbxMtl, FbxSurfaceMaterial::sDiffuse);
		auto normalTex = getMtlPropMapValue(fbxMtl, FbxSurfaceMaterial::sNormalMap);
		auto emissiveTex = getMtlPropMapValue(fbxMtl, FbxSurfaceMaterial::sEmissive);
		auto specularTex = getMtlPropMapValue(fbxMtl, FbxSurfaceMaterial::sSpecularFactor);
		auto reflectionTex = getMtlPropMapValue(fbxMtl, FbxSurfaceMaterial::sReflection);

		auto fillTextureMap = [](FbxTexturePtr tex, ImportTexture& myTex)
		{
			if (tex)
			{
				if (tex->GetClassId() == FbxFileTexture::ClassId)
				{
					FbxFileTexture* fileTex = FbxCast<FbxFileTexture>(tex);
					myTex.fileName = beautifyPath(fileTex->GetRelativeFileName());
					myTex.tiling.set(fileTex->GetScaleU(), fileTex->GetScaleV(), 1);
					myTex.offset.set(fileTex->GetTranslationU(), fileTex->GetTranslationV(), 0);
				}
			}
		};

		fillTextureMap(diffuseTex, mtl.diffuseMap);
		fillTextureMap(normalTex, mtl.normalMap);
		fillTextureMap(emissiveTex, mtl.emissiveMap);
		fillTextureMap(specularTex, mtl.specularMap);
		fillTextureMap(reflectionTex, mtl.reflectionMap);

		B_LOG_DEBUG("\tDiffuse: " << toString(mtl.diffuse));
		B_LOG_DEBUG("\tAmbient: " << toString(mtl.ambient));
		B_LOG_DEBUG("\tSpecular: " << toString(mtl.specular));
		B_LOG_DEBUG("\tShininess: " << toString(mtl.shininess));

		if (fbxMtl->ClassId == FbxSurfacePhong::ClassId)
		{
			FbxSurfacePhong* phongMtl = (FbxSurfacePhong*)fbxMtl;
		}
		else if (fbxMtl->ClassId == FbxSurfaceLambert::ClassId)
		{
			FbxSurfaceLambert* lambertMtl = (FbxSurfaceLambert*)fbxMtl;
		}

		materials.append(mtl);
	}

	B_LOG_DEBUG("Fbx import done. " << meshes.size() << " meshes");

	return true;
}

ImportMesh* FbxFileImporter::findMeshByFbxNode(FbxNode* node)
{
	for (size_t i = 0; i < meshes.size(); i++)
	{
		if (meshes[i]->fbxNode == node)
		{
			return meshes[i];
		}
	}

	return nullptr;
}

bool FbxFileImporter::importAnimationCurve(FbxAnimCurve* fbxCurve, ImportAnimationCurve& curve, const String& curveName)
{
	if (fbxCurve)
	{
		int keyCount = fbxCurve->KeyGetCount();

		B_LOG_DEBUG(curveName << " key count: " << keyCount);
		B_LOG_INDENT
		for (int count = 0; count < keyCount; count++)
		{
			ImportAnimationKey key;
			auto& fbxKey = fbxCurve->KeyGet(count);
			
			key.time = fbxKey.GetTime().GetSecondDouble();
			key.value = fbxKey.GetValue();
			
			if (fbxKey.GetTangentMode() == FbxAnimCurveDef::ETangentMode::eTangentAuto)
			{
				//TODO
			}
			else if (fbxKey.GetTangentMode() == FbxAnimCurveDef::ETangentMode::eTangentTCB)
			{
				key.isTCB = true;
				key.tens = fbxKey.GetDataFloat(FbxAnimCurveDef::eTCBTension);
				key.cont = fbxKey.GetDataFloat(FbxAnimCurveDef::eTCBContinuity);
				key.bias = fbxKey.GetDataFloat(FbxAnimCurveDef::eTCBBias);
			}

			curve.keys.append(key);

			B_LOG_DEBUG("key[" << count << "]: time:" << key.time << " value: " << key.value);
		}
		B_LOG_UNINDENT

		if (keyCount)
		{
			return true;
		}
	}

	return false;
}

void FbxFileImporter::free()
{
	if (fbxScene && fbxMgr)
	{
		fbxScene->Clear();

		for (size_t i = 0; i < meshes.size(); ++i)
		{
			delete meshes[i];
		}

		meshes.clear();
		animations.clear();
		materials.clear();

		if (fbxScene)
		{
			fbxScene->Destroy();
			fbxScene = nullptr;
		}

		if (fbxMgr)
		{
			fbxMgr->Destroy();
			fbxMgr = nullptr;
		}
	}
}

}