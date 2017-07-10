#include "game/systems/flowgraph_node.h"
#include "core/core.h"
#include "game/types.h"
#include "game/systems/flowgraph_system.h"
#include "game/systems/flowgraph_base_nodes.h"

namespace engine
{
B_REGISTER_CLASS(FlowGraphNode);

FlowGraphNode::FlowGraphNode()
{
}

FlowGraphNode::~FlowGraphNode()
{
	deleteSlots();
}

void FlowGraphNode::process(FlowGraphNodeSlot* pFromSlot)
{
}

void FlowGraphNode::addSlot(
						const char* pName,
						FlowGraphNodeSlot::EType aType,
						bool bVisible,
						Variant aDefaultValue,
						u32 aMaxLinkCount)
{
	if (m_slots[(char*)pName])
	{
		return;
	}

	FlowGraphNodeSlot* pSlot = new FlowGraphNodeSlot();

	pSlot->setNode(this);
	pSlot->setVisible(bVisible);
	pSlot->setDefaultValue(aDefaultValue);
	pSlot->setType(aType);
	pSlot->m_maxLinkCount = aMaxLinkCount;
	m_slots[(char*)pName] = pSlot;
}

Variant FlowGraphNode::variableForSlot(const char* pSlotName)
{
	if (m_slots.find((char*)pSlotName) == m_slots.end())
	{
		return Variant();
	}
	//TODO: optimize m_slots[(char*)pSlotName] to an iter
	if (!m_slots[(char*)pSlotName]->m_links.isEmpty())
	{
		FlowGraphVariableNode* pNode = (FlowGraphVariableNode*)m_slots[(char*)pSlotName]->m_links[0];

		if (pNode->type() == eType_Variable)
		{
			pNode->prepareVariable();

			return pNode->variableValue();
		}
	}

	return Variant(VariantType::None);
}

void FlowGraphNode::deleteSlots()
{
	Map<char*, FlowGraphNodeSlot*>::Iterator iter = m_slots.begin();

	while (iter != m_slots.end())
	{
		if (iter->value)
		{
			delete iter->value;
		}

		++iter;
	}

	m_slots.clear();
}

void FlowGraphNode::signalOutputSlot(const char* pSlotName)
{
	signalOutputSlot(m_slots[(char*)pSlotName]);
}

void FlowGraphNode::signalOutputSlot(FlowGraphNodeSlot* pSlot)
{
	if (pSlot)
	{
		pSlot->m_bSignalOut = true;
	}
}

size_t FlowGraphNode::removeObjectRef(Rtti* pObj)
{
	return 0;
}
}