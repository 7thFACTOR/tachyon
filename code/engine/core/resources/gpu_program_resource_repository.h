// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "core/resource_type_repository.h"
#include "core/resources/gpu_program_resource.h"

namespace engine
{
class E_API GpuProgramResourceRepository : public ResourceTypeRepositoryTpl<GpuProgramResource, ResourceType::GpuProgram>
{
public:
	GpuProgramResourceRepository()
	{
		localizationSupported = false;
	}
	
	bool load(Stream& stream, ResourceId resId);
	void unload(ResourceId resId);
};

}