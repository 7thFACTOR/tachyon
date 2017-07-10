#include "game/animation/skeletal_anim_data.h"
#include "game/animation/animation.h"
#include "system/file_manager.h"
#include "system/core.h"
#include "system/file.h"
#include "system/logger.h"
#include "system/assert.h"
#include "system/resource_manager.h"
#include "math/math_convert.h"

namespace nytro
{
SkeletonAnimation::SkeletonAnimation()
{
	m_animationTotalTime = 0;
}

SkeletonAnimation::~SkeletonAnimation()
{
}

bool SkeletonAnimation::load(const char* pFilename)
{
	unload();
	Resource::load(pFilename);
	File* pFile = files().open(m_fileName.c_str(), File::eMode_BinaryRead);

	if (!pFile)
	{
		N_LOG_ERROR("Could not load skinned model animation file: '%s'", m_fileName.c_str());

		return false;
	}

	FileChunkHeader chunk;
	String signature;
	u32 flags = 0;
	u16 boneCount = 0;

	while (!pFile->eof())
	{
		if (!pFile->readChunkHeader(&chunk))
			break;

		switch (chunk.id)
		{
		//
		// read header
		//
		//TODO: make these enums
		case 1:
			{
				if (chunk.version != kSkeletonAnimationFileVersion)
				{
					N_LOG_ERROR("SkeletonAnimationData version is different from the file: %s, Engine: ver%d File: ver%d",
						m_fileName.c_str(), kSkeletonAnimationFileVersion, chunk.version);
					return false;
				}

				*pFile >> signature;

				if (signature != "NSKA")
				{
					N_LOG_ERROR("Invalid format signature for skinned model animation file: '%s'",
						m_fileName.c_str());

					return false;
				}

				*pFile >> flags;
				*pFile >> boneCount;
				N_LOG_INFO("Loading animation data: '%s' - BoneCount: %d",
					m_fileName.c_str(), boneCount);
				break;
			}

		//
		// read bone animations
		//
		case 2:
			{
				String boneName;
				u32 translationKeyCount, rotationKeyCount, scaleKeyCount;
				f32 keyTime, keyTens, keyCont, keyBias, keyEaseIn, keyEaseOut;
				u32 keyFlags;

				N_LOG_INFO("Loading mesh bone animations for %d bones...", boneCount);

				for (u32 i = 0; i < boneCount; ++i)
				{
					Animation* pAnimation = new Animation;

					N_ASSERT(pAnimation);

					if (!pAnimation)
					{
						continue;
					}

					*pFile >> boneName;
					*pFile >> translationKeyCount;
					*pFile >> rotationKeyCount;
					*pFile >> scaleKeyCount;

					pAnimation->setName(boneName.c_str());

					N_LOG_DEBUG("Loading animation for bone: '%s' : %d translation keys, %d rotation keys, %d scale keys",
						boneName.c_str(), translationKeyCount, rotationKeyCount, scaleKeyCount);

					AnimationTrack* pTranslationTrack = pAnimation->addTrack(
																		"Pos",
																		AnimationTrack::eType_Vector3,
																		nullptr);
					AnimationTrack* pRotationTrack = pAnimation->addTrack(
																		"Rot",
																		AnimationTrack::eType_Quaternion,
																		nullptr);
					AnimationTrack* pScaleTrack = pAnimation->addTrack(
																	"Scl",
																	AnimationTrack::eType_Vector3,
																	nullptr);

					N_ASSERT(pTranslationTrack);
					N_ASSERT(pRotationTrack);
					N_ASSERT(pScaleTrack);

					for (u32 j = 0; j < translationKeyCount; ++j)
					{
						*pFile >> keyFlags;
						*pFile >> keyTime;
						*pFile >> keyTens;
						*pFile >> keyCont;
						*pFile >> keyBias;
						*pFile >> keyEaseIn;
						*pFile >> keyEaseOut;

						Vec3 pos;

						*pFile >> pos;
						AnimationKey* pKey = pTranslationTrack->addKey(keyTime, pos);
						N_ASSERT(pKey);

						if (pKey)
						{
							pKey->m_tension = keyTens;
							pKey->m_continuity = keyCont;
							pKey->m_bias = keyBias;
							pKey->m_easeIn = keyEaseIn;
							pKey->m_easeOut = keyEaseOut;

							if (pKey->m_time > m_animationTotalTime)
								m_animationTotalTime = pKey->m_time;
						}
					}

					Vec3 axis;
					f32 angle;
					Quat q;

					for (u32 j = 0; j < rotationKeyCount; ++j)
					{
						*pFile >> keyFlags;
						*pFile >> keyTime;
						*pFile >> keyTens;
						*pFile >> keyCont;
						*pFile >> keyBias;
						*pFile >> keyEaseIn;
						*pFile >> keyEaseOut;

						*pFile >> angle;
						*pFile >> axis.x;
						*pFile >> axis.y;
						*pFile >> axis.z;

						toQuat(angle, axis, q);

						AnimationKey* pKey = pRotationTrack->addKey(keyTime, q);
						N_ASSERT(pKey);

						if (pKey)
						{
							pKey->m_tension = keyTens;
							pKey->m_continuity = keyCont;
							pKey->m_bias = keyBias;
							pKey->m_easeIn = keyEaseIn;
							pKey->m_easeOut = keyEaseOut;

							if (pKey->m_time > m_animationTotalTime)
								m_animationTotalTime = pKey->m_time;
						}
					}

					for (u32 j = 0; j < scaleKeyCount; ++j)
					{
						*pFile >> keyFlags;
						*pFile >> keyTime;
						*pFile >> keyTens;
						*pFile >> keyCont;
						*pFile >> keyBias;
						*pFile >> keyEaseIn;
						*pFile >> keyEaseOut;

						Vec3 scl;

						*pFile >> scl;

						AnimationKey* pKey = pScaleTrack->addKey(keyTime, scl);
						N_ASSERT(pKey);

						if (pKey)
						{
							pKey->m_tension = keyTens;
							pKey->m_continuity = keyCont;
							pKey->m_bias = keyBias;
							pKey->m_easeIn = keyEaseIn;
							pKey->m_easeOut = keyEaseOut;

							if (pKey->m_time > m_animationTotalTime)
								m_animationTotalTime = pKey->m_time;
						}
					}

					pAnimation->setLoopMode(eAnimationLoop_Repeat);
					pAnimation->setAnimationFlags(
									pAnimation->animationFlags()
									& ~Animation::eFlag_AutoDuration);
					pAnimation->setDuration(m_animationTotalTime);
					pAnimation->play();

					m_boneAnimations.append(pAnimation);
				}

				break;
			}

		// else ignore this unknown chunk
		default:
			{
				pFile->ignoreChunk(&chunk);
				break;
			}
		}
	}

	pFile->close();
	N_LOG_DEBUG("Skeleton animations done loading.");

	return true;
}

bool SkeletonAnimation::unload()
{
	free();

	return true;
}

void SkeletonAnimation::free()
{
	Array<Animation*>::Iterator iter = m_boneAnimations.begin();

	while (iter != m_boneAnimations.end())
	{
		delete *iter;
		++iter;
	}

	m_boneAnimations.clear();
	m_animationTotalTime = 0;
}

const Array<Animation*>& SkeletonAnimation::boneAnimations() const
{
	return m_boneAnimations;
}

f32 SkeletonAnimation::animationTotalTime() const
{
	return m_animationTotalTime;
}

//---

SkeletonAnimationSet::SkeletonAnimationSet()
{
}

SkeletonAnimationSet::~SkeletonAnimationSet()
{
}

bool SkeletonAnimationSet::load(const char* pFilename)
{
	Resource::load(pFilename);
	//TODO
	//XmlDocument xml;

	//N_LOG_INFO("Loading skeleton animation set file: '%s'...", m_fileName.c_str());

	//if (!xml.load(m_fileName.c_str()))
	//{
	//	N_LOG_ERROR("Could not load skeleton animation set file: '%s'", m_fileName.c_str());

	//	return false;
	//}

	//if (!xml.isSuccess())
	//{
	//	N_LOG_ERROR("Could not load skeleton animation set file: '%s'", m_fileName.c_str());

	//	return false;
	//}

	//XmlElement* pSkeletonAnimationSetElem = xml.getRoot().getChild("skeletonAnimationSet");
	//XmlElement* pSkeletonAnimationElem = nullptr;

	//m_name = pSkeletonAnimationSetElem->getAttributeValue("name", "Unnamed");

	//// read animations
	//for (u32 i = 0, iCount = pSkeletonAnimationSetElem->getChildren().size(); i < iCount; ++i)
	//{
	//	pSkeletonAnimationElem = pSkeletonAnimationSetElem->getChildren()[i];
	//	N_LOG_INFO("Loading skeleton animation file '%s'...", pSkeletonAnimationElem->getAttributeValue("file").c_str());
	//	SkeletonAnimation* pAnim = (SkeletonAnimation*)resources().load(pSkeletonAnimationElem->getAttributeValue("file").c_str());

	//	// if we've found an animation file
	//	if (pAnim)
	//	{
	//		if (pSkeletonAnimationElem->getAttributeValue("name") != "")
	//		{
	//			pAnim->setName(pSkeletonAnimationElem->getAttributeValue("name").c_str());
	//		}

	//		m_animations.push_back(pAnim);
	//	}
	//}

	return true;
}

bool SkeletonAnimationSet::unload()
{
	free();

	return true;
}

void SkeletonAnimationSet::free()
{
	m_animations.clear();
}

SkeletonAnimation* SkeletonAnimationSet::firstAnimation() const
{
	Array<SkeletonAnimation*>::Iterator iter = m_animations.begin();

	if (iter != m_animations.end())
	{
		return *iter;
	}

	return nullptr;
}

const Array<SkeletonAnimation*>& SkeletonAnimationSet::animations() const
{
	return m_animations;
}
}