// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/types.h"
#include "base/defines.h"
#include "base/string.h"

namespace engine
{
using namespace base;

typedef u64 ResourceId;
typedef u128 UUID;

const ResourceId nullResourceId = 0;

//! The supported resource types
enum class ResourceType
{
	None,
	Flowgraph,
	Font,
	GpuProgram,
	Material,
	Mesh,
	Sound,
	Texture,
	TextureAtlas,
	World,
	Prefab,
	Character,
	Sprite,
	Ui,
	Particles,
	Script,
	Animation,
	AnimationSet,
	Cinematic,

	Count
};

//! Resource level of detail
enum class LevelOfDetail
{
	Low,
	Medium,
	High,
	Count
};

enum class EngineMode
{
	//! the engine is fully running the game
	Playing,
	//! the engine is simulating game systems, but no actual gameplay is happening
	Simulating,
	//! the engine is running inside editor, idle, no game playing (used by the entities to render schematic things)
	Editing,
	
	Count
};

enum class EngineHost
{
	Launcher,
	Editor,
	User,
	
	Count
};

enum class ResourceFlags
{
	None,
	Loaded = B_BIT(0)
};
B_ENUM_AS_FLAGS(ResourceFlags);

struct SystemTimer
{
	static const TimerId logic = 0;
	static const TimerId gui = 1;
	static const TimerId user = 2;
	static const TimerId lastId = 3;
};

}