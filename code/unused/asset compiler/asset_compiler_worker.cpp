#include "asset_compiler.h"
#include "base/stdio_logger.h"
#include "base/logger.h"
#include "asset_processor_registry.h"
#include "base/file.h"

namespace ac
{

AssetCompilerWorker::AssetCompilerWorker()
{
}

AssetCompilerWorker::~AssetCompilerWorker()
{
	for (size_t i = 0, iCount = m_processors.size(); i < iCount; ++i)
	{
		delete m_processors[i];
	}
}

void AssetCompilerWorker::initialize(Project* pProject)
{
	m_pProject = pProject;
	AssetProcessorRegistry::instance().instantiateProcessors(m_processors);
}

void AssetCompilerWorker::addAssetNode(AssetDependencyNode* asset)
{
	AutoLock<RMutex> lock(m_lockFiles);
	m_assets.append(asset);
}

u32 AssetCompilerWorker::assetItemCount()
{
	AutoLock<RMutex> lock(m_lockFiles);
	return m_assets.size();
}

bool AssetCompilerWorker::busy()
{
	return m_bBusy;
}

void AssetCompilerWorker::setWorkerIndex(u32 aIndex)
{
	m_workerIndex = aIndex;
}

void AssetCompilerWorker::setArgs(const ArgsParser& rArgs)
{
	m_args = rArgs;
}

void AssetCompilerWorker::onRun()
{
	m_bBusy = true;

	for (auto& asset : m_assets)
	{
		File outputFile;

		for (size_t j = 0, jCount = m_processors.size(); j < jCount; ++j)
		{
			SupportedAssetType supportedAssetType = m_processors[j]->supportedAssetType();

			if (supportedAssetType.type == asset.type)
			{
				B_LOG_INFO("Processing: '%s' to '%s'...",
					asset.filename.c_str(), asset.destFilename.c_str());

				String destFolder;

				extractFilePath(asset.destFilename, destFolder);
				createFullPath(destFolder);

				BundleInfo* pBundleInfo = m_pProject->findBundleById(asset.bundleId);

				if (!pBundleInfo)
				{
					B_LOG_ERROR("Could not find a bundle with id: %d", asset.bundleId);
					break;
				}

				bool bOk = m_processors[j]->process(asset, pBundleInfo, *m_pProject);

				if (!bOk)
				{
					B_LOG_ERROR("An error occured when converting file '%s'", asset.filename.c_str());
				}
				else
				{
					DateTime dt;

					if (fileDateTime(asset.filename, 0, 0, &dt))
					{
						m_pProject->db.markAssetAsCompiled(asset.resId, dt.toUnixTime());
					}
					else
					{
						B_LOG_ERROR("Cannot retrieve lastwrite timestamp for final asset '%s'", asset.destFilename.c_str());
					}
				}

				break;
			}
		}
	}
	
	m_bBusy = false;
}

}