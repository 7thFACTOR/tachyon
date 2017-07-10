#include "render/scene/scene_node.h"
#include "render/scene/scene.h"
#include "base/math/conversion.h"
#include "base/logger.h"

namespace engine
{
SceneNode::SceneNode()
{
	m_pLookAt = nullptr;
	m_pParent = nullptr;
	m_levelOfDetail = 0;
	m_bUseParentTransform = false;
	m_pScene = nullptr;
	m_entity = 0;
}

SceneNode::~SceneNode()
{
	deleteChildren();
}

void SceneNode::update(f32 deltaTime)
{
	updateTransform();

	B_LOG_DEBUG("%s updated of ent %d", m_name.c_str(),  m_entity);

	for (size_t i = 0; i < m_children.size(); ++i)
	{
		m_children[i]->update(deltaTime);
	}
}

void SceneNode::updateTransform()
{
	if (m_bUseParentTransform)
	{
		if (m_pParent)
		{
			m_worldMatrix = m_pParent->m_worldMatrix;
			m_localMatrix = m_pParent->m_localMatrix;
			m_parentSpaceMatrix = m_pParent->m_parentSpaceMatrix;
			m_hierarchyTranslation = m_pParent->m_hierarchyTranslation;
			m_hierarchyRotation = m_pParent->m_hierarchyRotation;
			m_hierarchyScale = m_pParent->m_hierarchyScale;
		}

		return;
	}

	//if (!m_transform.bDirty)
	//	return;

	Matrix mtxRotation, mtxRoll;

	if (!m_pLookAt)
	{
		if (m_transform.flags.useLookDirection)
		{
			mtxRotation.lookTowards(m_transform.lookDirection, m_transform.upAxis);
		}
		else
		{
			toMatrix(m_transform.rotation, mtxRotation);
		}
	}
	else
	{
		mtxRotation.lookTowards(m_transform.translation - m_pLookAt->m_transform.translation, m_transform.upAxis);
	}

	if (m_transform.flags.useRollAngle)
	{
		mtxRoll = mtxRotation;
		mtxRotation.setIdentity();
		mtxRotation.setRotationZ(m_transform.roll);
		mtxRotation.multiply(mtxRoll);
	}

	Matrix mtxTrans, mtxScale;

	if (m_pParent)
	{
		if (m_transform.flags.inheritParentRotation)
			m_hierarchyRotation = m_pParent->m_hierarchyRotation * toQuat(mtxRotation);
		else
			m_hierarchyRotation = toQuat(mtxRotation);

		if (m_transform.flags.inheritParentScale)
			m_hierarchyScale = m_pParent->m_hierarchyScale * m_transform.scale;
		else
			m_hierarchyScale = m_transform.scale;

		toMatrix(m_pParent->m_hierarchyRotation).transform(
			m_pParent->m_hierarchyScale * m_transform.translation, m_hierarchyTranslation);

		m_hierarchyTranslation += m_pParent->m_hierarchyTranslation;
	}
	else
	{
		m_hierarchyTranslation = m_transform.translation;
		m_hierarchyRotation = toQuat(mtxRotation);
		m_hierarchyScale = m_transform.scale;
	}

	mtxTrans.setIdentity();
	mtxTrans.setTranslation(m_hierarchyTranslation);
	mtxScale.setIdentity();
	mtxScale.setScale(m_hierarchyScale);

	m_worldMatrix = mtxScale * toMatrix(m_hierarchyRotation) * mtxTrans;
	mtxTrans.setIdentity();
	mtxTrans.setTranslation(m_transform.translation);
	mtxScale.setIdentity();
	mtxScale.setScale(m_transform.scale.x, m_transform.scale.y, m_transform.scale.z);
	m_localMatrix = mtxScale * mtxRotation;
	m_parentSpaceMatrix = m_localMatrix * mtxTrans;
	m_transform.bDirty = false;
}

void SceneNode::makeLookAt(const Vec3& rTowardsWorldPos)
{
	Matrix m;
	Vec3 worldToLocalPos;

	m_worldMatrix.inverted().transform(rTowardsWorldPos, worldToLocalPos);
	m_transform.lookDirection = (worldToLocalPos - m_transform.translation).normalized();
	m.lookAt(m_transform.translation, worldToLocalPos, m_transform.upAxis);
	m_transform.rotation = toQuat(m);
	m_transform.bDirty = true;
}

void SceneNode::addRollAngle(f32 aDegrees)
{
	Quat q;

	toMatrix(m_transform.rotation).transform(Vec3(0.0f, 0.0f, 1.0f), m_transform.rightAxis);
	toQuat(-aDegrees, m_transform.rightAxis, q);
	m_transform.rotation = q * m_transform.rotation;
}

void SceneNode::deleteChildren()
{
	for (auto child : m_children)
	{
		child->deleteChildren();
		delete child;
	}

	m_children.clear();
}

}