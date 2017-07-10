#pragma once
#include "base/array.h"
#include "base/string.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/dictionary.h"
#include "base/util.h"
#include "base/logger.h"
#include "core/defines.h"
#include "core/types.h"
#include "core/bundle.h"
#include "base/observer.h"
#include "base/thread.h"
#include "base/mutex.h"

namespace engine
{
using namespace base;
struct Resource;
class Bundle;
class ResourceLoaderThread;
class ResourceTypeRepository;

const u32 maxWaitToMapResourceMsec = 10000;

class E_API ResourceRepository
{
public:
	ResourceRepository();
	virtual ~ResourceRepository();

	ResourceId createDynamicResource(ResourceType type);
	void load(ResourceId resId);
	void waitLoadAll();
	void unload(ResourceId resId);
	void reload(ResourceId resId);
	u64 getMemoryBudget() const;
	u64 getMemoryUsage() const;
	ResourceId getId(const String& resourceFilename);
	String getName(ResourceId resId);
	ResourceType getResourceType(ResourceId resId);
	template <typename Type>
	Type* map(ResourceId resId, u32 waitTryMsec = maxWaitToMapResourceMsec)
	{ return (Type*)map(resId, waitTryMsec); }
	void* map(ResourceId resId, u32 waitTryMsec = maxWaitToMapResourceMsec);
	void unmap(ResourceId resId);

	bool initialize();
	bool shutdown();
	void unloadAll();
	void update();
	Bundle* addBundle(const String& bundleTocFile, BundleMode mode = BundleMode::Auto, const String& password = "");
	void addBundlesFrom(const String& bundlesPath, bool recursive = true, BundleMode mode = BundleMode::Auto, const String& password = "");
	void removeBundle(Bundle* bundle);
	inline const Array<Bundle*>& getBundles() const { return bundles; }
	Bundle* getBundleOf(ResourceId resId);
	void computeMemoryUsage();
	void setMemoryBudget(u64 budget);
	void registerResourceTypeRepository(ResourceTypeRepository* repo);
	void unregisterResourceTypeRepository(ResourceType resType);
	void fetchLoadedResourceIds(ResourceType type, Array<ResourceId>& outArray);

protected:
	friend class ResourceLoaderThread;

	void deleteBundleResources(Bundle* bundle);
	bool createResource(ResourceId resId);
	bool loadResource(Stream* stream, ResourceId resId);
	ResourceTypeRepository* getResourceTypeRepository(ResourceId resId);
	void addLoadedResource(ResourceType resType, ResourceId resId);
	bool isLocalizable(ResourceId resId);
	
	Mutex streamingThreadLock;
	u64 memBudget, memUsage;
	Array<Bundle*> bundles;
	Array<ResourceId> resourceIdLoadQueue;
	Dictionary<ResourceId, u32> resourcesUsage;
	Dictionary<ResourceType, ResourceTypeRepository*> resourceTypeRepositories;
	Dictionary<String, ResourceId> resourceNameIdCache;
	Dictionary<ResourceId, String> resourceIdNameCache;
	Dictionary<Bundle*, ResourceLoaderThread*> loaderThreads;
	Dictionary<ResourceType, Array<ResourceId> > loadedResourcesQueue;
};

template<typename ResourceClass>
class ScopedResourceMapping
{
public:
	ScopedResourceMapping(ResourceId resId)
	{
		mappedResource = getResources().map<ResourceClass>(resId);
		resourceId = resId;
	}

	~ScopedResourceMapping()
	{
		getResources().unmap(resourceId);
	}

	operator ResourceClass*() { return mappedResource; }
	ResourceClass* operator ->() const { return mappedResource; }

protected:
	ResourceId resourceId = nullResourceId;
	ResourceClass* mappedResource = nullptr;
};

}