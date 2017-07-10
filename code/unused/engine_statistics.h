#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "core/defines.h"

namespace engine
{
//! This class will gather information about the whole engine and current game level/world
class ENGINE_API EngineStatistics
{
public:
	struct MemoryInfo
	{
		u32 count = 0;
		u64 usedMemory = 0;
	};

	EngineStatistics();
	virtual ~EngineStatistics();

	virtual bool gatherInfo();

	MemoryInfo textures, systemTextures,
		entities, sounds, gpuPrograms, materials,
		meshes, flowgraphs, particleSystems, fonts,
		prefabs, physicsMaterials;
	u64 totalMemoryUsage = 0;
	u64 peakMemoryUsage = 0;

protected:
	virtual bool gatherResourcesInfo();
	virtual bool gatherGameInfo();
	virtual bool gatherMiscInfo();
};

}