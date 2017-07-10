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

namespace engine
{
//! Camera projection type
enum class CameraProjection
{
	Perspective,
	Top,
	Left,
	Front
};

struct CameraData
{
	f32 fov = 75;
	f32 aspectRatio = 3.0f / 4.0f;
	f32 farPlane = 50000.0f;
	f32 nearPlane = 0.5f;
	f32 stereoEyeDistance = 0.15f;
	CameraProjection projectionType = CameraProjection::Perspective;
	Vec3 rightAxis = Vec3(1, 0, 0);
	Vec3 upAxis = Vec3(0, 1, 0);
	bool bStereo = false;
	bool bAutoAspectRatio = true;
	bool bCorrectUpSideDownYaw = true;
};

class E_API CameraNode : public SceneNode
{
public:
	CameraNode();
	virtual ~CameraNode();
	void update(f32 deltaTime);
	const Matrix& projectionMatrix() const { return m_projectionMatrix; }
	const Matrix& viewMatrix() const { return m_viewMatrix; }
	const Frustum& frustum() const { return m_frustum; }
	const CameraData& data() const { return m_data; }

protected:
	Matrix m_projectionMatrix;
	Matrix m_viewMatrix;
	Frustum m_frustum;
	CameraData m_data;
};

}