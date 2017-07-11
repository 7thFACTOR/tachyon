#include "core/globals.h"
#include "core/resource_repository.h"
#include "base/util.h"
#include "base/assert.h"
#include "base/logger.h"
#include "base/profiler.h"
#include "core/resource.h"
#include "core/resource_type_repository.h"
#include "base/variable.h"

#include "core/resources/animation_resource_repository.h"
#include "core/resources/gpu_program_resource_repository.h"
#include "core/resources/material_resource_repository.h"
#include "core/resources/mesh_resource_repository.h"
#include "core/resources/texture_resource_repository.h"


namespace engine
{
class ResourceLoaderThread : public Thread
{
public:
	ResourceLoaderThread()
	{
		bundle = nullptr;
		create(this, false, "BundleLoader");
	}

	void queueLoadResource(ResourceId resId)
	{
		AutoLock<Mutex> lock(loaderLock);
		loadQueue.append(resId);
	}

	bool stillLoading()
	{
		AutoLock<Mutex> lock(loaderLock);
		bool still = loadQueue.notEmpty();

		return still;
	}

	void onRun()
	{
		while (running)
		{
			ResourceId resId = nullResourceId;

			{
				AutoLock<Mutex> lock(loaderLock);
			
				if (!loadQueue.isEmpty())
				{
					resId = loadQueue[0];
				}
			}
			
			if (resId == nullResourceId)
			{
				yield();
				continue;
			}
			
			Stream* stream = bundle->startLoadResource(resId);
			
			if (stream && !stream->isEndOfStream())
			{
				if (getResources().createResource(resId))
				{
					if (!getResources().loadResource(stream, resId))
					{
						B_LOG_ERROR("Error while streaming resource '" << bundle->getResourceName(resId) << "' (#" << resId << ")");
					}
					else
					{
						B_LOG_DEBUG("Loaded resource #" << resId << " (" << bundle->getResourceName(resId) << ")");
						ResourceType resType = bundle->getResourceType(resId);
						getResources().addLoadedResource(resType, resId);
					}
				}
				else
				{
					B_LOG_ERROR("Error while creating resource '" << bundle->getResourceName(resId) << "' (#" << resId << ")");
				}
			}
			else
			{
				B_LOG_DEBUG("Cannot start to load resource " << resId << ": " << bundle->getResourceName(resId));
			}

			if (stream)
			{
				bundle->endLoadResource(resId, stream);
			}

			{
				AutoLock<Mutex> lock(loaderLock);

				loadQueue.popFirst(resId);
			}
		}
	}

	Bundle* bundle = nullptr;
	Array<ResourceId> loadQueue;
	Mutex loaderLock;
};

ResourceRepository::ResourceRepository()
{
	memBudget = 0;
	memUsage = 0;

	registerResourceTypeRepository(new GpuProgramResourceRepository);
	registerResourceTypeRepository(new TextureResourceRepository);
	registerResourceTypeRepository(new AnimationResourceRepository);
	registerResourceTypeRepository(new MaterialResourceRepository);
	registerResourceTypeRepository(new MeshResourceRepository);
}

ResourceRepository::~ResourceRepository()
{
	//TODO: delete bundles, etc.
}

ResourceId ResourceRepository::createDynamicResource(ResourceType type)
{
	auto iter = resourceTypeRepositories.find(type);

	if (iter == resourceTypeRepositories.end())
	{
		return nullResourceId;
	}

	static u32 dynResCounter = 0;
	String fullPath = String("dynres/") << (u32)dynResCounter++;
	ResourceId resId = toResourceId(fullPath);

	{
		AutoLock<Mutex> lock(streamingThreadLock);

		resourcesUsage[resId] = 1;
		resourceNameIdCache[fullPath] = resId;
		resourceIdNameCache[resId] = fullPath;
		loadedResourcesQueue[type].appendUnique(resId);

		iter->value->createResource(resId);
		Resource* res = (Resource*)iter->value->tryMap(resId);

		if (res)
		{
			res->dynamicResource = true;
			res->resourceFlags |= ResourceFlags::Loaded;
		}
	}

	return resId;
}

void ResourceRepository::load(ResourceId resId)
{
	// own context to lock
	{ 
		AutoLock<Mutex> lock(streamingThreadLock);
		auto iter = resourceIdNameCache.find(resId);

		// already loaded, return
		if (iter != resourceIdNameCache.end())
		{
			return;
		}

		// call this to precache the various information
		if (getName(resId).isEmpty())
		{
			B_LOG_DEBUG("Inexistent resource #" << resId);
			return;
		}
		
		resourcesUsage[resId]++;
		// put on the thread loading queue
		resourceIdLoadQueue.appendUnique(resId);
		B_LOG_DEBUG("Loading resource #" << resId);
	}
}

void ResourceRepository::waitLoadAll()
{
	u32 lastTime = getTimeMilliseconds();

	while (true)
	{
		update();

		AutoLock<Mutex> lock(streamingThreadLock);
		bool stillLoading = false;

		for (auto thread : loaderThreads)
		{
			if (thread.value->stillLoading())
			{
				stillLoading = true;
			}
		}

		if (!stillLoading)
			return;

		yield();

		u32 now = getTimeMilliseconds();
		const u32 tooLongToLoad = 1000 * 60 * 60 * 24; //24h

		if (now - lastTime > tooLongToLoad)
		{
			B_LOG_ERROR("Waiting for resources to load took too long, exiting wait...");
			return;
		}
	}
}

void ResourceRepository::unload(ResourceId resId)
{
	AutoLock<Mutex> lock(streamingThreadLock);
	auto resRepo = getResourceTypeRepository(resId);

	B_ASSERT(resRepo);

	if (!resRepo)
	{
		return;
	}

	resourcesUsage[resId]--;

	if (resourcesUsage[resId] <= 0)
	{
		resRepo->unload(resId);
	}

	resourceIdNameCache.erase(resId);

	for (size_t i = 0; i < resourceNameIdCache.size(); i++)
	{
		if (resourceNameIdCache.valueAt(i) == resId)
		{
			resourceNameIdCache.eraseAt(i);
			break;
		}
	}
}

void ResourceRepository::reload(ResourceId resId)
{
	unload(resId);
	load(resId);
}

u64 ResourceRepository::getMemoryBudget() const
{
	return memBudget;
}

u64 ResourceRepository::getMemoryUsage() const
{
	return memUsage;
}

ResourceId ResourceRepository::getId(const String& resourceName)
{
	AutoLock<Mutex> lock(streamingThreadLock);
	size_t index = resourceNameIdCache.findIndex(resourceName);

	if (index != invalidDictionaryIndex)
	{
		return resourceNameIdCache.valueAt(index);
	}

	for (size_t i = 0; i < bundles.size(); ++i)
	{
		ResourceId resId = bundles[i]->getResourceId(resourceName);
			
		if (resId != nullResourceId)
		{
			resourceNameIdCache.add(resourceName, resId);
			return resId;
		}
	}

	return nullResourceId;
}

String ResourceRepository::getName(ResourceId resId)
{
	AutoLock<Mutex> lock(streamingThreadLock);

	auto iter = resourceIdNameCache.find(resId);

	if (iter != resourceIdNameCache.end())
	{
		return iter->value;
	}

	for (auto bundle : bundles)
	{
		if (!bundle->hasResourceId(resId))
		{
			continue;
		}

		const String& name = bundle->getResourceName(resId);

		if (!name.isEmpty())
		{
			resourceIdNameCache.add(resId, name);
			return name;
		}
	}

	B_LOG_ERROR("Could not find the name for resource #" << resId);

	return "";
}

bool ResourceRepository::isLoaded(ResourceId resId)
{
	return resourcesUsage.contains(resId);
}

ResourceType ResourceRepository::getResourceType(ResourceId resId)
{
	AutoLock<Mutex> lock(streamingThreadLock);
	Bundle* bundle = getBundleOf(resId);

	if (bundle)
	{
		return bundle->getResourceType(resId);
	}

	return ResourceType::None;
}

void* ResourceRepository::map(ResourceId resId, u32 waitTryMsec)
{
	AutoLock<Mutex> lock(streamingThreadLock);
	auto iter = resourceTypeRepositories.find(getResourceType(resId));

	if (iter == resourceTypeRepositories.end())
	{
		return nullptr;
	}

	u32 waited = 0;
	void* res = nullptr;

	while (waited <= waitTryMsec && !res)
	{
		res = iter->value->tryMap(resId);
		waited++;
		yield();
	}

	return res;
}

void ResourceRepository::unmap(ResourceId resId)
{
	AutoLock<Mutex> lock(streamingThreadLock);
	auto iter = resourceTypeRepositories.find(getResourceType(resId));

	if (iter == resourceTypeRepositories.end())
	{
		return;
	}

	iter->value->unmap(resId);
}

bool ResourceRepository::initialize()
{
	return true;
}

bool ResourceRepository::shutdown()
{
	unloadAll();
	return true;
}

void ResourceRepository::unloadAll()
{
	for (size_t i = 0; i < resourceTypeRepositories.size(); ++i)
	{
		resourceTypeRepositories.valueAt(i)->unloadAll();
	}
}

void ResourceRepository::update()
{
	AutoLock<Mutex> lock(streamingThreadLock);

	// add the current res ids to the bundle loader threads
	while (resourceIdLoadQueue.notEmpty())
	{
		ResourceId resId;

		resourceIdLoadQueue.popFirst(resId);
		Bundle* bundle = getBundleOf(resId);
		
		if (bundle)
		{
			loaderThreads[bundle]->queueLoadResource(resId);
		}
		else
		{
			B_LOG_ERROR("Resource #" << resId << " has no bundle, so probably does not exists");
		}
	}

	for (size_t i = 0; i < resourceTypeRepositories.size(); i++)
	{
		resourceTypeRepositories.valueAt(i)->update();
	}
}

Bundle* ResourceRepository::addBundle(const String& bundleName, BundleMode mode, const String& password)
{
	AutoLock<Mutex> lock(streamingThreadLock);

	if (mode == BundleMode::Auto)
	{
		if (getVariableRegistry().getVariableValue<String>("sys_build_type") == "shipping")
		{
			mode = BundleMode::OneBigFile;
		}
		else
		{
			// for debug, debug_editor, development, development_editor, shipping_editor
			// the resource files are loose on disk
			mode = BundleMode::LooseFiles;
		}
	}

	if (mode == BundleMode::LooseFiles)
	{
		if (!fileExists(bundleName + ".toc"))
		{
			B_LOG_ERROR("Bundle toc file not found " << bundleName);
			return nullptr;
		}
	}

	for (auto theBundle : bundles)
	{
		if (theBundle->getFilename() == bundleName)
		{
			B_LOG_DEBUG("Bundle " << bundleName << " already loaded");
			return theBundle;
		}
	}

	Bundle* bundle = new Bundle();

	if (!bundle->open(bundleName, mode, password))
	{
		delete bundle;
		return nullptr;
	}

	ResourceLoaderThread* loaderThread = new ResourceLoaderThread();
	
	loaderThread->bundle = bundle;
	loaderThread->start();

	{
		AutoLock<Mutex> lock(streamingThreadLock);
		loaderThreads.add(bundle, loaderThread);
		bundles.append(bundle);
	}

	return bundle;
}

void ResourceRepository::addBundlesFrom(const String& bundlesPath, bool recursive, BundleMode mode, const String& password)
{
	Array<FoundFileInfo> foundBundles;

	if (mode == BundleMode::Auto)
	{
		if (getVariableRegistry().getVariableValue<String>("sys_build_type") == "shipping")
		{
			mode = BundleMode::OneBigFile;
		}
		else
		{
			// for debug, debug_editor, development, development_editor, shipping_editor
			// the resource files are loose on disk
			mode = BundleMode::LooseFiles;
		}
	}

	if (mode == BundleMode::OneBigFile)
		scanFileSystem(bundlesPath, "*.bundle", foundBundles, recursive);
	else
		scanFileSystem(bundlesPath, "*.toc", foundBundles, recursive);

	for (auto& bundle : foundBundles)
	{
		addBundle(getFilenameNoExtension(bundle.filename), mode, password);
	}
}

void ResourceRepository::removeBundle(Bundle* bundle)
{
	AutoLock<Mutex> lock(streamingThreadLock);

	B_ASSERT(bundle);
	loaderThreads[bundle]->stop();
	delete loaderThreads[bundle];
	loaderThreads.erase(bundle);
	deleteBundleResources(bundle);
	bundle->close();
	delete bundle;
	bundles.erase(bundle);
}

Bundle* ResourceRepository::getBundleOf(ResourceId resId)
{
	AutoLock<Mutex> lock(streamingThreadLock);

	if (bundles.isEmpty())
	{
		return nullptr;
	}

	// search for the bundle from last loaded to first loaded
	// we do this because if a bundle loaded later can have a resource with the same 
	// path as one from a previous loaded bundle, this way we allow resource modding
	// addon bundles can override the standard base bundles
	for (int i = bundles.size() - 1; i >= 0; i--)
	{
		if (bundles[i]->hasResourceId(resId))
		{
			return bundles[i];
		}
	}

	return nullptr;
}

void ResourceRepository::computeMemoryUsage()
{
	AutoLock<Mutex> lock(streamingThreadLock);
	memUsage = 0;

	for (size_t i = 0; i < resourceTypeRepositories.size(); ++i)
	{
		memUsage += resourceTypeRepositories.valueAt(i)->getUsedMemory();
	}
}

void ResourceRepository::deleteBundleResources(Bundle* bundle)
{
	for (size_t i = 0; i < resourceTypeRepositories.size(); ++i)
	{
		resourceTypeRepositories.valueAt(i)->unloadBundleResources(bundle);
	}
}

bool ResourceRepository::isLocalizable(ResourceId resId)
{
	ResourceType resType = getResourceType(resId);

	{
		AutoLock<Mutex> lock(streamingThreadLock);

		auto iter = resourceTypeRepositories.find(resType);
	
		if (iter == resourceTypeRepositories.end())
		{
			B_LOG_DEBUG("Could not find a resource type repository that matches the resource type: " << (u32)resType);
			return false;
		}

		return iter->value->supportsLocalization();
	}
	
	return false;
}

bool ResourceRepository::createResource(ResourceId resId)
{
	// if a language is set, try find the resource as: some.jp.ntex
	if (!getCurrentLanguage().isEmpty() && isLocalizable(resId))
	{
		String ext, langFile = getName(resId), newExt;

		ext = getFilenameExtension(langFile);
		newExt = "." + getCurrentLanguage() + ext;
		langFile.replace(ext, newExt);

		B_LOG_DEBUG("Checking for localized resource: '" << langFile << "'...");

		ResourceId resIdLang = getId(langFile);

		if (resIdLang != nullResourceId)
		{
			resId = resIdLang;
			B_LOG_DEBUG("Found localized resource id: " << resIdLang);
		}
	}

	ResourceType resType = getResourceType(resId);

	{
		AutoLock<Mutex> lock(streamingThreadLock);

		auto iter = resourceTypeRepositories.find(resType);
	
		if (iter == resourceTypeRepositories.end())
		{
			B_LOG_DEBUG("Could not find a resource type repository that matches the resource type: " << (u32)resType);
			return false;
		}

		iter->value->createResource(resId);
	}

	return true;
}

bool ResourceRepository::loadResource(Stream* stream, ResourceId resId)
{
	ResourceType resType = getResourceType(resId);

	{
		AutoLock<Mutex> lock(streamingThreadLock);

		for (size_t i = 0; i < resourceTypeRepositories.size(); ++i)
		{
			if (resourceTypeRepositories.valueAt(i)->getResourceType() == resType)
			{
				return resourceTypeRepositories.valueAt(i)->load(*stream, resId);
			}
		}
	}

	return false;
}

ResourceTypeRepository* ResourceRepository::getResourceTypeRepository(ResourceId resId)
{
	ResourceType resType = getResourceType(resId);
	auto iter = resourceTypeRepositories.find(resType);

	if (iter == resourceTypeRepositories.end())
	{
		return nullptr;
	}

	return iter->value;
}

void ResourceRepository::addLoadedResource(ResourceType resType, ResourceId resId)
{
	AutoLock<Mutex> lock(streamingThreadLock);

	auto iter = loadedResourcesQueue.find(resType);

	if (iter != loadedResourcesQueue.end())
	{
		iter->value.append(resId);
	}
	else
	{
		loadedResourcesQueue.add(resType, Array<ResourceId>());
		iter = loadedResourcesQueue.find(resType);

		if (iter != loadedResourcesQueue.end())
		{
			iter->value.append(resId);
		}
	}
}

void ResourceRepository::setMemoryBudget(u64 budget)
{
	//TODO: actually use this budget, make resources report their size
	memBudget = budget;
}

void ResourceRepository::registerResourceTypeRepository(ResourceTypeRepository* repo)
{
	B_ASSERT(repo);

	if (resourceTypeRepositories.contains(repo->getResourceType()))
	{
		B_ASSERT_MSG(0, "There is already a resource type repository set for this resource type");
		return;
	}

	resourceTypeRepositories.add(repo->getResourceType(), repo);
}

void ResourceRepository::unregisterResourceTypeRepository(ResourceType resType)
{
	resourceTypeRepositories.erase(resType);
}

void ResourceRepository::fetchLoadedResourceIds(ResourceType type, Array<ResourceId>& outArray)
{
	AutoLock<Mutex> lock(streamingThreadLock);
	auto iter = loadedResourcesQueue.find(type);

	if (iter != loadedResourcesQueue.end())
	{
		outArray = iter->value;
		iter->value.clear();
		return;
	}

	outArray.clear();
}

}