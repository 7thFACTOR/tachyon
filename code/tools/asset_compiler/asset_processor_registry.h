#pragma once
#include "common.h"
#include "asset_processor.h"
#include "base/string.h"
#include "base/array.h"
#include "base/thread.h"
#include "base/mutex.h"
#include "base/json.h"
#include "base/cmdline_arguments.h"
#include "core/types.h"

namespace ac
{
using namespace base;
using namespace engine;

struct AssetProcessor;

struct AssetProcessorRegistry
{
	AssetProcessorRegistry();
	virtual ~AssetProcessorRegistry();

	static AssetProcessorRegistry& getInstance();

	ResourceType findResourceTypeByFileExtension(const String& ext);
	AssetProcessor* findProcessorByResourceType(ResourceType type);
	AssetProcessor* findProcessorByImportFileExtension(const String& ext);
	void instantiateProcessors(Array<AssetProcessor*>& processors);
	void debug();

	Array<AssetProcessor*> processors;
};

}