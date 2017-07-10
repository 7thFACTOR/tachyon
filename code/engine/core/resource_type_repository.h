#pragma once
#include "base/string.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/platform.h"
#include "base/memory.h"
#include "base/assert.h"
#include "core/defines.h"
#include "core/types.h"
#include "base/mutex.h"
#include "base/class_registry.h"
#include "core/bundle.h"

namespace base
{
class Stream;
}

namespace engine
{
using namespace base;

class ResourceTypeRepository
{
public:
	ResourceTypeRepository(){}
	virtual ~ResourceTypeRepository(){}

	virtual bool load(Stream& stream, ResourceId resId) = 0;
	virtual void unload(ResourceId resId) = 0;
	virtual ResourceType getResourceType() = 0;
	virtual bool supportsLocalization() const = 0;
	virtual void* tryMap(ResourceId resId) = 0;
	virtual void unmap(ResourceId resId) = 0;
	virtual void unloadAll() = 0;
	virtual void reloadAll() = 0;
	virtual u64 getUsedMemory() const = 0;
	virtual bool createResource(ResourceId resId) = 0;
	virtual void unloadBundleResources(Bundle* bundle) = 0;
	virtual void update() = 0;

protected:
	Array<ResourceId> mappedResources;
};

template<typename Type, ResourceType ResType>
class ResourceTypeRepositoryTpl : public ResourceTypeRepository
{
public:
	ResourceTypeRepositoryTpl()
	{}
	
	virtual ~ResourceTypeRepositoryTpl()
	{}
	
	virtual bool load(Stream& stream, ResourceId resId) override
	{
		return false;
	}
	
	virtual void unload(ResourceId resId) override
	{}

	virtual void reloadAll() override
	{}

	virtual void update() override
	{}
	
	ResourceType getResourceType() override { return ResType; }

	virtual void* tryMap(ResourceId resId) override
	{
		auto iter = resources.find(resId);

		if (iter == resources.end())
		{
			return nullptr;
		}

		if (mappedResources.contains(resId))
		{
			return nullptr;
		}

		mappedResources.append(resId);

		return iter->value;
	}

	virtual void unmap(ResourceId resId) override
	{
		mappedResources.erase(resId);
	}

	virtual void unloadAll() override
	{
		for (size_t i = 0; i < resources.size(); ++i)
		{
			delete resources.valueAt(i);
		}

		mappedResources.clear();
		resources.clear();
	}

	virtual u64 getUsedMemory() const override
	{
		return 0;
	}

	virtual bool createResource(ResourceId resId) override
	{
		if (resources.find(resId) != resources.end())
		{
			return false;
		}

		//TODO: use a memory pool ?
		Type* res = new Type();
		resources.add(resId, res);

		return true;
	}

	virtual void unloadBundleResources(Bundle* bundle) override
	{
		B_ASSERT(bundle);
		Array<ResourceId> removeIds;
		
		for (size_t i = 0; i < resources.size(); ++i)
		{
			if (bundle->getTocEntries().contains(resources.keyAt(i)))
			{
				removeIds.append(resources.keyAt(i));
			}
		}

		for (size_t i = 0; i < removeIds.size(); i++)
		{
			unload(removeIds[i]);
		}
	}
	
	bool supportsLocalization() const
	{
		return localizationSupported;
	}

protected:
	Dictionary<ResourceId, Type*> resources;
	bool localizationSupported = true;
};

}