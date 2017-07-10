#include "render/scene/camera_node.h"
#include "base/math/conversion.h"
#include "core/core.h"
#include "render/renderer.h"
#include "base/logger.h"
#include "render/scene/scene.h"

namespace engine
{
CameraNode::CameraNode()
	: SceneNode()
{
	m_bUseParentTransform = true;
}

CameraNode::~CameraNode()
{}

void CameraNode::update(f32 deltaTime)
{
	SceneNode::update(deltaTime);

	m_viewMatrix = Matrix::translationMatrix(m_hierarchyTranslation.negated()) * toMatrix(m_hierarchyRotation);

	if (m_data.bAutoAspectRatio)
	{
		if (m_data.projectionType != CameraProjection::Perspective)
		{
			m_data.aspectRatio = (f32) engine::renderer().viewport().width / engine::renderer().viewport().height;
		}
		else
		{
			m_data.aspectRatio = (f32) engine::renderer().viewport().height / engine::renderer().viewport().width;
		}
	}

	m_projectionMatrix.setIdentity();

	switch (m_data.projectionType)
	{
	case CameraProjection::Perspective:
		{
			m_projectionMatrix.perspective(
				m_data.fov,
				m_data.aspectRatio,
				m_data.nearPlane,
				m_data.farPlane);
			break;
		}

	case CameraProjection::Front:
		{
			//TODO
			break;
		}

	case CameraProjection::Left:
		{
			//TODO
			break;
		}

	case CameraProjection::Top:
		{
			//TODO
			break;
		}
	}

	m_frustum.calculateFrustum(m_worldMatrix, m_projectionMatrix);
}

}