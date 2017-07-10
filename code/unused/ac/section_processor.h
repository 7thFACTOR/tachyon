#pragma once
#include "common.h"
#include "asset_processor.h"

namespace ac
{
class SectionProcessor : public AssetProcessor
{
public:
	SectionProcessor();
	~SectionProcessor();

	bool process(const char* pSrcFilename, const Project& project);
	const char* supportedFileExtensions() const;
};

}