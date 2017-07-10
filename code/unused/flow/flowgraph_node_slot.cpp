#include "game/systems/flowgraph_node_slot.h"
#include "core/core.h"
#include "game/types.h"
#include "game/systems/flowgraph_system.h"

namespace engine
{
B_REGISTER_CLASS(FlowGraphNodeSlot);

FlowGraphNodeSlot::FlowGraphNodeSlot()
{
	m_bVisible = true;
	m_bSignalOut = false;
	m_pNode = nullptr;
	m_defaultValue = 0;
	m_signalDelay = 0;
	m_signalDelayTimer = 0;
	m_maxLinkCount = 0;
}

FlowGraphNodeSlot::~FlowGraphNodeSlot()
{
}

const Array<FlowGraphNodeSlot*>& FlowGraphNodeSlot::links() const
{
	return m_links;
}

const Map<FlowGraphNodeSlot*, bool>& FlowGraphNodeSlot::enabledOutputLinks() const
{
	return m_bEnabledOutputLinks;
}

bool FlowGraphNodeSlot::isVisible() const
{
	return m_bVisible;
}

bool FlowGraphNodeSlot::isSignalOut() const
{
	return m_bSignalOut;
}

FlowGraphNode* FlowGraphNodeSlot::node() const
{
	return m_pNode;
}

const Variant& FlowGraphNodeSlot::defaultValue() const
{
	return m_defaultValue;
}

f32 FlowGraphNodeSlot::signalDelay() const
{
	return m_signalDelay;
}

f32 FlowGraphNodeSlot::signalDelayTimer() const
{
	return m_signalDelayTimer;
}

u32 FlowGraphNodeSlot::maxLinkCount() const
{
	return m_maxLinkCount;
}

FlowGraphNodeSlot::EType FlowGraphNodeSlot::type() const
{
	return m_type;
}

void FlowGraphNodeSlot::setType(EType aType)
{
	m_type = aType;
}

void FlowGraphNodeSlot::enableOutputLink(FlowGraphNodeSlot* pSlot, bool bEnable)
{
	m_bEnabledOutputLinks[pSlot] = bEnable;
}

void FlowGraphNodeSlot::setVisible(bool bValue)
{
	m_bVisible = bValue;
}

void FlowGraphNodeSlot::setSignalOut(bool bValue)
{
	m_bSignalOut = bValue;
}

void FlowGraphNodeSlot::setNode(FlowGraphNode* pNode)
{
	m_pNode = pNode;
}

void FlowGraphNodeSlot::setDefaultValue(const Variant& rValue)
{
	m_defaultValue = rValue;
}

void FlowGraphNodeSlot::setSignalDelay(f32 aValue)
{
	m_signalDelay = aValue;
}

void FlowGraphNodeSlot::setSignalDelayTimer(f32 aValue)
{
	m_signalDelayTimer = aValue;
}
}