#pragma once
#include "base/array.h"
#include "base/map.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/variant.h"
#include "core/resource.h"
#include "game/types.h"

namespace engine
{
class FlowGraph;
class FlowGraphNode;
class FlowGraphNodeSlot;
struct FlowGraphDebugBreakpoint;

struct FlowGraphDebugBreakpoint
{
	typedef void (*TPfnCallback)(FlowGraphDebugBreakpoint* pBreakPoint, bool bAfterNodeExecution);

	FlowGraphDebugBreakpoint()
		: pFlowGraph(nullptr)
		, pNode(nullptr)
		, bEnabled(true)
		, bHit(false)
	{}

	FlowGraph* pFlowGraph;
	FlowGraphNode* pNode;
	bool bEnabled;
	bool bHit;
	TPfnCallback pfnUserCallback;
};

//! A flow graph is used to define logic for a triggered event
class E_API FlowGraph: public Resource
{
public:
	B_RUNTIME_CLASS;
	FlowGraph();
	virtual ~FlowGraph();
	bool load(Stream* pStream);
	bool unload();
	void free();
	virtual void start(FlowGraphNode* pTrigger = nullptr);
	virtual void process();
	Array<FlowGraphDebugBreakpoint*>& debugBreakPoints();

protected:
	FlowGraphDebugBreakpoint* findDebugBreakpoint(FlowGraphNode* pNode);

	Array<FlowGraphNode*> m_nodes;
	Array<FlowGraphNodeSlot*> m_currentRunningSlots;
	Array<FlowGraphDebugBreakpoint*> m_debugBreakPoints;
	u32 m_runLimitTimeMsec;
};
}