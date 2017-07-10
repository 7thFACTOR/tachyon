#include "base/types.h"

namespace engine
{
struct FlowgraphBreakpoint
{
	typedef void (*TPfnCallback)(FlowgraphBreakpoint* pBreakPoint, bool bAfterNodeExecution);

	FlowgraphBreakpoint()
		: pFlowGraph(nullptr)
		, pNode(nullptr)
		, bEnabled(true)
		, bHit(false)
	{}

	Flowgraph* pFlowGraph;
	FlowgraphNode* pNode;
	bool bEnabled;
	bool bHit;
	TPfnCallback pfnUserCallback;
};
}