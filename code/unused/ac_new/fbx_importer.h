#pragma once
#include "../common.h"
#include "../asset_processor.h"

namespace fbxsdk_2014_2
{
	class FbxManager;
	class FbxDocument;
	class FbxScene;
}

using namespace fbxsdk_2014_2;

namespace ac
{
struct ImportMesh;

struct FbxImportOptions
{
	bool bImportMaterials;
	bool bImportAnimations;
	bool bImportLODs;
	bool bImportHulls;
	bool bSingleMesh;
	bool bSingleLodMaterial;
	f32 lodStartDistance;
	f32 lodStep;
	String password;
	String destSavePath;
};

struct FbxFileImporter
{
	FbxFileImporter();
	~FbxFileImporter();

	bool import(const String& fbxFilename, const FbxImportOptions& options);
	void free();

protected:
	bool loadFbxScene(FbxManager* pSdkManager, FbxDocument* pScene, const String& filename);

	FbxManager* m_pFbxMgr;
	FbxScene* m_pFbxScene;
	Array<ImportMesh*> m_meshes;
	FbxImportOptions m_options;
};

}