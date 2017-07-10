#pragma once
#include "common.h"
#include "asset_processor.h"

namespace ac
{
class ParticlesProcessor : public AssetProcessor
{
public:
	ParticlesProcessor();
	~ParticlesProcessor();

	bool process(const char* pSrcFilename, const Project& project);
	const char* supportedFileExtensions() const;
};
}