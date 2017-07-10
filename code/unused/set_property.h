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
struct Component;

//! This graph object is used to set a property value for an entity, when entered
class N_API SetPropertyFlowGraphNode: public FlowGraphNode
{
public:
	SetPropertyFlowGraphNode();
	virtual ~SetPropertyFlowGraphNode();

	virtual void process(FlowGraphNodeSlot* pFromSlot);

protected:
	Component* m_pComponent;
	String m_propertyName;
};
}