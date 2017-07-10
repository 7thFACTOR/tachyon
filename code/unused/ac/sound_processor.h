#pragma once
#include "common.h"
#include "asset_processor.h"

namespace ac
{
class SoundProcessor : public AssetProcessor
{
public:
	SoundProcessor();
	~SoundProcessor();

	bool process(const char* pSrcFilename, const Project& project);
	const char* supportedFileExtensions() const;
};
}