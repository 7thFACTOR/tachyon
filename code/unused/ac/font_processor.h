#pragma once
#include "common.h"
#include "asset_processor.h"

namespace ac
{
class FontProcessor : public AssetProcessor
{
public:
	FontProcessor();
	~FontProcessor();

	bool process(const char* pSrcFilename, const Project& project) override;
	const char* supportedFileExtensions() const;
};
}