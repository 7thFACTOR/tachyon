// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "asset_compiler.h"
#include "base/stdio_logger.h"
#include "base/logger.h"
#include "asset_processor_registry.h"
#include "base/file.h"

namespace ac
{
AssetCompilerWorker::AssetCompilerWorker()
{}

AssetCompilerWorker::~AssetCompilerWorker()
{
	for (size_t i = 0, iCount = processors.size(); i < iCount; ++i)
	{
		delete processors[i];
	}
}

void AssetCompilerWorker::initialize(Project* prj, u32 workerThreadIndex, const CommandLineArguments& arguments)
{
	project = prj;
	args = arguments;
	workerIndex = workerThreadIndex;
	AssetProcessorRegistry::getInstance().instantiateProcessors(processors);
}

void AssetCompilerWorker::addAsset(Asset* asset)
{
	AutoLock<Mutex> lock(lockAssets);
	assets.append(asset);
}

size_t AssetCompilerWorker::getAssetCount()
{
	AutoLock<Mutex> lock(lockAssets);
	return assets.size();
}

void AssetCompilerWorker::onRun()
{
	busy = true;
	
	B_LOG_INFO("Worker " << workerIndex << " started");

	while (running)
	{
		Asset* asset = nullptr;

		{
			AutoLock<Mutex> lock(lockAssets);

			if (assets.notEmpty())
			{
				assets.popFirst(asset);
			}
		}

		if (!asset)
		{
			compiler->stealWorkFor(this);
		}

		if (!asset)
		{
			busy = false;
		}

		if (asset)
		{
			File outputFile;
            BundleInfo* bundle = asset->bundle;
            String deployFolder;

            if (!asset->bundle)
            {
                B_LOG_ERROR("Asset has no bundle: " << asset->resId << " " << asset->name);
                continue;
            }
            
            for (size_t j = 0, jCount = processors.size(); j < jCount; ++j)
			{
				auto& supportedAssetInfo = processors[j]->getSupportedAssetInfo();

				if (supportedAssetInfo.importerOnly)
				{
					continue;
				}

				if (supportedAssetInfo.outputResourceType == asset->type)
				{
					B_LOG_INFO("Processing: '" << asset->name << "' to '" << asset->absoluteOutputFilename << "'...");

                    createPath(asset->absolutePath);
                    createPath(getFilenamePath(asset->absoluteOutputFilename));

					JsonDocument doc(mergePathPath(bundle->absolutePath, asset->name + ".asset"));

					if (doc.hasErrors())
					{
						continue;
					}

					asset->dependencies.clear();

					B_LOG_INFO("Processing " << asset->name + ".asset");

					bool ok = processors[j]->process(*asset, doc);

					if (!ok)
					{
						B_LOG_ERROR("An error occured when converting file '" << asset->name << "'");
					}
					else
					{
						DateTime dt;
                        String finalPath;

						if (getFileDateTime(asset->absoluteName, 0, 0, &dt))
						{
							asset->lastWriteTime = dt.toUnixTime();
						}
						else
						{
							B_LOG_ERROR("Cannot retrieve lastwrite timestamp for asset '" << asset->absoluteName << "'");
						}
					}

					break;
				}
			}
		}

		base::yield();
	}

	B_LOG_INFO("Worker " << workerIndex << " has finished.");

	busy = false;
}

}