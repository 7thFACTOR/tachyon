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
#include "game/flowgraph/flowgraph_base_nodes.h"

namespace nytro
{
struct Component;

//! This graph object is used to get a property value, used as variable in a graph
class N_API PropertyValueFlowGraphNode: public FlowGraphVariableNode
{
public:
	PropertyValueFlowGraphNode();
	virtual ~PropertyValueFlowGraphNode();

	void prepareVariable();
	void onPropertyValueChanged(void* pPropertyData);

protected:
	Component* m_pComponent;
	String m_propertyName;
};
}