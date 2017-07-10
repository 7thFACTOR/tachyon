#pragma once
#include "base/array.h"
#include "base/map.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/variant.h"
#include "core/resource.h"
#include "game/types.h"
#include "game/systems/flowgraph_node.h"

namespace engine
{
class FlowGraphNode;
class FlowGraphNodeSlot;
struct FlowGraphDebugBreakpoint;

//! A flow graph variable object, used to store or retrieve data from/to a variable variant
class E_API FlowGraphVariableNode: public FlowGraphNode
{
public:
	FlowGraphVariableNode();
	virtual ~FlowGraphVariableNode()
	{}

	virtual void prepareVariable()
	{};
	const Variant& variableValue() const
	{
		return m_variableValue;
	}
	void setVariableValue(const Variant& rValue)
	{
		m_variableValue = rValue;
	}
	EType type() const
	{
		return m_type;
	}
	void setType(EType aType)
	{
		m_type = aType;
	}

protected:
	Variant m_variableValue;
	EType m_type;
};

//! A flow graph object used for visual comments placed into the graph
class E_API FlowGraphCommentNode : public FlowGraphNode
{
public:
	FlowGraphCommentNode();
	virtual ~FlowGraphCommentNode()
	{}

protected:
	String m_commentText;
};
}