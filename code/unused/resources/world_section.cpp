#include "resources/world_section.h"
#include "core/types.h"

namespace engine
{
B_REGISTER_DERIVED_TAGGED_CLASS(WorldSectionResource, Resource, ResourceTypes::WorldSection);

WorldSectionResource::WorldSectionResource()
{
}

WorldSectionResource::~WorldSectionResource()
{
}

bool WorldSectionResource::load(Stream* pStream)
{
	return true;
}

bool WorldSectionResource::unload()
{
	return true;
}

}