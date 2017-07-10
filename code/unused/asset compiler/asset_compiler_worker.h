#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/args_parser.h"
#include "types.h"
#include "database.h"

namespace ac
{
class AssetCompilerWorker: public Thread
{
public:
	AssetCompilerWorker();
	~AssetCompilerWorker();

	void initialize(Project* pProject);
	void addAssetNode(AssetDependencyNode* asset);
	u32 assetItemCount();
	bool busy();
	void setWorkerIndex(u32 aIndex);
	void setArgs(const ArgsParser& rArgs);
	void onRun();

protected:
	u32 m_workerIndex;
	volatile bool m_bBusy;
	Array<AssetProcessor*> m_processors;
	Array<AssetDependencyNode*> m_assets;
	RMutex m_lockFiles;
	ArgsParser m_args;
	Project* m_pProject;
};

}