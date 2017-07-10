#include "game/flowgraph/property_value.h"
#include "system/rtti.h"
#include "game/game_manager.h"
#include "game/game_types.h"
#include "system/core.h"
#include "system/assert.h"

namespace nytro
{
PropertyValueFlowGraphNode::PropertyValueFlowGraphNode()
{
	m_pComponent = nullptr;
}

PropertyValueFlowGraphNode::~PropertyValueFlowGraphNode()
{
}

void PropertyValueFlowGraphNode::prepareVariable()
{
	String propValue;

	if (m_pComponent)
	{
		//TODO:
		//propValue = m_pEntity->propertyValueAsText(m_propertyName.c_str());
	}

	m_variableValue = propValue;
}

void PropertyValueFlowGraphNode::onPropertyValueChanged(void* pPropertyData)
{
}
}