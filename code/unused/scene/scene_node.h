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

namespace engine
{
using namespace base;

class Scene;

union SceneNodeTransformFlags
{
	struct
	{
		//! if set, the final object rotation will be computed from the object's look direction
		u32 useLookDirection : 1;
		//! if set, the roll angle will be added to the final object rotation
		u32 useRollAngle : 1;
		//! does this object inherits its parent scale
		u32 inheritParentScale : 1;
		//! does this object inherits its parent rotation
		u32 inheritParentRotation : 1;
	};

	u32 _flags = 0;
};

struct SceneNodeTransform
{
	bool bDirty = true;
	bool bUseParentTransform = true;
	Vec3 translation = { 0.0f, 0.0f, 0.0f };
	Vec3 scale = { 1.0f, 1.0f, 1.0f };
	Quat rotation;
	SceneNodeTransformFlags flags;
	Vec3 lookDirection = { 0.0f, 0.0f, 1.0f };
	//! the object's local up axis (0,1,0) usually but it can change, used for look direction
	Vec3 upAxis = { 0.0f, 1.0f, 0.0f };
	Vec3 rightAxis = { 1.0f, 0.0f, 0.0f };
	//! the roll angle around the local Z axis
	f32 roll = 0.0f;
	Matrix worldMatrix;
	Matrix localMatrix;
	Matrix parentSpaceMatrix;
	//! the world translation inherited from parent to child
	Vec3 hierarchyTranslation;
	//! the world scale inherited from parent to child
	Vec3 hierarchyScale;
	//! the world rotation inherited from parent to child
	Quat hierarchyRotation;
	//! the scene's look at target object
	TransformComponent* pLookAt;
};

class E_API SceneNode
{
public:
	SceneNode();
	virtual ~SceneNode();

	virtual void update(f32 deltaTime);
	virtual void render() {}

	String m_name;
	EntityId m_entity;
	f32 m_distanceToCamera;
	u32 m_renderLayerIndex;
	Scene* m_pScene;
	u32 m_levelOfDetail;
	Array<SceneNode*> m_children;
	SceneNodeTransform m_previousTransform;
	SceneNodeTransform m_transform;
	//! the scene's look at target object
	SceneNode* m_pLookAt;
	SceneNode* m_pParent;

	void addChild(SceneNode* pNode) { m_children.appendUnique(pNode); pNode->m_pParent = this; }
	void removeChild(SceneNode* pNode) { m_children.erase(pNode); pNode->m_pParent = nullptr; }
	void updateTransform();
	void makeLookAt(const Vec3& towardsWorldPos);
	void addRollAngle(f32 degrees);
	void deleteChildren();
};

}