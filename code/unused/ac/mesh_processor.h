#pragma once
#include "common.h"
#include "asset_processor.h"
#include "mesh_utils.h"
#include "base/math/vec3.h"
#include "base/math/quat.h"
#include "base/math/matrix.h"
#include "base/math/bbox.h"
#include "base/array.h"
#include "base/string.h"
#include "base/map.h"

namespace ac
{
class MeshProcessor : public AssetProcessor
{
public:
	MeshProcessor();
	virtual ~MeshProcessor();

	bool process(const char* pSrcFilename, Stream* pStream, const Project& project) override;
	const char* supportedFileExtensions() const override;
	void free();
	bool loadMesh(const char* pFilename);
	void saveMeshes();

protected:
	Mesh m_mesh;
};
}