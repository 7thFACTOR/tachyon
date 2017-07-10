#pragma once
#include "base/string.h"
#include "base/array.h"
#include "base/map.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/class_registry.h"
#include "base/observer.h"
#include "base/math/ray.h"
#include "base/thread_queue.h"
#include "base/double_container.h"
#include "game/types.h"
#include "base/math/quat.h"
#include "core/defines.h"
#include "base/math/matrix.h"

namespace engine
{
using namespace base;
class SceneNode;
class CameraNode;

//! The scene stores a tree of scene objects, in a hierarchy, and each frame it
//! gathers the render packets from the visible scene objects, sort the packets
//! by their properties (gpu_program, render layer), and then renders them
class E_API Scene
{
public:
	Scene();
	virtual ~Scene();

	void update(f32 deltaTime);
	SceneNode* root() { return m_pRoot; }

protected:
	friend class SceneNode;

	SceneNode* m_pRoot;
};

}