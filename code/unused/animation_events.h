#pragma once
//*****************************************************************************
// Nytro Game Engine
//
// (C) Copyright 7thFACTOR Entertainment
// All rights reserved
//*****************************************************************************
#include "system/string.h"
#include "system/defines.h"
#include "game/animation/animation.h"

namespace nytro
{
class Transform;
class FlowGraph;
class SoundSource;

//! A sound animation event
class N_API SoundAnimationEvent : public AnimationEvent
{
public:
	SoundAnimationEvent();
	virtual ~SoundAnimationEvent();
	virtual void onTrigger();

protected:
	String m_soundFile, m_parentName;
	f32 m_volume;
	bool m_bFollowParent;
	Transform* m_pParent;
	SoundSource* m_pSound;
};

//! A flow graph event animation event
class N_API FlowGraphAnimationEvent : public AnimationEvent
{
public:
	FlowGraphAnimationEvent();
	virtual ~FlowGraphAnimationEvent();
	virtual void onTrigger();

protected:
	FlowGraph* m_pGraph;
};
}