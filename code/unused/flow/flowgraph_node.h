#pragma once
#include "base/array.h"
#include "base/map.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/variant.h"
#include "core/resource.h"
#include "game/types.h"
#include "game/systems/flowgraph_node_slot.h"

namespace engine
{
class FlowGraphNodeSlot;
struct FlowGraphDebugBreakpoint;

//! A flow graph object, with input and output slots, and data/variable slots
class E_API FlowGraphNode: public Rtti
{
public:
	friend class FlowGraph;
	B_RUNTIME_CLASS
	
	enum EType
	{
		eType_Action,
		eType_Condition,
		eType_Variable,
		eType_Comment,
		eType_Custom
	};

	FlowGraphNode();
	virtual ~FlowGraphNode();

	virtual void process(FlowGraphNodeSlot* pFromSlot);
	void addSlot(
			const char* pName,
			FlowGraphNodeSlot::EType aType,
			bool bVisible = true,
			Variant aDefaultValue = 0,
			u32 aMaxLinkCount = 0);
	Variant variableForSlot(const char* pSlotName);
	FlowGraphNodeSlot* slot(const char* pName)
	{
		return m_slots[(char*)pName];
	}
	const Map<char*, FlowGraphNodeSlot*>& slots() const
	{
		return m_slots;
	}
	void deleteSlots();
	void signalOutputSlot(const char* pSlotName);
	void signalOutputSlot(FlowGraphNodeSlot* pSlot);
	//! if this graph object has references to the pRef object,
	//! it must remove those from its internal data, when this function is called by
	//! the flow graph editor
	virtual size_t removeObjectRef(Rtti* pRef);
	EType type() const
	{
		return m_type;
	}
	void setType(EType aType)
	{
		m_type = aType;
	}
	
protected:
	EType m_type;
	Map<char*, FlowGraphNodeSlot*> m_slots;
	String m_customName, m_source, m_description;
	bool m_bRenderPreview;
};
}