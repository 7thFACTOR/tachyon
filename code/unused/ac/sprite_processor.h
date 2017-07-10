#pragma once
#include "common.h"
#include "asset_processor.h"

namespace ac
{
class SpriteProcessor : public AssetProcessor
{
public:
	SpriteProcessor();
	~SpriteProcessor();

	bool process(const char* pSrcFilename, const Project& project);
	const char* supportedFileExtensions() const;
};

}