// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/array.h"
#include "base/dictionary.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/variant.h"
#include "core/resource.h"
#include "logic/types.h"

namespace engine
{
typedef u32 FlowgraphNodeId;
typedef u32 FlowgraphNodeSlotId;

enum class FlowgraphNodeType
{
	Initiator,
	Transform,
	
	Count
};

enum class FlowgraphNodeDomain
{
	Logic,
	Event,
	Branch,
	Function,
	Variable,
	Comment,
	Macro,
	
	Count
};

enum class FlowgraphType
{
	Editor,
	Constructor,
	Logic,
	Material,
	
	Count
};

enum class FlowgraphNodeSlotDataType
{
	None,
	Int32,
	Int64,
	Float,
	Double,
	String,
	Bool,
	Vec2,
	Vec3,
	Color,
	Quat,
	Matrix,
	Texture,
	Shader,
	Material,
	Transform,
	
	Count
};

enum class FlowgraphNodeSlotMode
{
	None,
	In,
	Out
};

struct FlowgraphNodeDefinition
{
	FlowgraphNodeType type;
	FlowgraphNodeDomain domain;
	Dictionary<String, Variant> properties;
};

struct FlowgraphNodeSlotDefinition
{
	String name;
	FlowgraphNodeSlotDataType dataType;
	FlowgraphNodeSlotMode mode;
	Variant defaultValue;
};

struct FlowgraphNodeData
{
	FlowgraphNodeId id;
	FlowgraphNodeDefinition* definition;
	Dictionary<String, Variant> properties;
};

struct FlowgraphNodeLinkData
{
	FlowgraphNodeId fromNode, toNode;
	FlowgraphNodeSlotId fromSlot, toSlot;
};

struct FlowgraphNodeSlotData
{
	FlowgraphNodeSlotId id;
	Variant value;
};

struct FlowgraphResource
{
	FlowgraphType type;
	size_t nodeCount;
	FlowgraphNodeData* nodes;
	size_t linkCount;
	FlowgraphNodeLinkData* links;
};

}