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
class ENGINE_API Flowgraph
{
public:
	B_DECLARE_CLASS(Flowgraph);
	Flowgraph();
	virtual ~Flowgraph();
	void free();
	void setResource(ResourceId res);
	virtual void start();
	virtual void update();
	const Array<FlowgraphBreakpoint*>& getDebugBreakPoints() const { return breakPoints; }

protected:
	FlowgraphBreakpoint* findDebugBreakpoint(FlowgraphNode* node);
	
	ResourceId flowgraph;
	Array<FlowgraphNode*> nodes;
	FlowgraphNodeSlot* currentSlot;
	Array<FlowgraphBreakpoint*> breakPoints;
	u32 maxRunTimeMsec;
};

}