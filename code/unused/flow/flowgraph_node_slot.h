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
class FlowGraphNode;
struct FlowGraphDebugBreakpoint;

//! A flow graph object slot, it can be an input, output or variable slot
class E_API FlowGraphNodeSlot: public Rtti
{
public:
	friend class FlowGraphNode;
	friend class FlowGraph;
	B_RUNTIME_CLASS

	enum EType
	{
		//! this slot is an input slot, receives activation signals
		eType_Input,
		//! this slot is an output slot, send activation signals
		eType_Output,
		//! this slot accepts a variable as input
		eType_VariableInput,
		//! this slot outputs a variable
		eType_VariableOutput
	};

	FlowGraphNodeSlot();
	virtual ~FlowGraphNodeSlot();

	const Array<FlowGraphNodeSlot*>& links() const;
	const Map<FlowGraphNodeSlot*, bool>& enabledOutputLinks() const;
	bool isVisible() const;
	bool isSignalOut() const;
	FlowGraphNode* node() const;
	const Variant& defaultValue() const;
	f32 signalDelay() const;
	f32 signalDelayTimer() const;
	u32 maxLinkCount() const;
	EType type() const;
	
	void enableOutputLink(FlowGraphNodeSlot* pSlot, bool bEnable);
	void setVisible(bool bValue);
	void setSignalOut(bool bValue);
	void setNode(FlowGraphNode* pNode);
	void setDefaultValue(const Variant& rValue);
	void setSignalDelay(f32 aValue);
	void setSignalDelayTimer(f32 aValue);
	void setType(EType aType);

protected:
	EType m_type;
	Array<FlowGraphNodeSlot*> m_links;
	Map<FlowGraphNodeSlot*, bool> m_bEnabledOutputLinks;
	bool m_bVisible, m_bSignalOut;
	FlowGraphNode* m_pNode;
	Variant m_defaultValue;
	f32 m_signalDelay, m_signalDelayTimer;
	u32 m_maxLinkCount;
};
}