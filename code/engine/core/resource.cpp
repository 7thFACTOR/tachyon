#include "core/resource.h"
#include "core/globals.h"
#include "core/resource_repository.h"
#include "base/util.h"
#include "base/logger.h"

namespace engine
{
#ifdef _DEBUG
// make a Dictionary with id and filename for the asked resources
Dictionary<ResourceId, String> toResourceIds;
#endif

ResourceId toResourceId(const String& filename)
{
	JenkinsLookup3Hash64 hasher;

	hasher.add(filename.c_str(), filename.length());
	auto id = hasher.getHash64();

#ifdef _DEBUG
	auto iter = toResourceIds.find(id);

	if (iter == toResourceIds.end())
	{
#ifdef _ASKED_FOR_RESOURCEID
		B_LOG_INFO("Asked for #" << id << " -> " << filename);
#endif
		toResourceIds[id] = filename;
	}
#endif

	return id;
}

ResourceId loadResource(const String& filename)
{
	ResourceId id = toResourceId(filename);

	if (!getResources().isLoaded(id))
		B_LOG_INFO("Trying to load #" << id << " -> " << filename);

	getResources().load(id);

	return id;
}

}