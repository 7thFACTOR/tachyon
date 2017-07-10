#pragma once
#include "common.h"
#include "asset_processor.h"

namespace ac
{
class PhysicsMaterialProcessor : public AssetProcessor
{
public:
	PhysicsMaterialProcessor();
	~PhysicsMaterialProcessor();

	bool process(const char* pSrcFilename, const Project& project);
	const char* supportedFileExtensions() const;
};

}