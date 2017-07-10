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

//! A flow graph object used for visual comments placed into the graph
class ENGINE_API FlowGraphCommentNode : public FlowgraphNode
{
public:
	FlowGraphCommentNode();
	virtual ~FlowGraphCommentNode()
	{}

protected:
	String m_commentText;
};
}