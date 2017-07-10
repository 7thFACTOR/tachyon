#include "core/core.h"
#include "game/types.h"
#include "game/resources/flowgraph_resource.h"
#include "game/systems/flowgraph/flowgraph.h"
#include "game/systems/flowgraph/flowgraph_node.h"
#include "game/systems/flowgraph/flowgraph_node_slot.h"
#include "game/systems/flowgraph/flowgraph_system.h"
#include "game/systems/flowgraph/flowgraph_breakpoint.h"

namespace engine
{
B_REGISTER_CLASS(Flowgraph);

Flowgraph::Flowgraph()
{
	maxRunTimeMsec = 300;
}

Flowgraph::~Flowgraph()
{
}

void Flowgraph::free()
{
}

void Flowgraph::start()
{
	FlowgraphNode* pTriggerNode = nullptr;

	if (pTriggerNode)
	{
		auto iter = pTriggerNode->slots().find("In");
		
		if (iter == pTriggerNode->slots().end())
		{
			return;
		}

		FlowgraphNodeSlot* pSlot = pTriggerNode->slot("In");
	}
	else
	{
		// find the first action node
		for (size_t i = 0, iCount = nodes.size(); i < iCount; ++i)
		{
			// if this object is an action, run it
			if (nodes[i]->data()->definition->domain == FlowgraphNodeDomain::Logic)
			{
				//TODO: optimize to iter
				if (nodes[i]->slots().find("In") == nodes[i]->slots().end())
				{
					return;
				}

				if(!nodes[i]->slot("In"))
				{
					return;
				}

				// init the running of this slot chain
				currentSlot = nodes[i]->slot("In");

				return;
			}
		}
	}
}

void Flowgraph::update()
{
	Array<FlowgraphNodeSlot*> dirtySlots;
	// run from this slot over and over until:
	// 1. we find a loopable situation (visit to a dirty/already visited slot)
	// 2. we dont have any other output slots fired
	// 3. a time limit is reached (milliseconds)
	// 4. a delayed node/slot has been reached
	u32 lastLimitTime = tickCount();

	while (currentSlot)
	{
		FlowgraphNodeSlot* pSlot = currentSlot;

		// reset signaling
		auto iterSlots = pSlot->node()->slots().begin();

		while (iterSlots != pSlot->node()->slots().end())
		{
			if (iterSlots->value->definition()->mode == FlowgraphNodeSlotMode::Out)
			{
			}

			++iterSlots;
		}

		FlowgraphBreakpoint* pBreakPoint = findDebugBreakpoint(pSlot->node());

		if (pBreakPoint && pBreakPoint->pfnUserCallback)
		{
			pBreakPoint->pfnUserCallback(pBreakPoint, false);
		}

		// process and mark out slots
		pSlot->node()->process(pSlot);

		if (pBreakPoint && pBreakPoint->pfnUserCallback)
		{
			pBreakPoint->pfnUserCallback(pBreakPoint, true);
		}

		// for all outputs that do signal out, add them as concurrent running slots
		iterSlots = pSlot->node()->slots().begin();

		while (iterSlots != pSlot->node()->slots().end())
		{
			if (iterSlots->value->definition()->mode == FlowgraphNodeSlotMode::Out)
			{
				currentSlot =iterSlots->value;
			}

			++iterSlots;
		}
	}
}

FlowgraphBreakpoint* Flowgraph::findDebugBreakpoint(FlowgraphNode* pNode)
{
	for (size_t i = 0; i < breakPoints.size(); ++i)
	{
		if (breakPoints[i]->pNode == pNode)
		{
			return breakPoints[i];
		}
	}

	return nullptr;
}

}