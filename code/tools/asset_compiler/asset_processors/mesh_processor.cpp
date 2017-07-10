#include "../common.h"
#include "base/math/vec3.h"
#include "base/util.h"
#include "base/logger.h"
#include "base/file.h"
#include "base/json.h"
#include "mesh_processor.h"
#include "../asset_compiler.h"
#include "core/resources/mesh_resource.h"
#include "graphics/types.h"
#include "../project.h"

namespace ac
{
using namespace base;
using namespace engine;

MeshProcessor::MeshProcessor()
{
	Array<String> extensions;

	extensions.append(".mesh");
	supportedAssetType = SupportedAssetType(ResourceType::Mesh, extensions);
}

MeshProcessor::~MeshProcessor()
{}

bool MeshProcessor::process(Asset& asset, JsonDocument& assetCfg)
{
	// no modification for now, the mesh data format is the same as the final one
	//TODO: optimize for various platforms?

	copyFile(asset.absFilename, asset.deployFilename);

	return true;
}

}