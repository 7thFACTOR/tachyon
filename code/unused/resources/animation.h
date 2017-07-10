#pragma once
#include "base/string.h"
#include "base/map.h"
#include "core/resource.h"

namespace engine
{
class Animation;

//! An animations resource file, holding a set of animations
class E_API AnimationSetResource : public Resource
{
public:
	B_RUNTIME_CLASS;
	AnimationSetResource();
	virtual ~AnimationSetResource();

	//! load animation set from file
	bool load(Stream* pStream);
	//! unload animation set
	bool unload();
	//! free animation set
	void free();
	//! \return animations
	const Map<String, Animation*>& animations() const;

protected:
	//! animations for object names
	Map<String, Animation*> m_animations;
};
}