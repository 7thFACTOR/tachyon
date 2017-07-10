#include "game/flowgraph/boolean_condition.h"
#include "system/rtti.h"
#include "game/game_manager.h"
#include "game/game_types.h"
#include "system/core.h"
#include "system/assert.h"

namespace nytro
{
N_REGISTER_CLASS(BooleanConditionFlowGraphNode);

BooleanConditionFlowGraphNode::BooleanConditionFlowGraphNode()
{
	m_type = eType_Condition;
	addSlot("In", FlowGraphNodeSlot::eType_Input);
	addSlot("True", FlowGraphNodeSlot::eType_Output);
	addSlot("False", FlowGraphNodeSlot::eType_Output);
	addSlot("Value", FlowGraphNodeSlot::eType_VariableInput, true, Variant(false));
}

BooleanConditionFlowGraphNode::~BooleanConditionFlowGraphNode()
{
}

void BooleanConditionFlowGraphNode::process(FlowGraphNodeSlot* pFromSlot)
{
	Variant value;

	value = variableForSlot("Value");

	if ((i32)value != 0)
	{
		signalOutputSlot("True");
	}
	else
	{
		signalOutputSlot("False");
	}
}
}