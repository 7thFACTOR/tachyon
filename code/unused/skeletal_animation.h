#pragma once
//*****************************************************************************
// Nytro Game Engine
//
// (C) Copyright 7thFACTOR Entertainment
// All rights reserved
//*****************************************************************************
#include "system/array.h"
#include "system/defines.h"
#include "system/types.h"
#include "game/render/components/transform.h"
#include "system/resource.h"

namespace nytro
{
class Animation;
class MeshData;
class Material;

//! Skinned model skeleton animation
class N_API SkeletonAnimation : public Resource
{
public:
	static const int kSkeletonAnimationFileVersion = 1;

	SkeletonAnimation();
	virtual ~SkeletonAnimation();

	//! load model animation file
	bool load(const char* pFilename = nullptr);
	//! unload animation data
	bool unload();
	//! free animation data
	void free();

	//! \return the bone animations
	const Array<Animation*>& boneAnimations() const;
	//! \return animation total time in seconds
	f32 animationTotalTime() const;

protected:
	//! bone animation, key is the bone name
	Array<Animation*> m_boneAnimations;
	f32 m_animationTotalTime;
};

//! An animation set for a skeleton
class N_API SkeletonAnimationSet : public Resource
{
public:
	SkeletonAnimationSet();
	virtual ~SkeletonAnimationSet();

	//! load animation set file
	bool load(const char* pFilename = nullptr);
	//! unload animation set data
	bool unload();
	//! free animation set data
	void free();

	//! \return first animation, used when creating the skeleton scene objects
	SkeletonAnimation* firstAnimation() const;
	//! \return animations
	const Array<SkeletonAnimation*>& animations() const;

protected:
	//! skinned model animations (run,walk,jump,idle, etc.), key is animation name
	Array<SkeletonAnimation*> m_animations;
};
}