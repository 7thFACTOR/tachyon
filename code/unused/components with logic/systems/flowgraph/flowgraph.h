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
class Flowgraph;
class FlowgraphNode;
class FlowgraphNodeSlot;
struct FlowgraphBreakpoint;

//! A flow graph is used to define logic for a triggered event
class E_API Flowgraph
{
public:
	B_DECLARE_CLASS(Flowgraph);
	Flowgraph();
	virtual ~Flowgraph();
	void free();
	void setResource(ResourceId res);
	virtual void start();
	virtual void update();
	const Array<FlowgraphBreakpoint*>& debugBreakPoints() const { return m_breakPoints; }

protected:
	FlowgraphBreakpoint* findDebugBreakpoint(FlowgraphNode* pNode);
	
	ResourceId m_flowgraph;
	Array<FlowgraphNode*> m_nodes;
	FlowgraphNodeSlot* m_pCurrentSlot;
	Array<FlowgraphBreakpoint*> m_breakPoints;
	u32 m_maxRunTimeMsec;
};
}