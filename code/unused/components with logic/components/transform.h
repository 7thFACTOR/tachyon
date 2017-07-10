#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/math/vec3.h"
#include "base/math/quat.h"
#include "base/math/matrix.h"
#include "game/component.h"
#include "render/scene/scene_node.h"

namespace engine
{
struct TransformComponent : Component
{
	enum
	{
		TypeId = ComponentType::Transform,
		DefaultCount = kMaxComponentCountPerType
	};

	TransformComponent()
	{}

	SceneNodeTransform data;

	void move(const Vec3& delta)
	{
		if (delta.isEquivalent(Vec3(), 0.0001f))
		{
			return;
		}

		Matrix m = data.worldMatrix;
		Vec3 deltaLocal;

		m.invert();
		m.rotationOnly().transform(delta, deltaLocal);
		data.translation += deltaLocal;
		data.bDirty = true;
	}
};
}