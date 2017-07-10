#pragma once
#include "base/array.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/math/vec3.h"
#include "base/math/matrix.h"
#include "base/math/ray.h"
#include "base/math/util.h"
#include "game/components/transform.h"
#include "base/math/color.h"
#include "base/observer.h"
#include "base/math/easing.h"

namespace engine
{
class ComponentProxy;
struct Component;
class EditorGizmo;

enum EEditorMouseFlags
{
	eEditorMouseFlag_Left = B_BIT(0),
	eEditorMouseFlag_Middle = B_BIT(1),
	eEditorMouseFlag_Right = B_BIT(2),
	eEditorMouseFlag_Ctrl = B_BIT(3),
	eEditorMouseFlag_Shift = B_BIT(4),
	eEditorMouseFlag_Alt = B_BIT(5)
};

struct HitTestInfo
{
};

struct E_API EditorGizmoObserver
{
	virtual void onGizmoMouseButton(EditorGizmo* pGizmo, u32 aMouseFlags, const HitTestInfo& rHit){};
	virtual void onGizmoMouseMove(EditorGizmo* pGizmo, u32 aMouseFlags, const HitTestInfo& rHit){};
};

class E_API EditorGizmoManager: Observable<EditorGizmoObserver>
{
public:
	bool registerGizmo(EditorGizmo* pGizmo);
	bool unregisterGizmo(EditorGizmo* pGizmo);
	void renderGizmos();
};

class E_API EditorComponentProxy
{
public:
	enum EFlags
	{
		eFlag_Selected = B_BIT(0),
		eFlag_Hidden = B_BIT(1),
		eFlag_Frozen = B_BIT(2)
	};

	Component* component()
	{
		return m_pComponent;
	}

	virtual void setComponent(Component* pComp)
	{
		m_pComponent = pComp;
	}

	virtual void registerGizmos(EditorGizmoManager& rMgr){}
	virtual void unregisterGizmos(EditorGizmoManager& rMgr){}
	virtual void updateGizmos(){}
	u32 flags(){ return m_flags; }
	virtual void setFlags(u32 aFlags){ m_flags |= aFlags; }
	virtual void unsetFlags(u32 aFlags){ m_flags &=~aFlags; }
	bool isFlagOn(u32 aFlag){ return 0 != (m_flags & aFlag);}

protected:
	Component* m_pComponent;
	u32 m_flags;
};

class E_API EditorGizmoAnimation
{
public:
	enum EType
	{
		eType_None,
		eType_Linear,
		eType_Slowdown,
		eType_Elastic
	};

	void start()
	{
		time = 0.0f;
		bAnimating = true;
	}

	void animate(f32 aDeltaTime, Vec3& rOutPos, Vec3& rOutScale, Color& rOutColor)
	{
		if (!bAnimating)
		{
			return;
		}

		time += aDeltaTime;

		if (time > 1.0f)
		{
			bAnimating = false;
			time = 1.0f;
		}
	
		switch (type)
		{
		case eType_None:
			rOutPos = toPosition;
			rOutScale = toScale;
			rOutColor = toColor;
			break;

		case eType_Linear:
			rOutPos = B_LERP(fromPosition, toPosition, time);
			rOutScale = B_LERP(fromScale, toScale, time);
			rOutColor = B_LERP(fromColor, toColor, time);
			break;

		case eType_Slowdown:
			rOutPos.x = easeOutExpo(time, fromPosition.x, toPosition.x, 1.0f);
			rOutPos.y = easeOutExpo(time, fromPosition.y, toPosition.y, 1.0f);
			rOutPos.z = easeOutExpo(time, fromPosition.z, toPosition.z, 1.0f);
			rOutScale.x = easeOutExpo(time, fromScale.x, toScale.x, 1.0f);
			rOutScale.y = easeOutExpo(time, fromScale.y, toScale.y, 1.0f);
			rOutScale.z = easeOutExpo(time, fromScale.z, toScale.z, 1.0f);
			rOutColor.r = easeOutExpo(time, fromColor.r, toColor.r, 1.0f);
			rOutColor.g = easeOutExpo(time, fromColor.g, toColor.g, 1.0f);
			rOutColor.b = easeOutExpo(time, fromColor.b, toColor.b, 1.0f);
			rOutColor.a = easeOutExpo(time, fromColor.a, toColor.a, 1.0f);
			break;

		case eType_Elastic:
			rOutPos.x = easeOutElastic(time, fromPosition.x, toPosition.x, 1.0f);
			rOutPos.y = easeOutElastic(time, fromPosition.y, toPosition.y, 1.0f);
			rOutPos.z = easeOutElastic(time, fromPosition.z, toPosition.z, 1.0f);
			rOutScale.x = easeOutElastic(time, fromScale.x, toScale.x, 1.0f);
			rOutScale.y = easeOutElastic(time, fromScale.y, toScale.y, 1.0f);
			rOutScale.z = easeOutElastic(time, fromScale.z, toScale.z, 1.0f);
			rOutColor.r = easeOutElastic(time, fromColor.r, toColor.r, 1.0f);
			rOutColor.g = easeOutElastic(time, fromColor.g, toColor.g, 1.0f);
			rOutColor.b = easeOutElastic(time, fromColor.b, toColor.b, 1.0f);
			rOutColor.a = easeOutElastic(time, fromColor.a, toColor.a, 1.0f);
			break;
		}
	}

	Vec3 fromPosition, toPosition;
	Vec3 fromScale, toScale;
	Color fromColor, toColor;
	EType type;
	f32 time;
	bool bAnimating;
};

class E_API EditorGizmo
{
public:
	enum EFlags
	{
		eFlag_ConstantScale = B_BIT(0),
		eFlag_Selected = B_BIT(1)
	};

	EditorGizmo(){};
	virtual ~EditorGizmo(){};
	virtual void render() = 0;
	virtual void update() = 0;
	virtual bool hitTest(const Ray& rRay, HitTestInfo& rOutHit) = 0;
	virtual void animate(){};
	EditorGizmoAnimation& animation()
	{
		return m_animation;
	}

	EditorComponentProxy* ownerProxy() const
	{
		return m_pOwnerProxy;
	}

	void setOwnerProxy(EditorComponentProxy* pOwner)
	{
		m_pOwnerProxy = pOwner;
	}

protected:
	//! if this is null, then this is a global gizmo (like the PRS gizmo)
	EditorComponentProxy* m_pOwnerProxy;
	Vec3 m_position;
	Vec3 m_scale;
	Color m_color;
	ResourceId m_texture;
	EditorGizmoAnimation m_animation;
	u32 m_flags;
};

}