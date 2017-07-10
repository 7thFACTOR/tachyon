#include "resources/animation.h"
#include "render/animation.h"
#include "base/assert.h"
#include "base/logger.h"
#include "core/core.h"
#include "core/resource_manager.h"
#include "base/rtti.h"

namespace engine
{
using namespace base;

B_REGISTER_DERIVED_TAGGED_CLASS(AnimationSetResource, Resource, ResourceTypes::AnimationSet);

AnimationSetResource::AnimationSetResource()
{
}

AnimationSetResource::~AnimationSetResource()
{
	free();
}

bool AnimationSetResource::load(Stream* pStream)
{
	//XmlDocument xml;

	//if (!xml.load(pFilename))
	//{
	//	return false;
	//}

	//if (!xml.isSuccess())
	//{
	//	N_LOG_ERROR("Error while loading animation set XML file: '%s': at line %d: %s",
	//		pFilename,
	//		xml.getCurrentErrorLine(),
	//		xml.getErrorText().c_str());

	//	return false;
	//}

	//unload();
	//Resource::load(pFilename);
	//XmlElement* pAnimationsElem = xml.getRoot().getChild("animations");

	//for (size_t i = 0, iCount = pAnimationsElem->getChildren().size(); i < iCount; ++i)
	//{
	//	XmlElement* pAnimationElem = pAnimationsElem->getChildren()[i];
	//	Animation* pAnimation = new Animation;
	//	N_ASSERT(pAnimation);

	//	if (!pAnimation)
	//	{
	//		continue;
	//	}

	//	pAnimation->loadFromXmlElements(pAnimationElem);
	//	m_animations[pAnimationElem->getAttributeValue("name")] = pAnimation;
	//}

	return true;
}

bool AnimationSetResource::unload()
{
	free();

	return true;
}

void AnimationSetResource::free()
{
	Map<String, Animation*>::Iterator iter = m_animations.begin();

	while (iter != m_animations.end())
	{
		B_ASSERT(iter->value);
		B_SAFE_DELETE(iter->value);
		++iter;
	}

	m_animations.clear();
}

const Map<String, Animation*>& AnimationSetResource::animations() const
{
	return m_animations;
}
}