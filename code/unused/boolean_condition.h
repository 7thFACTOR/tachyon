#pragma once
//*****************************************************************************
// Nytro Game Engine
//
// (C) Copyright 7thFACTOR Entertainment
// All rights reserved
//*****************************************************************************
#include "system/string.h"
#include "system/defines.h"
#include "game/flowgraph/flowgraph.h"
#include "game/flowgraph/flowgraph_node.h"

namespace nytro
{
class Entity;

//! This graph object is used to evaluate a boolean condition, when entered
class N_API BooleanConditionFlowGraphNode: public FlowGraphNode
{
public:
	N_RUNTIME_CLASS;
	BooleanConditionFlowGraphNode();
	virtual ~BooleanConditionFlowGraphNode();
	virtual void process(FlowGraphNodeSlot* pFromSlot);

protected:
};
}