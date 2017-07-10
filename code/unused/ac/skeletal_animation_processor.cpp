#include "common.h"
#include "mesh_processor.h"
#include "skeletal_anim_processor.h"
#include "base/math/conversion.h"
#include "base/math/util.h"
#include "base/logger.h"
#include "base/platform.h"
#include "base/json.h"
#include "base/util.h"

namespace ac
{
SkeletalAnimationProcessor::SkeletalAnimationProcessor()
{
}

SkeletalAnimationProcessor::~SkeletalAnimationProcessor()
{
}

void SkeletalAnimationProcessor::free()
{
}

const char* SkeletalAnimationProcessor::supportedFileExtensions() const
{
	return ".skeletalanim";
}

AssetProcessor::EType SkeletalAnimationProcessor::type() const
{
	return eType_Compiler;
}

bool SkeletalAnimationProcessor::process(
		const char* pSrcFilename,
		const char* pDestPath,
		const ArgsParser& rArgs)
{
	String str;
	String filename;
	String relDstFilename;
	FileWriter file;

	extractFileName(pSrcFilename, str);
	filename = mergePathFile(pDestPath, str.c_str());

	if (m_mesh.m_boneAnimations.size())
	{
		B_LOG_DEBUG("Exporting skeletal animation...");

		if (file.openFile(filename.c_str()))
		{
			// header
			file.beginChunk(eSkeletonAnimChunk_Header, 1);
			file.writeString("SKELETALANIMATION");
			file.writeInt32(0); //anim flags
			file.writeInt16(m_mesh.m_boneAnimations.size());
			file.endChunk();

			// animations
			file.beginChunk(2, 1);

			for (unsigned int j = 0; j < m_mesh.m_boneAnimations.size(); j++)
			{
				Animation* pBoneAnim = m_mesh.m_boneAnimations[j];

				B_LOG_DEBUG("   Writing bone animation: '%s' (TranslationKeys: %d, RotationKeys: %d, ScaleKeys: %d)",
					pBoneAnim->m_nodeName.c_str(),
					pBoneAnim->m_translationKeys.size(),
					pBoneAnim->m_rotationKeys.size(),
					pBoneAnim->m_scaleKeys.size());

				file.writeString(pBoneAnim->m_nodeName.c_str());
				file.writeInt32(pBoneAnim->m_translationKeys.size());
				file.writeInt32(pBoneAnim->m_rotationKeys.size());
				file.writeInt32(pBoneAnim->m_scaleKeys.size());

				for (unsigned int k = 0; k < pBoneAnim->m_translationKeys.size(); k++)
				{
					AnimationKey* pKey = pBoneAnim->m_translationKeys[k];

					file.writeInt32(0);
					file.writeFloat(pKey->m_time);
					file.writeFloat(pKey->m_tcb.x);
					file.writeFloat(pKey->m_tcb.y);
					file.writeFloat(pKey->m_tcb.z);
					file.writeFloat(pKey->m_easeIn);
					file.writeFloat(pKey->m_easeOut);
					file.writeFloat(pKey->m_values[0]);
					file.writeFloat(pKey->m_values[1]);
					file.writeFloat(pKey->m_values[2]);
				}

				Quat quat, newQuat;
				Vec3 axis(0, 0, 0);
				f32 angle = 0;

				for (unsigned int k = 0; k < pBoneAnim->m_rotationKeys.size(); k++)
				{
					AnimationKey* pKey = pBoneAnim->m_rotationKeys[k];

					file.writeInt32(0);
					file.writeFloat(pKey->m_time);
					file.writeFloat(pKey->m_tcb.x);
					file.writeFloat(pKey->m_tcb.y);
					file.writeFloat(pKey->m_tcb.z);
					file.writeFloat(pKey->m_easeIn);
					file.writeFloat(pKey->m_easeOut);

					quat.reset();
					quat.x = pKey->m_values[0];
					quat.y = pKey->m_values[1];
					quat.z = pKey->m_values[2];
					quat.w = pKey->m_values[3];
					toAngleAxis(quat, angle, axis);
					angle = (f32)B_RADIANS_TO_DEGREES(angle);

					file.writeFloat(angle);
					file.writeFloat(axis.x);
					file.writeFloat(axis.y);
					file.writeFloat(axis.z);
				}

				for (unsigned int k = 0; k < pBoneAnim->m_scaleKeys.size(); k++)
				{
					AnimationKey* pKey = pBoneAnim->m_scaleKeys[k];

					file.writeInt32(0);
					file.writeFloat(pKey->m_time);
					file.writeFloat(pKey->m_tcb.x);
					file.writeFloat(pKey->m_tcb.y);
					file.writeFloat(pKey->m_tcb.z);
					file.writeFloat(pKey->m_easeIn);
					file.writeFloat(pKey->m_easeOut);
					file.writeFloat(pKey->m_values[0]);
					file.writeFloat(pKey->m_values[1]);
					file.writeFloat(pKey->m_values[2]);
				}
			}

			file.endChunk();
			file.closeFile();
		}
	}// end skeletal anim save

	return true;
}
}