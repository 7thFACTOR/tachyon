#pragma once
#include "../common.h"
#include "../asset_processor.h"
#include "mesh_util.h"

namespace ac
{
class AnimationProcessor : public AssetProcessor
{
public:
	AnimationProcessor();
	~AnimationProcessor();

	bool process(Asset& asset, JsonDocument& assetCfg) override;

protected:
};

}