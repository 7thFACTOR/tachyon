#pragma once
#include "base/string.h"
#include "base/defines.h"
#include "core/resource.h"
#include "logic/types.h"
#include "base/streamable.h"

namespace engine
{
using namespace base;

typedef u32 EntityTagIndex;

struct ComponentData : public Streamable
{
	ComponentTypeId typeId = 0;
	bool active = true;
	u64 dataSize = 0;
	u8* data = nullptr;

	void streamData(Stream& stream) override
	{
		stream | typeId;
		stream | active;
		stream | dataSize;

		if (stream.isReading())
		{
			//TODO: use a pool ?
			data = new u8[dataSize];
			stream.read(data, dataSize);
		}
		else
		{
			stream.write(data, dataSize);
		}
	}
};

struct EntityData : public Streamable
{
	EntityId id = 0;
	String name;
	bool active = true;
	EntityId parentId = 0;
	Array<EntityTagIndex> tags; //!< the tags are indices into the WorldResource::tagNames
	ResourceId prefabId = 0;
	Array<ComponentData> components;
	Array<EntityData> children;

	void streamData(Stream& stream) override
	{
		stream | id;
		stream | name;
		stream | parentId;
		stream | prefabId;
		stream | active;
		stream.streamPodArray(tags);
		stream | components;
		stream | children;
	}

	void deleteComponentData()
	{
		for (auto& comp : components)
		{
			B_SAFE_DELETE_ARRAY(comp.data);
			comp.dataSize = 0;
		}

		for (size_t i = 0; i < children.size(); i++)
		{
			children[i].deleteComponentData();
		}

		components.clear();
	}
};

struct WorldResource : Resource
{
	Array<String> tagNames;
	EntityData rootEntityData;
};

}