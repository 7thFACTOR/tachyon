#pragma once
#include "common.h"
#include "asset_processor.h"

namespace ac
{
class PrefabProcessor : public AssetProcessor
{
public:
	PrefabProcessor();
	~PrefabProcessor();

	bool process(const char* pSrcFilename, const Project& project);
	const char* supportedFileExtensions() const;
};

}