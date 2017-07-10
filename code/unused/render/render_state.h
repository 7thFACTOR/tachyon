#pragma once
#include "base/array.h"
#include "base/string.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/variant.h"
#include "base/math/util.h"
#include "core/defines.h"

namespace engine
{
using namespace base;

class GpuProgram;
class RenderLayer;

//! The render state types
namespace RenderStateTypes
{
	enum RenderStateType
	{
		None,
		AlphaBlendEnable,
		AlphaRef,
		AlphaTestEnable,
		ColorWriteEnable,
		DepthBias,
		DestBlend,
		FillMode,
		SlopeScaleDepthBias,
		SrcBlend,
		StencilEnable,
		StencilFail,
		StencilFunc,
		StencilMask,
		StencilPass,
		StencilRef,
		StencilWriteMask,
		StencilDepthFail,
		DepthTestEnable,
		DepthFunc,
		DepthWriteEnable,
		CullMode,
		
		_Count
	};
}
typedef RenderStateTypes::RenderStateType RenderStateType;

class E_API RenderState
{
public:
	RenderState(RenderStateType aType = RenderStateTypes::None, const Variant& aValue = (i32)0);
	virtual ~RenderState();

	//! the render state type
	RenderStateType m_type;
	//! the render state value, depends on the render state type (can be f32, double, word etc.)
	Variant m_value;
};
}