#include "fbxsdk.h"
#include "fbx_importer.h"
#include "base/logger.h"
#include "base/assert.h"
#include "base/json.h"
#include "base/platform.h"
#include "base/util.h"
#include "base/math/util.h"
#include "base/thread.h"
#include "base/rmutex.h"

namespace ac
{
using namespace base;

RMutex s_lockFbxUsage;

#ifdef IOS_REF
#undef IOS_REF
#define IOS_REF (*(m_pFbxMgr->GetIOSettings()))
#endif

FbxDouble3 fbxGetMaterialProperty(const FbxSurfaceMaterial* pMaterial,
	const char* pPropertyName,
	const char* pFactorPropertyName,
	FbxFileTexture** pTex)
{
	FbxDouble3 lResult(0, 0, 0);
	const FbxProperty lProperty = pMaterial->FindProperty(pPropertyName);
	const FbxProperty lFactorProperty = pMaterial->FindProperty(pFactorPropertyName);

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
		const int lTextureCount = lProperty.GetSrcObjectCount(FbxFileTexture::ClassId);

		if (lTextureCount)
		{
			*pTex = lProperty.GetSrcObject(FBX_TYPE(FbxFileTexture), 0);
		}
	}

	return lResult;
}

FbxFileImporter::FbxFileImporter()
{
	m_pFbxMgr = FbxManager::Create();
	// create an IOSettings object
	FbxIOSettings *ios = FbxIOSettings::Create(m_pFbxMgr, IOSROOT);
	m_pFbxMgr->SetIOSettings(ios);

	// Load plugins from the executable directory
	FbxString lPath = FbxGetApplicationDirectory();
#if defined(FBXSDK_ENV_WIN)
	FbxString lExtension = "fbx.dll";
#elif defined(FBXSDK_ENV_MAC)
	FbxString lExtension = "fbx.dylib";
#elif defined(FBXSDK_ENV_LINUX)
	FbxString lExtension = "fbx.so";
#endif
	m_pFbxMgr->LoadPluginsDirectory(lPath.Buffer(), lExtension.Buffer());

	// Create the entity that will hold the scene.
	m_pFbxScene = FbxScene::Create(m_pFbxMgr, "");
}

FbxFileImporter::~FbxFileImporter()
{
	free();
	m_pFbxScene->Destroy();
	m_pFbxMgr->Destroy();
}

bool FbxFileImporter::loadFbxScene(FbxManager* pSdkManager, FbxDocument* pScene, const String& filename)
{
	AutoLock<RMutex> lockFbxUsage(s_lockFbxUsage);

	int lFileMajor, lFileMinor, lFileRevision;
	int lSDKMajor,  lSDKMinor,  lSDKRevision;
	//int lFileFormat = -1;
	int i, lAnimStackCount;
	bool lStatus;
	char lPassword[1024];
	
	strcpy(lPassword, m_options.password.c_str());

	// Get the file version number generate by the FBX SDK.
	FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

	// Create an importer.
	FbxImporter* lImporter = FbxImporter::Create(pSdkManager, "");

	// Initialize the importer by providing a filename.
	const bool lImportStatus = lImporter->Initialize(filename.c_str(), -1, pSdkManager->GetIOSettings());
	lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

	if (!lImportStatus)
	{
		B_LOG_ERROR("Call to KFbxImporter::Initialize() failed.");
		B_LOG_ERROR("Error returned: %s", lImporter->GetStatus().GetErrorString());

		if (lImporter->GetStatus() != FbxStatus::eSuccess)
		{
			B_LOG_ERROR("FBX version number for this FBX SDK is %d.%d.%d",
				lSDKMajor, lSDKMinor, lSDKRevision);
			B_LOG_ERROR("FBX version number for file %s is %d.%d.%d",
				filename.c_str(), lFileMajor, lFileMinor, lFileRevision);
		}

		return false;
	}

	B_LOG_INFO("FBX version number for this FBX SDK is %d.%d.%d", lSDKMajor, lSDKMinor, lSDKRevision);

	if (lImporter->IsFBX())
	{
		B_LOG_INFO("FBX version number for file %s is %d.%d.%d", 
			filename.c_str(), lFileMajor, lFileMinor, lFileRevision);

		// From this point, it is possible to access animation stack information without
		// the expense of loading the entire file.

		B_LOG_INFO("Animation Stack Information");

		lAnimStackCount = lImporter->GetAnimStackCount();

		B_LOG_INFO("    Number of Animation Stacks: %d", lAnimStackCount);
		B_LOG_INFO("    Current Animation Stack: \"%s\"", lImporter->GetActiveAnimStackName().Buffer());
		B_LOG_INFO("");

		for (i = 0; i < lAnimStackCount; i++)
		{
			FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

			B_LOG_INFO("    Animation Stack %d", i);
			B_LOG_INFO("         Name: \"%s\"", lTakeInfo->mName.Buffer());
			B_LOG_INFO("         Description: \"%s\"", lTakeInfo->mDescription.Buffer());

			// Change the value of the import name if the animation stack should be imported
			// under a different name.
			B_LOG_INFO("         Import Name: \"%s\"", lTakeInfo->mImportName.Buffer());

			// Set the value of the import state to false if the animation stack should be not
			// be imported.
			B_LOG_INFO("         Import State: %s", lTakeInfo->mSelect ? "true" : "false");
			B_LOG_INFO("");
		}

		// Set the import states. By default, the import states are always set to
		// true. The code below shows how to change these states.
		IOS_REF.SetBoolProp(IMP_FBX_MATERIAL, true);
		IOS_REF.SetBoolProp(IMP_FBX_TEXTURE, true);
		IOS_REF.SetBoolProp(IMP_FBX_LINK, true);
		IOS_REF.SetBoolProp(IMP_FBX_SHAPE, true);
		IOS_REF.SetBoolProp(IMP_FBX_GOBO, true);
		IOS_REF.SetBoolProp(IMP_FBX_ANIMATION, true);
		IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
	}

	// Import the scene.
	lStatus = lImporter->Import(pScene);

	if (lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
	{
		B_LOG_ERROR("Please enter password!");

		FbxString lString(lPassword);

		IOS_REF.SetStringProp(IMP_FBX_PASSWORD, lString);
		IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

		lStatus = lImporter->Import(pScene);

		if (lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
		{
			B_LOG_ERROR("Password is wrong, import aborted.");
		}
	}

	// Destroy the importer.
	lImporter->Destroy();

	return lStatus;
}

bool FbxFileImporter::import(const char* pSrcFilename, const char* pDestPath, const ArgsParser& rArgs)
{
	m_bExportMaterials = !rArgs.hasSwitch("no-materials");
	m_bExportAnimations = !rArgs.hasSwitch("no-animations");
	m_bExportLods = !rArgs.hasSwitch("no-lods");
	m_bExportHulls = !rArgs.hasSwitch("no-hulls");
	m_bExportModels = !rArgs.hasSwitch("no-models");
	m_bExportMeshes = !rArgs.hasSwitch("no-meshes");
	m_bSingleMesh = rArgs.hasSwitch("single-mesh", true);
	m_bExportSingleLodMaterial = rArgs.hasSwitch("single-lod-material", false);
	m_lodStartDistance = (f32)atof(rArgs.argValue("lod-start", "0").c_str());
	m_lodStep = (f32)atof(rArgs.argValue("lod-step", "50").c_str());
	m_assetPassword = rArgs.argValue("fbx-password", "").c_str();
	m_destPath = pDestPath;

{
	AutoLock<Mutex> lockFbxUsage(s_lockFbxUsage);

	if (!loadFbxScene(m_pFbxMgr, m_pFbxScene, pSrcFilename))
		return false;

	E_LOG_VERBOSE("Geometries found: %d", m_pFbxScene->GetGeometryCount());

	Map<MeshPart*, String> partParentNameMap;
	Map<String, MeshPart*> namePartMap;
	Array<FbxSurfaceMaterial*> addedMaterials;
	FbxStringList lUVNames;
	Mesh *pAcMesh = nullptr;
	
	for (size_t m = 0; m < m_pFbxScene->GetGeometryCount(); ++m)
	{
		FbxGeometry* pGeom = m_pFbxScene->GetGeometry(m);
		bool bParent = true;

		if (pGeom->GetNode()->GetParent()
			&& strcmp(pGeom->GetNode()->GetParent()->GetName(), "RootNode"))
		{
			E_LOG_VERBOSE("Child node: '%s' with parent '%s'",
				pGeom->GetNode()->GetName(),
				pGeom->GetNode()->GetParent()->GetName());
			bParent = false;
		}

		if (bParent)
		{
			pAcMesh = new Mesh();
			B_ASSERT(pAcMesh);

			if (!pAcMesh)
				return false;

			m_meshes.append(pAcMesh);
			pAcMesh->m_name = pGeom->GetNode()->GetName();
		}

		MeshPart* pPart = new MeshPart();
		GeometryBuffer* pGeomBuf = new GeometryBuffer();

		pPart->m_name = pGeom->GetNode()->GetName();
		namePartMap[pPart->m_name] = pPart;

		if (pGeom->GetNode()->GetParent())
		{
			partParentNameMap[pPart] = pGeom->GetNode()->GetParent()->GetName();
		}

		pAcMesh->m_parts.append(pPart);
		pAcMesh->m_geometryBuffers.append(pGeomBuf);

		E_LOG_VERBOSE("Mesh part: %s", pPart->m_name.c_str());

		FbxMesh* pMesh = pGeom->GetNode()->GetMesh();

		int polygonCount = pMesh->GetPolygonCount();
		FbxVector4* pControlPoints = pMesh->GetControlPoints(); 
		int vertexId = 0;

		E_LOG_VERBOSE("Polygons: %d", polygonCount);

		FbxMatrix mtx = pGeom->GetNode()->EvaluateLocalTransform();
		auto geomPos = pGeom->GetNode()->GetGeometricTranslation(FbxNode::eSourcePivot);

		FbxVector4 pos;
		FbxQuaternion rot;
		FbxVector4 scl;
		FbxVector4 shear;
		double sign;

		mtx.GetElements(pos, rot, shear, scl, sign);

		E_LOG_VERBOSE("GeomPos: %f %f %f", geomPos[0], geomPos[1], geomPos[2]);

		pPart->m_translation.set(pos[0], pos[1], pos[2]);
		pPart->m_rotation.x = rot[0];
		pPart->m_rotation.y = rot[1];
		pPart->m_rotation.z = rot[2];
		pPart->m_rotation.w = rot[3];
		pPart->m_scale.set(scl[0], scl[1], scl[2]);

		pGeom->ComputeBBox();

		pPart->m_bbox.m_min = Vec3(pGeom->BBoxMin.Get()[0], pGeom->BBoxMin.Get()[1], pGeom->BBoxMin.Get()[2]);
		pPart->m_bbox.m_max = Vec3(pGeom->BBoxMax.Get()[0], pGeom->BBoxMax.Get()[1], pGeom->BBoxMax.Get()[2]);

		E_LOG_VERBOSE("Position: %f %f %f", pPart->m_translation.x, pPart->m_translation.y, pPart->m_translation.z);
		E_LOG_VERBOSE("Rotation: %f %f %f", rot[0], rot[1], rot[2]);
		E_LOG_VERBOSE("Scale: %f %f %f", pPart->m_scale.x, pPart->m_scale.y, pPart->m_scale.z);

		// Count the polygon count of each material
		FbxLayerElementArrayTemplate<int>* lMaterialIndices = nullptr;
		FbxGeometryElement::EMappingMode lMaterialMappingMode = FbxGeometryElement::eNone;

		if (pMesh->GetElementMaterial())
		{
			E_LOG_VERBOSE("Found element material");
			lMaterialIndices = &pMesh->GetElementMaterial()->GetIndexArray();
			lMaterialMappingMode = pMesh->GetElementMaterial()->GetMappingMode();
			
			if (lMaterialIndices && lMaterialMappingMode == FbxGeometryElement::eByPolygon)
			{
				B_ASSERT(lMaterialIndices->GetCount() == polygonCount);

				if (lMaterialIndices->GetCount() == polygonCount)
				{
					// Count the faces of each material
					for (int lPolygonIndex = 0; lPolygonIndex < polygonCount; ++lPolygonIndex)
					{
						const int lMaterialIndex = lMaterialIndices->GetAt(lPolygonIndex);
						
						if (pPart->m_clusters.size() < lMaterialIndex + 1)
						{
							Array<MeshCluster*> clusters = pPart->m_clusters;
							pPart->m_clusters.resize(lMaterialIndex + 1);
							pPart->m_clusters.fill(0, pPart->m_clusters.size(), nullptr);
							
							for (int c = 0; c < clusters.size(); ++c)
							{
								pPart->m_clusters[c] = clusters[c];
							}
						}

						B_ASSERT(lMaterialIndex >= 0);

						if (pPart->m_clusters[lMaterialIndex] == nullptr)
						{
							pPart->m_clusters[lMaterialIndex] = new MeshCluster();
							pPart->m_clusters[lMaterialIndex]->m_pGeomBuffer = pGeomBuf;
							
							FbxSurfaceMaterial* pMtl = pMesh->GetNode()->GetMaterial(lMaterialIndex);
							
							if (pMtl && addedMaterials.end() == addedMaterials.find(pMtl))
							{
								addedMaterials.append(pMtl);

								FbxFileTexture* pDiffuseTex = nullptr;
								FbxFileTexture* pNormalTex = nullptr;
								FbxFileTexture* pSpecularTex = nullptr;
								FbxFileTexture* pEmissiveTex = nullptr;

								fbxGetMaterialProperty(
									pMtl,
									FbxSurfaceMaterial::sDiffuse,
									FbxSurfaceMaterial::sDiffuseFactor,
									&pDiffuseTex);

								fbxGetMaterialProperty(
									pMtl,
									FbxSurfaceMaterial::sNormalMap,
									FbxSurfaceMaterial::sDiffuseFactor,
									&pNormalTex);

								fbxGetMaterialProperty(
									pMtl,
									FbxSurfaceMaterial::sSpecular,
									FbxSurfaceMaterial::sSpecularFactor,
									&pSpecularTex);

								fbxGetMaterialProperty(
									pMtl,
									FbxSurfaceMaterial::sEmissive,
									FbxSurfaceMaterial::sEmissiveFactor,
									&pEmissiveTex);

								E_LOG_VERBOSE("Mtl#%d is %s (Multi?:%d) textures: "
									"Diffuse: %s Normal: %s Specular: %s Emissive: %s",
									lMaterialIndex,
									pMtl->GetName(),
									pMtl->MultiLayer == 1,
									pDiffuseTex ? pDiffuseTex->GetFileName() : "<none>",
									pNormalTex ? pNormalTex->GetFileName() : "<none>",
									pSpecularTex ? pSpecularTex->GetFileName() : "<none>",
									pEmissiveTex ? pEmissiveTex->GetFileName() : "<none>");

								Material* pAcMtl = new Material();

								pAcMtl->m_name = pMtl->GetName();
								pAcMtl->m_pHandle = pMtl;
								pAcMtl->m_gpuProgram = "gpu_program/default.gpu_program";
								
								if (pDiffuseTex)
								{
									pAcMtl->m_textureFiles["diffuseMap"] = pDiffuseTex->GetFileName();
								}

								if (pNormalTex)
								{
									pAcMtl->m_textureFiles["normalMap"] = pNormalTex->GetFileName();
								}
								
								if (pSpecularTex)
								{
									pAcMtl->m_textureFiles["specularMap"] = pSpecularTex->GetFileName();
								}

								if (pEmissiveTex)
								{
									pAcMtl->m_textureFiles["emissiveMap"] = pEmissiveTex->GetFileName();
								}

								pAcMesh->m_materials.append(pAcMtl);
								pPart->m_clusters[lMaterialIndex]->m_pMaterial = pAcMtl;
							}
						}

						pPart->m_clusters[lMaterialIndex]->m_primitiveCount++;
					}

					// Record the offset (how many vertices)
					const int lMaterialCount = pPart->m_clusters.size();
					int lOffset = 0;

					for (int lIndex = 0; lIndex < lMaterialCount; ++lIndex)
					{
						pPart->m_clusters[lIndex]->m_startIndex = lOffset;
						lOffset += pPart->m_clusters[lIndex]->m_primitiveCount * 3;
						// This will be used as counter in the following procedures, reset to zero
						pPart->m_clusters[lIndex]->m_primitiveCount = 0;
					}
					B_ASSERT(lOffset == polygonCount * 3);
				}
			}
		}

		// All faces will use the same material.
		if (pPart->m_clusters.isEmpty())
		{
			pPart->m_clusters.resize(1);
			pPart->m_clusters[0] = new MeshCluster();
			pPart->m_clusters[0]->m_pMaterial = new Material();
			pPart->m_clusters[0]->m_pGeomBuffer = pGeomBuf;
			pPart->m_clusters[0]->m_indexCount = polygonCount * 3;
			//pPart->m_clusters[0]->m_primitiveCount = polygonCount;
		}

		// Congregate all the data of a mesh to be cached in VBOs.
		// If normal or UV is by polygon vertex, record all vertex attributes by polygon vertex.
		bool mHasNormal = pMesh->GetElementNormalCount() > 0;
		bool mHasUV = pMesh->GetElementUVCount() > 0;
		bool mHasTangents = pMesh->GetElementTangentCount() > 0;
		bool mHasBitangents = pMesh->GetElementBinormalCount() > 0;
		bool mHasColors = pMesh->GetElementVertexColorCount() > 0;

		bool mAllByControlPoint = true;
		FbxGeometryElement::EMappingMode lNormalMappingMode = FbxGeometryElement::eNone;
		FbxGeometryElement::EMappingMode lUVMappingMode = FbxGeometryElement::eNone;

		if (mHasNormal)
		{
			lNormalMappingMode = pMesh->GetElementNormal(0)->GetMappingMode();
		
			if (lNormalMappingMode == FbxGeometryElement::eNone)
			{
				mHasNormal = false;
			}

			if (mHasNormal && lNormalMappingMode != FbxGeometryElement::eByControlPoint)
			{
				mAllByControlPoint = false;
			}
		}

		if (mHasUV)
		{
			lUVMappingMode = pMesh->GetElementUV(0)->GetMappingMode();

			if (lUVMappingMode == FbxGeometryElement::eNone)
			{
				mHasUV = false;
			}
			
			if (mHasUV && lUVMappingMode != FbxGeometryElement::eByControlPoint)
			{
				mAllByControlPoint = false;
			}
		}
		
		// Allocate the array memory, by control point or by polygon vertex.
		int lPolygonVertexCount = pMesh->GetControlPointsCount();

		if (!mAllByControlPoint)
		{
			lPolygonVertexCount = polygonCount * 3;
		}

		pGeomBuf->m_indices.resize(polygonCount * 3);
		pGeomBuf->m_vertices.resize(lPolygonVertexCount);

		lUVNames.Clear();
		pMesh->GetUVSetNames(lUVNames);

		if (lUVNames.GetCount() > kMaxUvSets)
		{
			B_LOG_WARNING("Max number of UV sets exceeded (MaxUvSets: %d, mesh has %d sets)", kMaxUvSets, lUVNames.GetCount());
			continue;
		}
		else
		{
			E_LOG_VERBOSE("# of UV sets in the mesh: %d", lUVNames.GetCount());
			pGeomBuf->m_uvSetCount = lUVNames.GetCount();
		}

		// Populate the array with vertex attribute, if by control point.
		const FbxVector4* lControlPoints = pMesh->GetControlPoints();
		FbxVector4 lCurrentVertex;
		FbxVector4 lCurrentNormal;
		FbxVector4 lCurrentTangent;
		FbxVector4 lCurrentBitangent;
		FbxVector2 lCurrentUV;
		FbxColor lCurrentColor;
		const FbxGeometryElementNormal* lNormalElement = nullptr;
		FbxGeometryElementUV* lUVElement[kMaxUvSets];
		const FbxGeometryElementTangent* lTangentElement = nullptr;
		const FbxGeometryElementBinormal* lBinormalElement = nullptr;
		const FbxGeometryElementVertexColor* lColorElement = nullptr;

		for (int uvset = 0; uvset < kMaxUvSets; ++uvset)
		{
			lUVElement[uvset] = nullptr;
		}

		if (mHasNormal)
		{
			lNormalElement = pMesh->GetElementNormal(0);
		}

		if (mHasUV)
		{
			for (int uvset = 0; uvset < minValue(kMaxUvSets, lUVNames.GetCount()); ++uvset)
			{
				lUVElement[uvset] = pMesh->GetElementUV(lUVNames[uvset]);
			}
		}

		if (mHasTangents)
		{
			lTangentElement = pMesh->GetElementTangent(0);
		}

		if (mHasBitangents)
		{
			lBinormalElement = pMesh->GetElementBinormal(0);
		}

		if (mHasColors)
		{
			lColorElement = pMesh->GetElementVertexColor(0);

			if (lColorElement)
			{
				E_LOG_VERBOSE("Mesh has vertex colors.");
			}
		}
		else
		{
			E_LOG_VERBOSE("No vertex colors found.");
		}

		if (mAllByControlPoint)
		{
			for (int lIndex = 0; lIndex < lPolygonVertexCount; ++lIndex)
			{
				// Save the vertex position.
				lCurrentVertex = lControlPoints[lIndex];

				Vertex vtx;

				vtx.position.x = -lCurrentVertex[0] + geomPos[0];
				vtx.position.y = -lCurrentVertex[1] + geomPos[1];
				vtx.position.z = -lCurrentVertex[2] + geomPos[2];

				E_LOG_VERBOSE("VtxPos[ABCP]: %f %f %f", vtx.position.x, vtx.position.y, vtx.position.z);

				// Save the normal.
				if (mHasNormal)
				{
					int lNormalIndex = lIndex;

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
				if (mHasUV)
				{
					int lUVIndex = lIndex;

					for (int uvset = 0; uvset < kMaxUvSets; ++uvset)
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
						vtx.uv[uvset].u = lCurrentUV[0];
						vtx.uv[uvset].v = lCurrentUV[1];
					}
				}

				if (mHasTangents)
				{
					int lTangentIndex = lIndex;

					if (lTangentElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
					{
						lTangentIndex = lTangentElement->GetIndexArray().GetAt(lIndex);
					}

					lCurrentTangent = lTangentElement->GetDirectArray().GetAt(lTangentIndex);
					vtx.tangent.x = lCurrentTangent[0];
					vtx.tangent.y = lCurrentTangent[1];
					vtx.tangent.z = lCurrentTangent[2];
				}

				if (mHasBitangents)
				{
					int lBinormalIndex = lIndex;

					if (lBinormalElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
					{
						lBinormalIndex = lBinormalElement->GetIndexArray().GetAt(lIndex);
					}

					lCurrentBitangent = lBinormalElement->GetDirectArray().GetAt(lBinormalIndex);
					vtx.bitangent.x = lCurrentBitangent[0];
					vtx.bitangent.y = lCurrentBitangent[1];
					vtx.bitangent.z = lCurrentBitangent[2];
				}

				if (mHasColors)
				{
					int lColorIndex = lIndex;

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

				pGeomBuf->m_vertices[lIndex] = vtx;
			}
		}
		else
		{
			E_LOG_VERBOSE("Data is By Polygon Vertex");

			if (mHasColors)
			{
				lColorElement = pMesh->GetElementVertexColor(0);

				if (lColorElement)
				{
					E_LOG_VERBOSE("Mesh has vertex colors.");
				}
			}
			else
			{
				E_LOG_VERBOSE("No vertex colors found.");
			}
		}

		int lVertexCount = 0;

		for (int lPolygonIndex = 0; lPolygonIndex < polygonCount; ++lPolygonIndex)
		{
			// The material for current face.
			int lMaterialIndex = 0;

			if (lMaterialIndices && lMaterialMappingMode == FbxGeometryElement::eByPolygon)
			{
				lMaterialIndex = lMaterialIndices->GetAt(lPolygonIndex);
			}

			// Where should I save the vertex attribute index, according to the material
			const int lIndexOffset = pPart->m_clusters[lMaterialIndex]->m_startIndex + pPart->m_clusters[lMaterialIndex]->m_primitiveCount * 3;

			for (int lVerticeIndex = 0; lVerticeIndex < 3; ++lVerticeIndex)
			{
				const int lControlPointIndex = pMesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex);

				if (mAllByControlPoint)
				{
					pGeomBuf->m_indices[lIndexOffset + lVerticeIndex] = static_cast<unsigned int>(lControlPointIndex);
				}
				// Populate the array with vertex attribute, if by polygon vertex.
				else
				{
					pGeomBuf->m_indices[lIndexOffset + lVerticeIndex] = static_cast<unsigned int>(lVertexCount);

					lCurrentVertex = lControlPoints[lControlPointIndex];
					
					pGeomBuf->m_vertices[lVertexCount].position.x = static_cast<f32>(lCurrentVertex[0]) + geomPos[0];
					pGeomBuf->m_vertices[lVertexCount].position.y = static_cast<f32>(lCurrentVertex[1]) + geomPos[1];
					pGeomBuf->m_vertices[lVertexCount].position.z = static_cast<f32>(lCurrentVertex[2]) + geomPos[2];

					E_LOG_VERBOSE("VtxPos: %f %f %f",
						pGeomBuf->m_vertices[lVertexCount].position.x,
						pGeomBuf->m_vertices[lVertexCount].position.y,
						pGeomBuf->m_vertices[lVertexCount].position.z);

					if (mHasNormal)
					{
						pMesh->GetPolygonVertexNormal(lPolygonIndex, lVerticeIndex, lCurrentNormal);
						pGeomBuf->m_vertices[lVertexCount].normal.x = static_cast<f32>(lCurrentNormal[0]);
						pGeomBuf->m_vertices[lVertexCount].normal.y = static_cast<f32>(lCurrentNormal[1]);
						pGeomBuf->m_vertices[lVertexCount].normal.z = static_cast<f32>(lCurrentNormal[2]);
					}

					if (mHasTangents)
					{
						int lTangentIndex = lControlPointIndex;

						if (lTangentElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
						{
							lTangentIndex = lTangentElement->GetIndexArray().GetAt(lTangentIndex);
						}

						lCurrentTangent = lTangentElement->GetDirectArray().GetAt(lTangentIndex);
						pGeomBuf->m_vertices[lVertexCount].tangent.x = lCurrentTangent[0];
						pGeomBuf->m_vertices[lVertexCount].tangent.y = lCurrentTangent[1];
						pGeomBuf->m_vertices[lVertexCount].tangent.z = lCurrentTangent[2];
					}

					if (mHasBitangents)
					{
						int lBinormalIndex = lControlPointIndex;

						if (lBinormalElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
						{
							lBinormalIndex = lBinormalElement->GetIndexArray().GetAt(lBinormalIndex);
						}

						lCurrentBitangent = lBinormalElement->GetDirectArray().GetAt(lBinormalIndex);
						pGeomBuf->m_vertices[lVertexCount].bitangent.x = lCurrentBitangent[0];
						pGeomBuf->m_vertices[lVertexCount].bitangent.y = lCurrentBitangent[1];
						pGeomBuf->m_vertices[lVertexCount].bitangent.z = lCurrentBitangent[2];
					}

					if (mHasUV)
					{
						bool bUnmapped;
						for (int uvset = 0; uvset < lUVNames.GetCount(); ++uvset)
						{
							pMesh->GetPolygonVertexUV(lPolygonIndex, lVerticeIndex, lUVNames[uvset], lCurrentUV, bUnmapped);
							pGeomBuf->m_vertices[lVertexCount].uv[uvset].u = static_cast<f32>(lCurrentUV[0]);
							pGeomBuf->m_vertices[lVertexCount].uv[uvset].v = static_cast<f32>(lCurrentUV[1]);
						}
					}

					if (mHasColors)
					{
						int lColorIndex = lControlPointIndex;

						if (lColorElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
						{
							lColorIndex = lColorElement->GetIndexArray().GetAt(lControlPointIndex);
						}

						lCurrentColor = lColorElement->GetDirectArray().GetAt(lColorIndex);
						pGeomBuf->m_vertices[lVertexCount].color.r = lCurrentColor.mRed;
						pGeomBuf->m_vertices[lVertexCount].color.g = lCurrentColor.mGreen;
						pGeomBuf->m_vertices[lVertexCount].color.b = lCurrentColor.mBlue;
						pGeomBuf->m_vertices[lVertexCount].color.a = lCurrentColor.mAlpha;
					}
				}

				++lVertexCount;
			}

			pPart->m_clusters[lMaterialIndex]->m_primitiveCount++;
		}

		for (size_t p = 0; p < pPart->m_clusters.size(); ++p)
		{
			pPart->m_clusters[p]->m_indexCount =
				pPart->m_clusters[p]->m_primitiveCount * 3;
		}
	}

	// solve parenthood
	for (size_t i = 0; i < partParentNameMap.size(); ++i)
	{
		auto iter = namePartMap.find(partParentNameMap.valueAt(i));

		if (iter != namePartMap.end())
		{
			partParentNameMap.keyAt(i)->m_pParent = iter->value;
			iter->value->m_children.appendUnique(partParentNameMap.keyAt(i));
		}
	}
}
	E_LOG_VERBOSE("Saving meshes...");
	saveMeshes();
	E_LOG_VERBOSE("Saving models...");
	saveModels();
	E_LOG_VERBOSE("Saving materials...");
	saveMaterials();
	E_LOG_VERBOSE("Saving skeletal anims...");
	saveAnimations();
	free();
	E_LOG_VERBOSE("Done.");

	return true;
}

void FbxFileImporter::free()
{
	m_pFbxScene->Clear();

	for (size_t i = 0; i < m_meshes.size(); ++i)
	{
		delete m_meshes[i];
	}

	for (size_t i = 0; i < m_animations.size(); ++i)
	{
		delete m_animations[i];
	}

	for (size_t i = 0; i < m_sceneNodes.size(); ++i)
	{
		delete m_sceneNodes[i];
	}

	m_meshes.clear();
	m_animations.clear();
	m_sceneNodes.clear();
}

void FbxFileImporter::saveMeshes()
{
	if (!m_bExportMeshes || m_meshes.isEmpty())
	{
		return;
	}

	String strTemp;

	for (size_t i = 0; i < m_meshes.size(); ++i)
	{
		Mesh* pMesh = m_meshes[i];
		JsonDocument doc;

		doc.beginDocument();
		doc.addValueToObject("geomBuffers");
		doc.beginArray();

		for (size_t j = 0; j < pMesh->m_geometryBuffers.size(); ++j)
		{
			GeometryBuffer* pGB = pMesh->m_geometryBuffers[j];

			doc.beginObject();// geom buffer
			strTemp = "";

			for (size_t k = 0; k < pGB->m_indices.size(); ++k)
			{
				strTemp += toString(pGB->m_indices[k]);

				if (k < pGB->m_indices.size() - 1)
				{
					strTemp += ";";
				}
			}

			doc.addValueToObject("indices", strTemp.c_str());

			String str;

			// positions
			strTemp = "";
			for (size_t k = 0; k < pGB->m_vertices.size(); ++k)
			{
				Vertex& v = pGB->m_vertices[k];

				formatString(str, "%s;%s;%s", 
						f2t(v.position.x).c_str(),
						f2t(v.position.y).c_str(),
						f2t(v.position.z).c_str());
				strTemp += str;

				if (k < pGB->m_vertices.size() - 1)
				{
					strTemp += ";";
				}
			}
			doc.addValueToObject("positions", strTemp.c_str());

			// normals
			strTemp = "";
			for (size_t k = 0; k < pGB->m_vertices.size(); ++k)
			{
				Vertex& v = pGB->m_vertices[k];

				formatString(str, "%s;%s;%s",
						f2t(v.normal.x).c_str(),
						f2t(v.normal.y).c_str(),
						f2t(v.normal.z).c_str());
				strTemp += str;
				if (k < pGB->m_vertices.size() - 1)
				{
					strTemp += ";";
				}
			}
			doc.addValueToObject("normals", strTemp.c_str());

			// tangents
			strTemp = "";
			for (size_t k = 0; k < pGB->m_vertices.size(); ++k)
			{
				Vertex& v = pGB->m_vertices[k];

				formatString(str, "%s;%s;%s",
							f2t(v.tangent.x).c_str(),
							f2t(v.tangent.y).c_str(),
							f2t(v.tangent.z).c_str());
				strTemp += str;
	
				if (k < pGB->m_vertices.size() - 1)
				{
					strTemp += ";";
				}
			}
			doc.addValueToObject("tangents", strTemp.c_str());

			// bitangents
			strTemp = "";
			for (size_t k = 0; k < pGB->m_vertices.size(); ++k)
			{
				Vertex& v = pGB->m_vertices[k];

				formatString(str, "%s;%s;%s", 
							f2t(v.bitangent.x).c_str(),
							f2t(v.bitangent.y).c_str(),
							f2t(v.bitangent.z).c_str());
				strTemp += str;

				if (k < pGB->m_vertices.size() - 1)
				{
					strTemp += ";";
				}
			}
			doc.addValueToObject("bitangents", strTemp.c_str());

			// colors
			strTemp = "";
			for (size_t k = 0; k < pGB->m_vertices.size(); ++k)
			{
				Vertex& v = pGB->m_vertices[k];

				formatString(str, "%s;%s;%s;%s", 
						f2t(v.color.r).c_str(),
						f2t(v.color.g).c_str(),
						f2t(v.color.b).c_str(),
						f2t(v.color.a).c_str());
				strTemp += str;

				if (k < pGB->m_vertices.size() - 1)
				{
					strTemp += ";";
				}
			}
			doc.addValueToObject("colors", strTemp.c_str());

			// uvSets
			doc.addValueToObject("uvSets");
			doc.beginArray();

			for (int uvset = 0; uvset < pGB->m_uvSetCount; ++uvset)
			{
				strTemp = "";
				
				for (size_t k = 0; k < pGB->m_vertices.size(); ++k)
				{
					Vertex& v = pGB->m_vertices[k];

					formatString(str, "%s;%s", 
								f2t(v.uv[uvset].u).c_str(),
								f2t(v.uv[uvset].v).c_str());
					strTemp += str;

					if (k < pGB->m_vertices.size() - 1)
					{
						strTemp += ";";
					}
				}
				doc.addValueToArray(strTemp.c_str());
			}

			doc.endArray();
			doc.endObject();// geom buffer
		}

		doc.endArray();// geom buffers

		doc.addValueToObject("parts");
		doc.beginArray();

		for (size_t j = 0; j < pMesh->m_parts.size(); ++j)
		{
			MeshPart* pPart = pMesh->m_parts[j];
			doc.beginObject();
			doc.addValueToObject("name", pPart->m_name.c_str());
			doc.addValueToObject("parent", pPart->m_pParent ? pPart->m_pParent->m_name.c_str() : "");
			doc.addValueToObject("translation", toString(pPart->m_translation).c_str());
			doc.addValueToObject("rotation", toString(pPart->m_rotation).c_str());
			doc.addValueToObject("scale", toString(pPart->m_scale).c_str());
			doc.addValueToObject("boundingBox", toString(pPart->m_bbox).c_str());
			doc.addValueToObject("bindPoseInverseMatrix", toString(pPart->m_bindPoseInverse).c_str());

			if (pPart->m_pHull)
			{
				doc.addValueToObject("meshHullIndex", pMesh->findHullIndex(pPart->m_pHull));
			}

			doc.addValueToObject("clusters");
			doc.beginArray();
			
			for (size_t k = 0; k < pPart->m_clusters.size(); ++k)
			{
				MeshCluster* pCluster = pPart->m_clusters[k];
				doc.beginObject();
				doc.addValueToObject("indexCount", pCluster->m_indexCount);
				doc.addValueToObject("primitiveCount", pCluster->m_primitiveCount);
				doc.addValueToObject("startIndex", pCluster->m_startIndex);
				doc.addValueToObject("geomBufferIndex", pMesh->findGeometryBufferIndex(pCluster->m_pGeomBuffer));
				doc.endObject();
			}

			doc.endArray();// clusters
			doc.endObject();
		}
		
		doc.endArray();// parts
		doc.endDocument();

		String meshFilename =
			formattedString(
			"%s/%s.mesh",
			m_destPath.c_str(),
			pMesh->m_name.c_str());

		E_LOG_VERBOSE("Saving mesh: %s", meshFilename.c_str());

		doc.save(meshFilename.c_str());
	}
}

void FbxFileImporter::saveModels()
{
	if (!m_bExportModels || m_meshes.isEmpty())
	{
		return;
	}
	
	// for each root mesh, save a model
	for (size_t i = 0, iCount = m_meshes.size(); i < iCount; ++i)
	{
		Mesh* pMesh = m_meshes[i];

		// skip lods, we are saving them when we hit root meshes
		if (pMesh->m_bIsLod)
			continue;

		String modelFolder;
		base::JsonDocument doc;

		modelFolder = mergePathPath(
						m_destPath.c_str(),
						pMesh->m_name.c_str());

		modelFolder = beautifyPath(modelFolder.c_str());
		createFolder(modelFolder.c_str());
		doc.beginDocument();
		doc.addValueToObject("name", pMesh->m_name.c_str());
		doc.addValueToObject("physicsSetup", "");

		//
		// save mesh list
		//
		String meshFile;

		doc.addValueToObject("meshes");
		doc.beginArray(); // meshes array

		// write main mesh file
		doc.beginObject();
		doc.addValueToObject("name", pMesh->m_name.c_str());
		base::formatString(
					meshFile,
					"%s/%s.mesh",
					m_destPath.c_str(),
					pMesh->m_name.c_str());
		doc.addValueToObject("file", meshFile.c_str());
		doc.endObject();

		// write lods
		for (size_t j = 0, jCount = pMesh->m_lods.size(); j < jCount; ++j)
		{
			doc.beginObject();
			doc.addValueToObject("name", pMesh->m_lods[j]->m_name.c_str());
			base::formatString(
					meshFile,
					"%s/%s.mesh",
					m_destPath.c_str(),
					pMesh->m_lods[j]->m_name.c_str());
			meshFile = beautifyPath(meshFile.c_str());
			doc.addValueToObject("file", meshFile.c_str());
			doc.endObject();
		}

		doc.endArray(); // meshes array

		//
		// define lods
		//
		f32 lodDist = m_lodStartDistance;

		doc.addValueToObject("lods");
		doc.beginArray(); // lods array

		for (int lod = -1; lod < (int)pMesh->m_lods.size(); ++lod)
		{
			Mesh* pLodMesh = lod == -1 ? pMesh : pMesh->m_lods[lod];

			// lodlevel
			doc.beginObject();
			doc.addValueToObject("fromDistance", lodDist);
			lodDist += m_lodStep;
			doc.addValueToObject("mesh", pLodMesh->m_name.c_str());

			//
			// save parts
			//
			doc.addValueToObject("parts");
			doc.beginArray();

			String strMaterialFile = 
				formattedString(
					"%s/%s.material",
					modelFolder.c_str(),
					pLodMesh->m_name.c_str());

			for (size_t i = 0, iCount = pLodMesh->m_parts.size(); i < iCount; ++i)
			{
				MeshPart* pPart = pLodMesh->m_parts[i];

				// part
				doc.beginObject(); // part obj
				doc.addValueToObject("name", pPart->m_name.c_str());

				if (pPart->m_pParent)
				{
					doc.addValueToObject("parent", pPart->m_pParent->m_name.c_str());
				}

				doc.addValueToObject("defaultMaterial", strMaterialFile.c_str());

				//
				// clusters
				//
				doc.addValueToObject("clusters");
				doc.beginArray(); // clusters array

				for (u32 cluster = 0, clusterCount = pPart->m_clusters.size(); cluster < clusterCount; ++cluster)
				{
					String str;

					E_LOG_VERBOSE("Saving material for cluster: %d", cluster);

					doc.beginObject(); // cluster obj
					formatString(str, "%s/%s/%s.material",
						m_destPath.c_str(),
						pMesh->m_name.c_str(),
						(m_bExportSingleLodMaterial ? pMesh : pLodMesh)->m_name.c_str());
					str = beautifyPath(str.c_str());

					if (pPart->m_clusters[cluster]->m_pMaterial)
					{
						doc.addValueToObject("material", pPart->m_clusters[cluster]->m_pMaterial->m_name.c_str());
					}
					else
					{
						doc.addValueToObject("material", str.c_str());
					}

					doc.endObject(); // cluster obj
				} // end clusters

				doc.endArray(); // clusters array
				doc.endObject(); // part obj
			}

			doc.endArray(); // parts
		}

		doc.endArray(); // lods array
		doc.endDocument();

		String modelFilename =
			formattedString(
				"%s/%s.model",
				m_destPath.c_str(),
				pMesh->m_name.c_str());

		E_LOG_VERBOSE("Saving model: %s", modelFilename.c_str());

		doc.save(modelFilename.c_str());
	}
}

void FbxFileImporter::saveMaterials()
{
	//TODO
}

void FbxFileImporter::saveAnimations()
{
	//TODO
}
}