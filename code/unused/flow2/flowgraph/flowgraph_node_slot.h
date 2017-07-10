#pragma once
#include "base/array.h"
#include "base/map.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/variant.h"
#include "core/resource.h"
#include "game/types.h"
#include "game/resources/flowgraph_resource.h"

namespace engine
{
class FlowgraphNode;
struct FlowgraphBreakpoint;

//! A flow graph object slot, it can be an input, output or variable slot
class ENGINE_API FlowgraphNodeSlot : public ClassInfo
{
public:
	FlowgraphNodeSlot()
		: m_bExecuted(false)
		, m_pDefinition(nullptr)
		, m_pData(nullptr)
		, m_pNode(nullptr)
	{}

	~FlowgraphNodeSlot()
	{}

	FlowgraphNode* node() const { return m_pNode; }
	FlowgraphNodeSlotDefinition* definition() const { return m_pDefinition; }
	FlowgraphNodeSlotData* data() const { return m_pData; }
	bool isExecuted() const { return m_bExecuted; }
	
	void setNode(FlowgraphNode* pNode) { m_pNode = pNode; }
	void setDefinition(FlowgraphNodeSlotDefinition* pDef) { m_pDefinition = pDef; }
	void setData(FlowgraphNodeSlotData* pData) { m_pData = pData; }
	void setExecuted(bool bExecuted) { m_bExecuted = bExecuted; }

protected:
	bool m_bExecuted;
	FlowgraphNodeSlotDefinition* m_pDefinition;
	FlowgraphNodeSlotData* m_pData;
	FlowgraphNode* m_pNode;
};
}