#pragma once
#include "common.h"
#include "asset_processor.h"

namespace ac
{
class ModelProcessor : public AssetProcessor
{
public:
	ModelProcessor();
	~ModelProcessor();

	bool process(const char* pSrcFilename, const Project& project);
	const char* supportedFileExtensions() const;
};

}