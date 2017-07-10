#include "render/render_state.h"

namespace engine
{
RenderState::RenderState(RenderStateType aType, const Variant& aValue)
{
	m_type = aType;
	m_value = aValue;
}

RenderState::~RenderState()
{
}

}