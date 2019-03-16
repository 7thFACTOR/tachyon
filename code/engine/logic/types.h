// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/types.h"

namespace engine
{
using namespace base;
//! when a buffer in a ComponentPool reaches this number, another buffer is created and added to the pool with this component count
//! and so on, adding buffers when they get full, see the ComponentPool documentation for more info
const size_t componentCountPerArray = 10000;

typedef u32 EntityId;
typedef u64 ComponentTypeId;

const EntityId invalidEntityId = 0;
const ComponentTypeId invalidComponentTypeId = 0;

//! Built-in component types
enum StdComponentTypeIds
{
	StdComponentTypeId_Unknown = 0,
	
	//////////////////////////////////////////////////////////////////////////
	// General
	//////////////////////////////////////////////////////////////////////////
	StdComponentTypeId_Trigger,
	StdComponentTypeId_Flowgraph,
	StdComponentTypeId_Layer,
	StdComponentTypeId_Attributes,
	StdComponentTypeId_Script,
	
	//////////////////////////////////////////////////////////////////////////
	// Render
	//////////////////////////////////////////////////////////////////////////
	StdComponentTypeId_Transform,
	StdComponentTypeId_Camera,
	StdComponentTypeId_Mesh,
	StdComponentTypeId_MeshRenderer,
	StdComponentTypeId_MeshInstancer,
	StdComponentTypeId_Light,
	StdComponentTypeId_Terrain,
	StdComponentTypeId_Animation,
	StdComponentTypeId_Character,
	StdComponentTypeId_Precipitation,
	StdComponentTypeId_Particles,
	StdComponentTypeId_Flares,
	StdComponentTypeId_Sky,
	StdComponentTypeId_WaterPool,
	StdComponentTypeId_Ocean,
	StdComponentTypeId_River,
	StdComponentTypeId_Road,
	StdComponentTypeId_FirstPersonView,

	//////////////////////////////////////////////////////////////////////////
	// Physics
	//////////////////////////////////////////////////////////////////////////
	StdComponentTypeId_PhysicsBody,
	StdComponentTypeId_SphereCollider,
	StdComponentTypeId_CylinderCollider,
	StdComponentTypeId_CapsuleCollider,
	StdComponentTypeId_BoxCollider,
	StdComponentTypeId_PlaneCollider,
	StdComponentTypeId_ConvexCollider,
	StdComponentTypeId_MeshCollider,
	StdComponentTypeId_HeightFieldCollider,
	StdComponentTypeId_RayCollider,
	StdComponentTypeId_CylindricalJoint,
	StdComponentTypeId_UniversalJoint,
	StdComponentTypeId_DistanceJoint,
	StdComponentTypeId_FixedJoint,
	StdComponentTypeId_PointInPlaneJoint,
	StdComponentTypeId_PointOnLineJoint,
	StdComponentTypeId_PrismaticJoint,
	StdComponentTypeId_PulleyJoint,
	StdComponentTypeId_RevoluteJoint,
	StdComponentTypeId_SphericalJoint,
	StdComponentTypeId_Cloth,
	StdComponentTypeId_SoftBody,
	StdComponentTypeId_ForceField,
	StdComponentTypeId_PhysicsMaterial,

	//////////////////////////////////////////////////////////////////////////
	// Audio
	//////////////////////////////////////////////////////////////////////////
	StdComponentTypeId_AudioSource,
	StdComponentTypeId_AudioSpectator,
	StdComponentTypeId_AmbientMusic,

	//////////////////////////////////////////////////////////////////////////
	// Input
	//////////////////////////////////////////////////////////////////////////
	StdComponentTypeId_KeyboardController,
	StdComponentTypeId_MouseController,
	StdComponentTypeId_JoystickController,
	StdComponentTypeId_GamePadController,
	StdComponentTypeId_TouchController,
	StdComponentTypeId_SensorController,
	StdComponentTypeId_WheelController,
	StdComponentTypeId_InputMap,
	StdComponentTypeId_Flyby,

	//////////////////////////////////////////////////////////////////////////
	// PostProcess
	//////////////////////////////////////////////////////////////////////////
	//TODO: maybe these should be settings in the PostProcess component
	StdComponentTypeId_Blur,
	StdComponentTypeId_DepthOfField,
	StdComponentTypeId_ToneMapping,
	StdComponentTypeId_Fog,
	StdComponentTypeId_SSAO,
	StdComponentTypeId_SSDO,
	StdComponentTypeId_SSR,
	StdComponentTypeId_Bloom,
	StdComponentTypeId_MotionBlur,
	StdComponentTypeId_ColorCorrection,
	StdComponentTypeId_RimLight,
	StdComponentTypeId_Vignette,
	StdComponentTypeId_Sharpen,
	StdComponentTypeId_SunShafts,
	StdComponentTypeId_Glow,
	StdComponentTypeId_Antialiasing,
	StdComponentTypeId_Noise,
	StdComponentTypeId_Sepia,
	StdComponentTypeId_Fade,

	//////////////////////////////////////////////////////////////////////////
	// Ui
	//////////////////////////////////////////////////////////////////////////
	StdComponentTypeId_Canvas,
	StdComponentTypeId_Text,
	StdComponentTypeId_Button,
	StdComponentTypeId_Slider,
	StdComponentTypeId_Image,
	StdComponentTypeId_InputField,
	StdComponentTypeId_Toggle,

	StdComponentTypeId_Count
};

enum StdComponentSystemIds
{
	StdComponentSystemId_Input,
	StdComponentSystemId_Audio,
	StdComponentSystemId_Raycast,
	StdComponentSystemId_Render,
	StdComponentSystemId_Ui,

	StdComponentSystemId_Count
};

}