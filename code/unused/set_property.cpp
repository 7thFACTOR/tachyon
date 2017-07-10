#include "game/flowgraph/set_property.h"
#include "system/rtti.h"
#include "game/game_manager.h"
#include "game/game_types.h"
#include "system/core.h"
#include "system/assert.h"

namespace nytro
{
SetPropertyFlowGraphNode::SetPropertyFlowGraphNode()
{
	m_type = eType_Action;
	m_pComponent = nullptr;
	addSlot("In", FlowGraphNodeSlot::eType_Input);
	addSlot("Out", FlowGraphNodeSlot::eType_Output);
	addSlot("Component", FlowGraphNodeSlot::eType_VariableInput, true, Variant((void*)nullptr));
	addSlot("Value", FlowGraphNodeSlot::eType_VariableInput, true, Variant(""));
}

SetPropertyFlowGraphNode::~SetPropertyFlowGraphNode()
{
}

void SetPropertyFlowGraphNode::process(FlowGraphNodeSlot* pFromSlot)
{
	// get the data from the Component slot
	m_pComponent = (Component*)(void*)variableForSlot("Component");

	if (m_pComponent)
	{
		//TODO:
		//m_pEntity->setPropertyValueFromText(m_propertyName.c_str(), variableForSlot("Entity").toString().c_str());
	}

	signalOutputSlot("Out");
}
}