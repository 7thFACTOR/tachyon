#pragma once
#include "base/string.h"
#include "base/array.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/class_registry.h"
#include "base/math/color.h"
#include "base/math/bbox.h"
#include "base/math/vec3.h"
#include "base/math/quat.h"
#include "base/math/matrix.h"
#include "game/component.h"
#include "render/frustum.h"
#include "render/scene/scene_node.h"
#include "core/types.h"

namespace engine
{
class E_API MeshNode : public SceneNode
{
public:
	MeshNode();
	virtual ~MeshNode();
	void setMesh(ResourceId mesh);
	void update(f32 deltaTime);
	void render();

protected:
	ResourceId m_mesh;
};

}