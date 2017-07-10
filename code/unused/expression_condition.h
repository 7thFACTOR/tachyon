#pragma once
//*****************************************************************************
// Nytro Game Engine
//
// (C) Copyright 7thFACTOR Entertainment
// All rights reserved
//*****************************************************************************
#include "system/string.h"
#include "system/defines.h"
#include "game/flowgraph/flowgraph_node.h"

namespace nytro
{
class Entity;

//! This graph object is used to evaluate an expression, when entered
class N_API ExpressionConditionFlowGraphNode: public FlowGraphNode
{
public:
	ExpressionConditionFlowGraphNode();
	virtual ~ExpressionConditionFlowGraphNode();

	virtual void process(FlowGraphNodeSlot* pFromSlot);
};
}