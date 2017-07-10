#pragma once
#include "../common.h"
#include "../asset_processor.h"
#include "mesh_util.h"

namespace ac
{
class MeshProcessor : public AssetProcessor
{
public:
	MeshProcessor();
	~MeshProcessor();

	bool process(Asset& asset, JsonDocument& assetCfg) override;

protected:
};

}