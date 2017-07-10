#pragma once
#include "common.h"
#include "asset_processor.h"
#include "mesh_utils.h"
#include "mesh_processor.h"

namespace fbxsdk_2014_2
{
	class FbxManager;
	class FbxDocument;
	class FbxScene;
}

using namespace fbxsdk_2014_2;

namespace ac
{
class FbxFileImporter
{
public:
	FbxFileImporter();
	~FbxFileImporter();

	bool import(
			const char* pSrcFilename,
			const char* pDestPath,
			const ArgsParser& rArgs);
	void free();
	void saveMeshes();
	void saveModels();
	void saveMaterials();
	void saveAnimations();

protected:
	bool loadFbxScene(
		FbxManager* pSdkManager,
		FbxDocument* pScene,
		const char* pFilename);

	FbxManager* m_pFbxMan;
	FbxScene* m_pFbxScene;
	Array<Mesh*> m_meshes;
	Array<Animation*> m_animations;
	Array<Node*> m_sceneNodes;
	bool m_bExportMaterials,
		m_bExportMeshes,
		m_bExportAnimations,
		m_bSingleMesh,
		m_bExportModels,
		m_bExportLods,
		m_bExportHulls,
		m_bExportSingleLodMaterial,
		m_bSplitMeshRootsIntoModels;
	String m_assetFile, m_assetPassword, m_destPath;
	f32 m_lodStartDistance, m_lodStep;
};

}