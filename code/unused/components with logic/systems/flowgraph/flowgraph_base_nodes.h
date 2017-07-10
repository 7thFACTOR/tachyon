#pragma once
#include "base/array.h"
#include "base/map.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/variant.h"
#include "core/resource.h"
#include "game/types.h"
#include "game/systems/flowgraph/flowgraph_node.h"

namespace engine
{
class FlowgraphNode;
class FlowgraphNodeSlot;
struct FlowgraphBreakpoint;

//! A flow graph variable object, used to store or retrieve data from/to a variable variant
class E_API FlowGraphVariableNode : public FlowgraphNode
{
public:
	FlowGraphVariableNode()
	{}
	
	virtual ~FlowGraphVariableNode()
	{}
	
	const Variant& value() const { return m_value; }
	FlowgraphNodeSlotDataType type() const { return m_type; }
	
	void setValue(const Variant& rValue) { m_value = rValue; }
	void setType(FlowgraphNodeSlotDataType aType) { m_type = aType; }

protected:
	FlowgraphNodeSlotDataType m_type;
	Variant m_value;
};

//! A flow graph object used for visual comments placed into the graph
class E_API FlowGraphCommentNode : public FlowgraphNode
{
public:
	FlowGraphCommentNode()
	{}
	
	virtual ~FlowGraphCommentNode()
	{}

	void setText(const String& str) { m_commentText = str; }
	const String& text() const { return m_commentText; }

protected:
	String m_commentText;
};

}