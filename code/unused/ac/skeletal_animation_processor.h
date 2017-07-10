#pragma once
#include "common.h"
#include "asset_processor.h"
#include "base/math/vec3.h"
#include "base/math/quat.h"
#include "base/math/matrix.h"
#include "base/math/bbox.h"
#include "base/array.h"
#include "base/string.h"
#include "base/map.h"

namespace ac
{
class SkeletalAnimationProcessor : public AssetProcessor
{
public:
	SkeletalAnimationProcessor();
	virtual ~SkeletalAnimationProcessor();

	bool process(const char* pSrcFilename, const Project& project);
	const char* supportedFileExtensions() const;
	void free();

protected:
	Mesh m_mesh;
};

}