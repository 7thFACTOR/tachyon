#pragma once
#include "common.h"
#include "asset_processor.h"
#include "base/json.h"

namespace ac
{
class MaterialProcessor : public AssetProcessor
{
public:
	MaterialProcessor();
	~MaterialProcessor();

	bool process(const char* pSrcFilename, const Project& project);
	const char* supportedFileExtensions() const;
	bool convertMaterial(JsonDocument& doc, const char* pSrcFilename, const char* pDestPath, const ArgsParser& rArgs);
	bool writeMaterial(FileWriter& file, JsonNode* jsnRoot);
};

}