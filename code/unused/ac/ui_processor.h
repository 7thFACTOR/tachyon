#pragma once
#include "common.h"
#include "asset_processor.h"

namespace ac
{
class UiProcessor : public AssetProcessor
{
public:
	UiProcessor();
	~UiProcessor();

	bool process(const char* pSrcFilename, Stream* pStream, const Project& project) override;
	const char* supportedFileExtensions() const;
};

}