// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "../common.h"
#include "../asset_processor.h"
#include "mesh_util.h"

namespace FBXSDK_NAMESPACE
{
	class FbxManager;
	class FbxDocument;
	class FbxScene;
}

using namespace FBXSDK_NAMESPACE;

namespace ac
{
struct ImportMesh;

struct FbxImportOptions
{
	bool importMaterials = true;
	bool importAnimations = true;
	bool importLODs = true;
	bool importHulls = true;
	bool singleMesh = true;
	bool singleLodMaterial = true;
	f32 lodStartDistance = 0;
	f32 lodStep = 10;
	String password;
	String destSavePath;
};

class FbxFileImporter
{
public:
	FbxFileImporter();
	~FbxFileImporter();

	bool import(const String& fbxFilename, const FbxImportOptions& options);
	void free();

public:
	Array<ImportMesh*> meshes;
	Array<ImportAnimationClip> animations;
	Array<ImportMaterial> materials;
	String sourceFbx;
	FbxImportOptions options;

protected:
	bool loadFbxScene(FbxManager* sdkManager, FbxDocument* scene, const String& filename);
	bool importAnimationCurve(class FbxAnimCurve* fbxCurve, ImportAnimationCurve& curve, const String& curveName);
	ImportMesh* findMeshByFbxNode(FbxNode* node);

	FbxManager* fbxMgr = nullptr;
	FbxScene* fbxScene = nullptr;
};

}