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
struct FlowGraphBreakpoint;

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