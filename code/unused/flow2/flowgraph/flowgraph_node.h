#pragma once
#include "base/array.h"
#include "base/map.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/variant.h"
#include "core/resource.h"
#include "game/types.h"
#include "game/systems/flowgraph/flowgraph_node_slot.h"

namespace engine
{
class FlowgraphNodeSlot;
struct FlowgraphBreakpoint;

//! A flow graph object, with input and output slots, and data/variable slots
class ENGINE_API FlowgraphNode : public ClassInfo
{
public:	
	FlowgraphNode();
	virtual ~FlowgraphNode();

	virtual void process(FlowgraphNodeSlot* pFromSlot);
	virtual void setData(FlowgraphNodeData* pData) { m_pData = pData; }
	FlowgraphNodeSlot* slot(const String& aName) { return m_slots[aName]; }
	FlowgraphNodeData* data() const { return m_pData; }
	const Map<String, FlowgraphNodeSlot*>& slots() const { return m_slots; }
	void deleteSlots();
	
protected:
	FlowgraphNodeData* m_pData;
	Map<String, FlowgraphNodeSlot*> m_slots;
};

#define E_BEGIN_FLOWGRAPH_NODE(theClassName, the)

}