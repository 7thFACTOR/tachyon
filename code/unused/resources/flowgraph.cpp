#include "resources/flowgraph.h"
#include "game/systems/flowgraph_node.h"
#include "game/systems/flowgraph_node_slot.h"
#include "core/core.h"
#include "game/types.h"
#include "game/systems/flowgraph_system.h"

namespace engine
{
B_REGISTER_DERIVED_TAGGED_CLASS(FlowGraph, Resource, ResourceTypes::Flowgraph);

FlowGraph::FlowGraph()
{
	m_runLimitTimeMsec = 300;
}

FlowGraph::~FlowGraph()
{
}

bool FlowGraph::load(Stream* pStream)
{
	return true;
}

bool FlowGraph::unload()
{
	return true;
}

void FlowGraph::free()
{
}

void FlowGraph::start(FlowGraphNode* pTriggerEvent)
{
	if (pTriggerEvent)
	{
		Map<char*,FlowGraphNodeSlot*>::Iterator iter =
			pTriggerEvent->slots().find("In");
		
		if (iter == pTriggerEvent->slots().end())
		{
			return;
		}

		FlowGraphNodeSlot* pSlot = pTriggerEvent->slot("In");

		pSlot->setSignalDelayTimer(0);
		m_currentRunningSlots.append(pSlot);
	}
	else
	{
		// find the first action node
		for (size_t i = 0, iCount = m_nodes.size(); i < iCount; ++i)
		{
			// if this object is an action, run it
			if (m_nodes[i]->type() == FlowGraphNode::eType_Action)
			{
				//TODO: optimize to iter
				if (m_nodes[i]->slots().find("In") == m_nodes[i]->slots().end())
				{
					return;
				}

				if(!m_nodes[i]->slot("In"))
				{
					return;
				}

				// init the running of this slot chain
				m_nodes[i]->slot("In")->setSignalDelayTimer(0);
				m_currentRunningSlots.append(m_nodes[i]->slot("In"));

				return;
			}
		}
	}
}

void FlowGraph::process()
{
	Array<FlowGraphNodeSlot*> dirtySlots;
	Array<FlowGraphNodeSlot*>::Iterator iter = m_currentRunningSlots.begin();
	// run from this slot over and over until:
	// 1. we find a loopable situation (visit to a dirty/already visited slot)
	// 2. we dont have any other output slots fired
	// 3. a time limit is reached (milliseconds)
	// 4. a delayed node/slot has been reached
	u32 lastLimitTime = tickCount();

	while (iter != m_currentRunningSlots.end()
			&& tickCount() - lastLimitTime < m_runLimitTimeMsec)
	{
		FlowGraphNodeSlot* pSlot = *iter;

		// if we have a delay on the slot
		if (pSlot->m_signalDelay > 0)
		{
			pSlot->m_signalDelayTimer += gameDeltaTime();

			// if not ready yet, skip this slot
			if (pSlot->m_signalDelayTimer < pSlot->m_signalDelay)
			{
				++iter;
				continue;
			}
		}

		// reset signaling
		Map<char*, FlowGraphNodeSlot*>::Iterator iterSlots = pSlot->m_pNode->slots().begin();

		while (iterSlots != pSlot->m_pNode->slots().end())
		{
			if (iterSlots->value->type() == FlowGraphNodeSlot::eType_Output)
			{
				iterSlots->value->setSignalOut(false);
			}

			++iterSlots;
		}

		FlowGraphDebugBreakpoint* pDbgBrkPt = findDebugBreakpoint(pSlot->m_pNode);

		if (pDbgBrkPt && pDbgBrkPt->pfnUserCallback)
		{
			pDbgBrkPt->pfnUserCallback(pDbgBrkPt, false);
		}

		// process and mark out slots
		pSlot->m_pNode->process(pSlot);

		if (pDbgBrkPt && pDbgBrkPt->pfnUserCallback)
		{
			pDbgBrkPt->pfnUserCallback(pDbgBrkPt, true);
		}

		// for all outputs that do signal out, add them as concurrent running slots
		iterSlots = pSlot->m_pNode->slots().begin();

		while (iterSlots != pSlot->m_pNode->slots().end())
		{
			if (iterSlots->value->type() == FlowGraphNodeSlot::eType_Output
				&& iterSlots->value->isSignalOut())
			{
				m_currentRunningSlots.append(iterSlots->value);
			}

			++iterSlots;
		}

		// delete the ref to the slot, we don't need it
		iter = m_currentRunningSlots.erase(iter);
	}
}

Array<FlowGraphDebugBreakpoint*>& FlowGraph::debugBreakPoints()
{
	return m_debugBreakPoints;
}

FlowGraphDebugBreakpoint* FlowGraph::findDebugBreakpoint(FlowGraphNode* pNode)
{
	for (size_t i = 0; i < m_debugBreakPoints.size(); ++i)
	{
		if (m_debugBreakPoints[i]->pNode == pNode)
		{
			return m_debugBreakPoints[i];
		}
	}

	return nullptr;
}
}