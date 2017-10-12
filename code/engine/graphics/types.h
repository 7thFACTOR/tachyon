// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/string.h"
#include "core/defines.h"
#include "core/types.h"
#include <math.h>

namespace engine
{
using namespace base;

const u32 maxRenderTargetCount = 32;
typedef u32 VertexIndexType;
typedef u32 SystemTextureId;

enum class GraphicsApiType
{
	None,
	OpenGL,
	Vulkan,
	Direct3D,
	Metal,

	Count
};

enum class ShaderType
{
	Vertex,
	Pixel,
	Geometry,
	Domain,
	Hull,

	Count
};

enum class RenderState
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
	
	Count
};

//! Constant standard types
enum class GpuProgramConstantType
{
	Unknown,
	Float,
	Integer,
	Matrix,
	Vec2,
	Vec3,
	Color,
	Texture,

	WorldMatrix,
	WorldInverseMatrix,
	WorldInverseTransposeMatrix,
	ViewMatrix,
	ViewInverseMatrix,
	ViewInverseTransposeMatrix,
	ViewRotationMatrix,
	WorldViewMatrix,
	WorldViewInverseMatrix,
	WorldViewInverseTransposeMatrix,
	ProjectionMatrix,
	WorldProjectionMatrix,
	ViewProjectionMatrix,
	WorldViewProjectionMatrix,
	TextureMatrix,

	EngineTime,
	EngineFrameDeltaTime,
	UnitDomainRotatorTime,
	EngineFps,

	RenderTargetSize,
	ViewportSize,

	CameraFov,
	CameraNearPlane,
	CameraFarPlane,
	CameraUpAxis,
	CameraPosition,

	ColorTexture,
	DepthTexture,
	SystemTexture,

	Count
};

namespace GpuProgramOption
{
	enum
	{
		UseColorBuffer = B_BIT(0),
		UseShadowBuffer = B_BIT(1)
	};
}
typedef u32 GpuProgramOptions;

//! Render priority for surfaces
enum class RenderPriority
{
	//! these surfaces must be rendered at the very first moment, before everything else
	First,
	//! all opaque surfaces
	Opaque,
	//! all the surfaces that are transparent and use simple alpha
	Transparent,
	//! all surfaces that will blend additively
	Additive,
	//! these surfaces must be rendered at the very last moment, on top of everything else
	Last,
	
	Count
};

//! Full-scene anti-aliasing types
enum class AntialiasLevel
{
	None,
	Low,
	Medium,
	High
};

//! Blend type
enum class BlendType
{
	Zero,
	One,
	SrcColor,
	InvSrcColor,
	SrcAlpha,
	InvSrcAlpha,
	DstAlpha,
	InvDstAlpha,
	DstColor,
	InvDstColor,
	SrcAlphaSat
};

//! Comparing functions
enum class CompareFunction
{
	Never,
	Less,
	Equal,
	LessEqual,
	Greater,
	NotEqual,
	GreaterEqual,
	Always
};

//! Back-face culling modes
enum class CullMode
{
	None,
	CW,
	CCW
};

//! Primitive filling modes
enum class FillMode
{
	Point,
	Wireframe,
	Solid
};

//! Stencil operations
enum class StencilOperation
{
	Keep,
	Zero,
	Replace,
	IncrSat,
	DecrSat,
	Invert,
	Incr,
	Decr
};

enum class ClearBufferFlags
{
	None,
	All = ~0,
	Depth = B_BIT(0),
	Color = B_BIT(1),
	Stencil = B_BIT(2),
	DepthAndColor = Depth | Color
};
B_ENUM_AS_FLAGS(ClearBufferFlags);

enum class BlendOperation
{
	None,
	Add,
	Subtract,
	ReverseSubtract,
	Min,
	Max
};

enum class BlendMode
{
	Opaque,
	Additive,
	Alpha,
	PremultipliedAlpha
};

enum class TextureAddressType
{
	Repeat,
	Clamp
};

enum class TextureFilterType
{
	NearestMipMapNearest,
	LinearMipMapNearest,
	NearestMipMapLinear,
	LinearMipMapLinear,
	Nearest,
	Linear,
	Anisotropic
};

enum class DirtyMatrixFlags
{
	None,
	All = ~0,
	World = B_BIT(0),
	View = B_BIT(1),
	Projection = B_BIT(2),
	WorldInverse = B_BIT(3),
	WorldInverseTranspose = B_BIT(4),
	ViewInverse = B_BIT(5),
	ViewInverseTranspose = B_BIT(6),
	ViewRotation = B_BIT(7),
	WorldView = B_BIT(8),
	WorldViewInverse = B_BIT(9),
	WorldViewInverseTranspose = B_BIT(10),
	WorldProjection = B_BIT(11),
	ViewProjection = B_BIT(12),
	WorldViewProjection = B_BIT(13)
};
B_ENUM_AS_FLAGS(DirtyMatrixFlags);

enum class MatrixType
{
	World,
	View,
	Projection,
	Texture
};

enum class RenderPrimitive
{
	Points,
	Lines,
	LineStrip,
	Triangles,
	TriangleStrip
};

enum class GpuBufferType
{
	Indices = 0,
	Vertices,
	Constants,
	Custom,
	
	Count
};

enum class GpuBufferUsageType
{
	// the buffer is only accessible bu GPU
	Static,
	// the GPU can read, the CPU can write
	Dynamic,
	// data transfer from GPU to CPU
	Staging
};

enum class GpuBufferMapType
{
	Read,
	Write,
	ReadWrite
};

struct SystemTexture
{
	u32 id = 0;
	String name;
	ResourceId textureId = nullResourceId;
};

struct RenderTargetBlendState
{
	bool enable = false;
	BlendType srcBlend = BlendType::SrcColor;
	BlendType destBlend = BlendType::Zero;
	BlendOperation blendOp = BlendOperation::Add;
	BlendType srcBlendAlpha = BlendType::SrcAlpha;
	BlendType destBlendAlpha = BlendType::Zero;
	BlendOperation blendOpAlpha = BlendOperation::Add;
	u8 renderTargetWriteMask = ~0;

	bool operator == (const RenderTargetBlendState& other)
	{
		return enable == other.enable 
			&& srcBlend == other.srcBlend
			&& destBlend == other.destBlend
			&& blendOp == other.blendOp
			&& srcBlendAlpha == other.srcBlendAlpha
			&& destBlendAlpha == other.destBlendAlpha
			&& blendOpAlpha == other.blendOpAlpha
			&& renderTargetWriteMask == other.renderTargetWriteMask;
	}
};

struct BlendState
{
	bool alphaToCoverage = false;
	bool independentBlend = false;
	f32 alphaRef = 0.5f;
	RenderTargetBlendState renderTarget[maxRenderTargetCount];

	bool operator == (const BlendState& other)
	{
		bool rtBlendingOk = true;
		
		if (independentBlend)
		{
			for (int i = 0; i < maxRenderTargetCount; ++i)
			{
				if (!(renderTarget[i] == other.renderTarget[i]))
				{
					rtBlendingOk = false;
					break;
				}
			}
		}
		
		return alphaToCoverage == other.alphaToCoverage
			&& independentBlend == other.independentBlend
			&& fabs(alphaRef - other.alphaRef) < 0.0001f
			&& rtBlendingOk;
	}
};

struct RasterizerState
{
	FillMode fillMode = FillMode::Solid;
	CullMode cullMode = CullMode::CCW;
	i32 depthBias = 0;
	f32 depthBiasClamp = 0.0f;
	f32 slopeScaledDepthBias = 0.0f;
	bool depthClip = false;
	bool scissorTest = false;
	bool multisample = false;
	bool antialiasedLine = false;
	bool alphaTest = false;
	f32 alphaTestMinValue = 0.0f;

	bool operator == (const RasterizerState& other)
	{
		return fillMode == other.fillMode
		&& cullMode == other.cullMode
		&& depthBias == other.depthBias
		&& fabs(depthBiasClamp - other.depthBiasClamp) < 0.0001f
		&& fabs(slopeScaledDepthBias - other.slopeScaledDepthBias) < 0.0001f
		&& depthClip == other.depthClip
		&& scissorTest == other.scissorTest
		&& multisample == other.multisample
		&& antialiasedLine == other.antialiasedLine
		&& alphaTest == other.alphaTest
		&& fabs(alphaTestMinValue - other.alphaTestMinValue) < 0.0001f;
	}
};

struct DepthStencilOp
{
	StencilOperation failOp = StencilOperation::Replace;
	StencilOperation depthFailOp = StencilOperation::Replace;
	StencilOperation passOp = StencilOperation::Replace;
	CompareFunction function = CompareFunction::Always;

	bool operator == (const DepthStencilOp& other)
	{
		return failOp == other.failOp
			&& depthFailOp == other.depthFailOp
			&& passOp == other.passOp
			&& function == other.function;
	}
};

struct DepthStencilState
{
	bool depthTest = true;
	bool depthWrite = true;
	CompareFunction depthFunc = CompareFunction::LessEqual;
	bool stencil = false;
	u8 stencilReadMask = 0;
	u8 stencilWriteMask = 0;
	u8 stencilRef = 0;
	DepthStencilOp frontFace;
	DepthStencilOp backFace;

	bool operator == (const DepthStencilState& other)
	{
		return depthTest == other.depthTest
		&& depthWrite == other.depthWrite
		&& depthFunc == other.depthFunc
		&& stencil == other.stencil
		&& stencilReadMask == other.stencilReadMask
		&& stencilWriteMask == other.stencilWriteMask
		&& stencilRef == other.stencilRef
		&& frontFace == other.frontFace
		&& backFace == other.backFace;
	}
};

enum class GpuBufferElementType
{
	Unknown,
	Position,
	Normal,
	Tangent,
	Bitangent,
	Color,
	TexCoord,
	TessellationFactor,
	JointWeight,
	JointIndex,
	Index,
	Custom,
	InstanceTranslation,
	InstanceRotation,
	InstanceScale,
	InstanceColor,
	InstanceTransformMatrix,

	Count
};

enum class GpuBufferElementComponentType
{
	Float,
	Int8,
	Int16,
	Int32,

	Count
};

struct GpuBufferElement
{
	GpuBufferElement() {}
	GpuBufferElement(GpuBufferElementComponentType compType, u32 count, GpuBufferElementType elemType)
		: componentSize(0)
		, componentCount(count)
		, componentType(compType)
		, type(elemType)
		, offset(0)
		, index(0)
	{
		updateComponentSize();
	}

	inline u32 getElementSize() const
	{
		return componentSize * componentCount;
	}

	void updateComponentSize()
	{
		componentSize = 0;

		switch (componentType)
		{
		case GpuBufferElementComponentType::Float:
			componentSize = sizeof(f32); break;
		case GpuBufferElementComponentType::Int8:
			componentSize = sizeof(i8); break;
		case GpuBufferElementComponentType::Int16:
			componentSize = sizeof(i16); break;
		case GpuBufferElementComponentType::Int32:
			componentSize = sizeof(i32); break;
		}
	}

	GpuBufferElementType type = GpuBufferElementType::Position;
	GpuBufferElementComponentType componentType = GpuBufferElementComponentType::Float;
	u32 index = 0;
	u32 offset = 0;
	u32 componentSize = sizeof(f32);
	u32 componentCount = 3;
};

struct GpuBufferRange
{
	bool active = true;
	u32 baseVertexIndex = 0;
	u32 indexOffset = 0;
	u32 vertexCount = 0;
	u32 indexCount = 0;
};

struct VideoMode
{
	VideoMode() {}
	VideoMode(u32 newWidth, u32 newHeight)
		: width(newWidth)
		, height(newHeight)
	{}

	u32 width = 0;
	u32 height = 0;
	u32 bitsPerPixel = 32;
	u32 frequency = 0;
	bool fullScreen = false;
	u32 adapterIndex = 0;
};

}