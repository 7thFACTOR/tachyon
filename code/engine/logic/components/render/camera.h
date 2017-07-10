#pragma once
#include "base/defines.h"
#include "core/defines.h"
#include "base/math/vec3.h"
#include "base/math/matrix.h"
#include "logic/components/render/transform.h"
#include "graphics/frustum.h"
#include "logic/types.h"

namespace engine
{
enum class CameraProjection
{
	Perspective,
	Ortho,
	Top,
	Left,
	Front
};

struct CameraComponent : Component
{
	static const ComponentTypeId typeId = StdComponentTypeId_Camera;

	CameraComponent() {}

	void streamData(Stream& stream) override
	{
		Component::streamData(stream);

		stream | fov;
		stream | aspectRatio;
		stream | farPlane;
		stream | nearPlane;
		stream | stereoEyeDistance;
		stream | projectionType;
		stream | rightAxis;
		stream | upAxis;
		stream | stereo;
		stream | autoAspectRatio;
		stream | correctUpSideDownYaw;
		stream | autoUpAxis;
	}

	Matrix projectionMatrix;
	Matrix viewMatrix;
	Frustum frustum;
	f32 fov = 75.0f;
	f32 aspectRatio = 3.0f / 4.0f;
	f32 farPlane = 50000.0f;
	f32 nearPlane = 1.0f;
	f32 stereoEyeDistance = 0.15f;
	CameraProjection projectionType = CameraProjection::Perspective;
	Vec3 rightAxis = Vec3(1.0f, 0.0f, 0.0f);
	Vec3 upAxis = Vec3(0.0f, 1.0f, 0.0f);
	bool stereo = false;
	bool autoAspectRatio = true;
	bool correctUpSideDownYaw = true;
	bool autoUpAxis = false;
	class GpuProgram* overrideGpuProgram = nullptr;
};

}