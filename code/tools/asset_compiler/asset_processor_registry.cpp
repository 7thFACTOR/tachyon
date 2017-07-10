#include "asset_processor_registry.h"

#include "asset_processors/texture_processor.h"
#include "asset_processors/texture_atlas_processor.h"
#include "asset_processors/gpu_program_processor.h"
#include "asset_processors/fbx_processor.h"
#include "asset_processors/material_processor.h"
#include "asset_processors/mesh_processor.h"
#include "asset_processors/font_processor.h"
#include "asset_processors/animation_processor.h"
//#include "processors/sprite_processor.h"
//#include "processors/particles_processor.h"
//#include "processors/physicsmaterial_processor.h"
//#include "processors/prefab_processor.h"
//#include "processors/sound_processor.h"
//#include "processors/world_processor.h"
//#include "processors/skeletal_animation_processor.h"

namespace ac
{
AssetProcessorRegistry s_assetProcessorRegistry;

AssetProcessorRegistry& AssetProcessorRegistry::getInstance()
{
	return s_assetProcessorRegistry;
}

AssetProcessorRegistry::AssetProcessorRegistry()
{
	instantiateProcessors(processors);
}

AssetProcessorRegistry::~AssetProcessorRegistry()
{}

ResourceType AssetProcessorRegistry::findResourceTypeByFileExtension(const String& ext)
{
	for (auto processor : processors)
	{
		if (processor->getSupportedAssetType().assetExtensions.find(ext) != processor->getSupportedAssetType().assetExtensions.end())
		{
			return processor->getSupportedAssetType().type;
		}
	}

	return ResourceType::None;
}

AssetProcessor* AssetProcessorRegistry::findProcessorByResourceType(ResourceType type)
{
	for (AssetProcessor* processor : processors)
	{
		if (processor->getSupportedAssetType().type == type)
		{
			return processor;
		}
	}

	return nullptr;
}

AssetProcessor* AssetProcessorRegistry::findProcessorByImportFileExtension(const String& ext)
{
	for (AssetProcessor* processor : processors)
	{
		if (processor->getSupportedAssetType().assetExtensions.find(ext) != processor->getSupportedAssetType().assetExtensions.end())
		{
			return processor;
		}
	}

	return nullptr;
}

void AssetProcessorRegistry::instantiateProcessors(Array<AssetProcessor*>& processors)
{
	processors.append(new FbxProcessor());
	processors.append(new TextureProcessor());
	processors.append(new TextureAtlasProcessor());
	processors.append(new GpuProgramProcessor());
	processors.append(new MaterialProcessor());
	processors.append(new MeshProcessor());
	processors.append(new FontProcessor());
	processors.append(new AnimationProcessor());
	//processors.append(new PhysicsMaterialProcessor());
	//processors.append(new PrefabProcessor());
	//processors.append(new SkeletalAnimationProcessor());
	//processors.append(new SoundProcessor());
	//processors.append(new SpriteProcessor());
	//processors.append(new UiProcessor());
	//processors.append(new WorldProcessor());
}

void AssetProcessorRegistry::debug()
{
	B_LOG_DEBUG("Available asset processors:");
	for (auto& ap : processors)
	{
		B_LOG_DEBUG("\tType: " << (u32)ap->getSupportedAssetType().type
			<< " Imports extensions:");
		B_LOG_INDENT
		for (auto& ext : ap->getSupportedAssetType().assetExtensions)
		{
			B_LOG_DEBUG(ext);
		}
		B_LOG_UNINDENT
	}
}

}