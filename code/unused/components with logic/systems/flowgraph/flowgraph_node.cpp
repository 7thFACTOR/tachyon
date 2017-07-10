#include "core/core.h"
#include "game/types.h"
#include "game/systems/flowgraph/flowgraph_node.h"
#include "game/systems/flowgraph/flowgraph_system.h"
#include "game/systems/flowgraph/flowgraph_base_nodes.h"

namespace engine
{
FlowgraphNode::FlowgraphNode()
{}

FlowgraphNode::~FlowgraphNode()
{
	deleteSlots();
}

void FlowgraphNode::process(FlowgraphNodeSlot* pFromSlot)
{}

void FlowgraphNode::deleteSlots()
{
	auto iter = m_slots.begin();

	while (iter != m_slots.end())
	{
		if (iter->value)
		{
			delete iter->value;
		}

		++iter;
	}

	m_slots.clear();
}

}