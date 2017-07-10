#include "game/flowgraph/expression_condition.h"
#include "system/rtti.h"
#include "game/game_manager.h"
#include "game/game_types.h"
#include "system/core.h"
#include "system/assert.h"

namespace nytro
{
ExpressionConditionFlowGraphNode::ExpressionConditionFlowGraphNode()
{
	m_type = eType_Condition;
	addSlot("In", FlowGraphNodeSlot::eType_Input);
	addSlot("A == B", FlowGraphNodeSlot::eType_Output);
	addSlot("A >= B", FlowGraphNodeSlot::eType_Output);
	addSlot("A <= B", FlowGraphNodeSlot::eType_Output);
	addSlot("A > B", FlowGraphNodeSlot::eType_Output);
	addSlot("A < B", FlowGraphNodeSlot::eType_Output);
	addSlot("A != B", FlowGraphNodeSlot::eType_Output);
	addSlot("A", FlowGraphNodeSlot::eType_VariableInput, true, Variant(0));
	addSlot("B", FlowGraphNodeSlot::eType_VariableInput, true, Variant(0));
}

ExpressionConditionFlowGraphNode::~ExpressionConditionFlowGraphNode()
{
}

void ExpressionConditionFlowGraphNode::process(FlowGraphNodeSlot* pFromSlot)
{
	Variant A, B;

	A = variableForSlot("A");
	B = variableForSlot("B");

	if (A == B)
	{
		signalOutputSlot("A == B");
	}

	if (A >= B)
	{
		signalOutputSlot("A >= B");
	}

	if (A <= B)
	{
		signalOutputSlot("A <= B");
	}

	if (A > B)
	{
		signalOutputSlot("A > B");
	}

	if (A < B)
	{
		signalOutputSlot("A < B");
	}

	if (A != B)
	{
		signalOutputSlot("A != B");
	}
}
}