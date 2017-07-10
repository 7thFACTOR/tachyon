#include "game/systems/flowgraph_node.h"
#include "game/systems/flowgraph_base_nodes.h"
#include "core/core.h"
#include "game/types.h"
#include "game/systems/flowgraph_system.h"

namespace engine
{
FlowGraphVariableNode::FlowGraphVariableNode()
{
	m_variableValue = 0;
	m_type = eType_Variable;
}

//---

FlowGraphCommentNode::FlowGraphCommentNode()
{
	m_commentText = "";
	m_type = eType_Comment;
}
}