#include "editor/gizmo.h"
#include "game/components/camera.h"
#include "render/render_manager.h"
#include "core/core.h"
#include "base/assert.h"
#include "base/math/plane.h"
#include "base/math/util.h"
#include "base/math/intersection.h"
#include "base/math/conversion.h"

namespace engine
{
/*
static const u32 kAxisGizmoRotationCirclePoints = 15;

AxisGizmo::AxisGizmo()
{
	m_arrowSize = 70;
	m_arrowBodySize = 50;
	m_arrowAperture = 20;
	m_arrowOffset = 0;
	m_axisSize = 150;
	m_axisConeRadius = 7;
	m_axisConeHeight = 30;
	m_axisConeSegments = 6;
	m_sphereKnobSegments = 20;
	m_axisSizeInPixels = 10;
	m_gizmoPlaneSize = 45;
	m_axisScale = 1;
	m_selectedPlaneColor.set(1, 1, 0, 0.4f);
	m_planeColor.set(1, 1, 0, 0.0f);
	m_selectedAxisColor.set(1, 1, 0, 1);
	m_hoveredGizmoAxis = 0;
	m_selectedGizmoAxis = 0;
	m_rotationHandlesPosition = 1.1f;
	m_scaleHandlesPosition = 0.5f;
	m_rotationHandleRadius = 30;
	m_scaleKnobSize = 10;
	m_rotationSphereRadius = 10;
	m_bWorldMode = false;
	m_currentRotationAngle = 0;
	m_bDrawTranslateGizmo = true;
	m_bDrawRotationGizmo = true;
	m_bDrawScaleGizmo = true;
}

AxisGizmo::~AxisGizmo()
{
}

f32 AxisGizmo::computeConstantScaleForPoint(const Vec3& rPos)
{
	Vec3 v[2], vp[2];
	v[0].set(0, 0, 0);
	Camera* pCamera = gfx().camera();

	B_ASSERT(pCamera);

	if (!pCamera)
	{
		return 0;
	}

	pCamera->applyMatrices();

	if (pCamera->projection() == Camera::eProjection_Perspective)
	{
		f32 dist = pCamera->translation().distance(rPos);
		f32 scaleView = gfx().viewHeight();
		return dist / scaleView;;
	}
	else if (pCamera->projection() == Camera::eProjection_Top)
	{
		v[1].set(100.0f, 0, 0);
	}
	else
	{
		v[1].set(0, 100.0f, 0);
	}

	const Matrix& mtx = pCamera->worldMatrix();

	gfx().setMatrixMode(eMatrixMode_World);
	gfx().loadMatrix(mtx);
	gfx().project(v, 2, vp);

	vp[0].z = vp[1].z = 0;

	f32 factor = vp[0].distance(vp[1]) / 100.0f;
	return (f32) 1.0f / factor;
}


void AxisGizmo::computeMovePoint(const Vec3& r2DPoint)
{
	Plane plane;
	Vec3 theNewPos;
	f32 fTime = 0;

	m_pickRay = gfx().computePickRay(r2DPoint.x, r2DPoint.y);
	m_pickRay.setDirection(m_pickRay.direction().normalized());

	plane.setOrigin(m_translation);

	if (m_selectedGizmoAxis == eFlag_TranslatePlaneXY)
	{
		plane.setNormal(Vec3(0, 0, (gfx().camera()->translation().z > m_translation.z) ? 1 : -1));
	}
	else if (m_selectedGizmoAxis == eFlag_TranslatePlaneYZ)
	{
		plane.setNormal(Vec3((gfx().camera()->translation().x > m_translation.x) ? 1 : -1, 0, 0));
	}
	else if ((m_selectedGizmoAxis == eFlag_TranslatePlaneZX))
	{
		plane.setNormal(Vec3(0, (gfx().camera()->translation().y > m_translation.y) ? 1 : -1, 0));
	}
	else if (m_selectedGizmoAxis == eFlag_TranslateX
			|| m_selectedGizmoAxis == eFlag_ScaleX
			|| m_selectedGizmoAxis == eFlag_RotateX)
	{
		plane.setNormal(Vec3(0.0f, 0.0f, (gfx().camera()->translation().z > m_translation.z) ? 1.0f : -1.0f));
	}
	else if (m_selectedGizmoAxis == eFlag_TranslateY
			|| m_selectedGizmoAxis == eFlag_ScaleY
			|| m_selectedGizmoAxis == eFlag_RotateY)
	{
		plane.setNormal(Vec3(0.0f, 0.0f, (gfx().camera()->translation().z > m_translation.z) ? 1.0f : -1.0f));
	}
	else if (m_selectedGizmoAxis == eFlag_TranslateZ
			|| m_selectedGizmoAxis == eFlag_ScaleZ
			|| m_selectedGizmoAxis == eFlag_RotateZ)
	{
		plane.setNormal(Vec3(0.0f, (gfx().camera()->translation().y > m_translation.y) ? 1.0f : -1.0f, 0.0f));
	}

	if (intersectPlaneRay(plane, m_pickRay, fTime))
	{
		theNewPos = m_pickRay.interpolatedPointAt(-fTime);
	}
	else
	{
		theNewPos.set(0.0f, 0.0f, 0.0f);
	}

	m_movePoint = theNewPos;
}

f32 AxisGizmo::computeAxisScale()
{
	m_axisScale = computeConstantScaleForPoint(m_translation);
	
	return m_axisScale;
}

void AxisGizmo::checkHoveredElements(const Vec3& r2DPoint)
{
	Ray pickRay = gfx().computePickRay(r2DPoint.x, r2DPoint.y);
	Vec3 p1, p2;
	Vec3 b1, b2;
	f32 t1, t2;
	Plane plane;
	Vec3 a1 = pickRay.startPoint();
	Vec3 a2 = pickRay.endPoint();
	f32 dist = m_axisSize * m_axisScale * 0.05f;
	f32 distMin = m_axisScale * 10.0f;
	Matrix mtxRot;
	bool bWorldMode = m_bWorldMode;

	// force world mode when translate gizmo is visible
	if (m_bDrawTranslateGizmo)
	{
		bWorldMode = true;
	}

	if (!bWorldMode)
	{
		mtxRot = toMatrix(m_rotation);
	}

#define E_CHECK_HIT_SEGMENT(planeId)\
	if (!bWorldMode)\
	{\
		mtxRot.transform(b1, b1);\
		mtxRot.transform(b2, b2);\
	}\
	b1 += m_translation;\
	b2 += m_translation;\
	if (intersectSegmentSegment(a1, a2, b1, b2, p1, p2, t1, t2, distMin))\
	{\
		p1 = p2 - p1;\
		if (p1.length() < dist)\
		{\
			m_hoveredGizmoAxis = planeId;\
			return;\
		}\
	}

#define E_CHECK_HIT_AXIS(axisId)\
	if (!bWorldMode)\
	{\
		mtxRot.transform(b2, b2);\
	}\
	b2 += m_translation;\
	if (intersectSegmentSegment(a1, a2, b1, b2, p1, p2, t1, t2, distMin))\
	{\
		p1 = p2 - p1;\
		if (p1.length() < dist)\
		{\
			m_hoveredGizmoAxis = axisId;\
			return;\
		}\
	}
 
	if (m_bDrawScaleGizmo)
	{
		//
		// check scale boxes
		//
		BBox box;
		Vec3 pos;
		f32 scaleHandlesPosition = m_bDrawTranslateGizmo ? m_scaleHandlesPosition : 1.0f;
		f32 radius = m_axisScale * m_scaleKnobSize / 2.0f;

		pos.set(m_axisScale * m_axisSize * scaleHandlesPosition, 0.0f, 0.0f);

		if (!bWorldMode)
		{
			mtxRot.transform(pos, pos);
		}

		pos += m_translation;
		box.set(pos - radius, pos + radius);

		if (intersectBoxRay(box, pickRay, p1, p2, t1))
		{
			m_hoveredGizmoAxis = eFlag_ScaleX;
			return;
		}

		pos.set(0.0f, m_axisScale * m_axisSize * scaleHandlesPosition, 0.0f);

		if (!m_bWorldMode)
		{
			mtxRot.transform(pos, pos);
		}

		pos += m_translation;
		box.set(pos - radius, pos + radius);

		if (intersectBoxRay(box, pickRay, p1, p2, t1))
		{
			m_hoveredGizmoAxis = eFlag_ScaleY;
			return;
		}

		pos.set(0.0f, 0.0f, m_axisScale * m_axisSize * scaleHandlesPosition);

		if (!m_bWorldMode)
		{
			mtxRot.transform(pos, pos);
		}

		pos += m_translation;
		box.set(pos - radius, pos + radius);

		if (intersectBoxRay(box, pickRay, p1, p2, t1))
		{
			m_hoveredGizmoAxis = eFlag_ScaleZ;
			return;
		}
	}

	//
	// translation
	//
	if (m_bDrawTranslateGizmo)
	{
		//
		// check plane XY
		//

		// we do not want local space translate
		bool bOldWorldMode  = bWorldMode;

		// X axis
		b1 = Vec3(m_gizmoPlaneSize * m_axisScale, 0.0f, 0.0f);
		b2 = Vec3(m_gizmoPlaneSize * m_axisScale, m_gizmoPlaneSize * m_axisScale, 0.0f);
		N_CHECK_HIT_SEGMENT(eFlag_TranslatePlaneXY);

		// Y axis
		b1 = Vec3(0.0f, m_gizmoPlaneSize * m_axisScale, 0.0f);
		b2 = Vec3(m_gizmoPlaneSize * m_axisScale, m_gizmoPlaneSize * m_axisScale, 0.0f);
		N_CHECK_HIT_SEGMENT(eFlag_TranslatePlaneXY);

		//
		// check plane YZ
		//

		// Y axis
		b1 = Vec3(0.0f, m_gizmoPlaneSize * m_axisScale, 0.0f);
		b2 = Vec3(0.0f, m_gizmoPlaneSize * m_axisScale, m_gizmoPlaneSize * m_axisScale);
		N_CHECK_HIT_SEGMENT(eFlag_TranslatePlaneYZ);

		// Z axis
		b1 = Vec3(0.0f, 0.0f, m_gizmoPlaneSize * m_axisScale);
		b2 = Vec3(0.0f, m_gizmoPlaneSize * m_axisScale, m_gizmoPlaneSize * m_axisScale);
		N_CHECK_HIT_SEGMENT(eFlag_TranslatePlaneYZ);

		//
		// check plane ZX
		//

		// Z axis
		b1 = Vec3(m_gizmoPlaneSize * m_axisScale, 0.0f, 0.0f);
		b2 = Vec3(m_gizmoPlaneSize * m_axisScale, 0.0f, m_gizmoPlaneSize * m_axisScale);
		N_CHECK_HIT_SEGMENT(eFlag_TranslatePlaneZX);

		// X axis
		b1 = Vec3(0.0f, 0.0f, m_gizmoPlaneSize * m_axisScale);
		b2 = Vec3(m_gizmoPlaneSize * m_axisScale, 0.0f, m_gizmoPlaneSize * m_axisScale);
		N_CHECK_HIT_SEGMENT(eFlag_TranslatePlaneZX);

		//
		// check axes
		//

		// X
		b1 = m_translation;
		b2 = Vec3(m_axisSize * m_axisScale, 0.0f, 0.0f);
		N_CHECK_HIT_AXIS(eFlag_TranslateX);

		// Y
		b1 = m_translation;
		b2 = Vec3(0.0f, m_axisSize * m_axisScale, 0.0f);
		N_CHECK_HIT_AXIS(eFlag_TranslateY);

		// Z
		b1 = m_translation;
		b2 = Vec3(0.0f, 0.0f, m_axisSize * m_axisScale);
		N_CHECK_HIT_AXIS(eFlag_TranslateZ);

		bWorldMode = bOldWorldMode;
	}

	if (m_bDrawRotationGizmo)
	{
		//
		// check rotation axes
		//
		Array<Vec3> pts;
		f32 minDist = m_axisScale * m_rotationHandleRadius * 0.1f;
		Vec3 spherePos, hitPos;
		f32 hitTime;

		spherePos = Vec3(m_axisScale * m_axisSize * m_rotationHandlesPosition, 0.0f, 0.0f);
		pts = m_rotationAxisCirclePoints[0];

		for (size_t i = 0, iCount = pts.size(); i < iCount; ++i)
		{
			pts[i].x += m_axisSize * m_rotationHandlesPosition;
			pts[i] *= m_axisScale;
		}

		if (!bWorldMode)
		{
			for (size_t i = 0, iCount = pts.size(); i < iCount; ++i)
			{
				mtxRot.transform(pts[i], pts[i]);
			}

			mtxRot.transform(spherePos, spherePos);
		}

		for (size_t i = 0, iCount = pts.size(); i < iCount; ++i)
		{
			pts[i] += m_translation;
		}

		spherePos += m_translation;
		Vec3 aa, bb;

		if (intersectLinePolyline(a1, a2, pts, minDist)
			|| intersectRaySphere(
					spherePos,
					m_rotationSphereRadius * m_axisScale,
					pickRay, hitTime, hitPos, hitPos))
		{
			m_hoveredGizmoAxis = eFlag_RotateX;
			return;
		}

		//---

		spherePos = Vec3(0.0f, m_axisScale * m_axisSize * m_rotationHandlesPosition, 0.0f);
		pts = m_rotationAxisCirclePoints[1];

		for (size_t i = 0, iCount = pts.size(); i < iCount; ++i)
		{
			pts[i].y += m_axisSize * m_rotationHandlesPosition;
			pts[i] *= m_axisScale;
		}

		if (!bWorldMode)
		{
			for (size_t i = 0, iCount = pts.size(); i < iCount; ++i)
			{
				mtxRot.transform(pts[i], pts[i]);
			}

			mtxRot.transform(spherePos, spherePos);
		}

		for (size_t i = 0, iCount = pts.size(); i < iCount; ++i)
		{
			pts[i] += m_translation;
		}

		spherePos += m_translation;

		if (intersectLinePolyline(a1, a2, pts, minDist)
			|| intersectRaySphere(
					spherePos,
					m_rotationSphereRadius * m_axisScale,
					pickRay, hitTime, hitPos, hitPos))
		{
			m_hoveredGizmoAxis = eFlag_RotateY;
			return;
		}

		//---

		spherePos = Vec3(0.0f, 0.0f, m_axisScale * m_axisSize * m_rotationHandlesPosition);
		pts = m_rotationAxisCirclePoints[2];

		for (size_t i = 0, iCount = pts.size(); i < iCount; ++i)
		{
			pts[i].z += m_axisSize * m_rotationHandlesPosition;
			pts[i] *= m_axisScale;
		}

		if (!bWorldMode)
		{
			for (size_t i = 0, iCount = pts.size(); i < iCount; ++i)
			{
				mtxRot.transform(pts[i], pts[i]);
			}

			mtxRot.transform(spherePos, spherePos);
		}

		for (size_t i = 0, iCount = pts.size(); i < iCount; ++i)
		{
			pts[i] += m_translation;
		}

		spherePos += m_translation;

		if (intersectLinePolyline(a1, a2, pts, minDist)
			|| intersectRaySphere(
					spherePos,
					m_rotationSphereRadius * m_axisScale,
					pickRay, hitTime, hitPos, hitPos))
		{
			m_hoveredGizmoAxis = eFlag_RotateZ;
			return;
		}
	}

	m_hoveredGizmoAxis = 0;
}

void GfxManager::drawAxisGizmo(AxisGizmo& rGizmo)
{
	bool bWorldMode = rGizmo.m_bWorldMode;
	
	m_pCamera->applyMatrices();
	setMatrixMode(eMatrixMode_World);
	loadIdentity();
	translate(rGizmo.m_translation.x, rGizmo.m_translation.y, rGizmo.m_translation.z);

	if (rGizmo.m_bDrawTranslateGizmo)
	{
		bWorldMode = true;
	}

	if (!bWorldMode)
	{
		multiplyMatrix(toMatrix(rGizmo.m_rotation));
	}

	scale(rGizmo.m_axisScale, rGizmo.m_axisScale, rGizmo.m_axisScale);
	disableTexturing();
	setDepthTest(false);
	setDepthWrite(false);
	setCullMode(eCullMode_None);
	setFillMode(eFillMode_Solid);
	setBlending(true, eBlendOp_SrcAlpha, eBlendOp_InvSrcAlpha);

	// draw plane XY
	beginPrimitives(eGfxPrimitive_TriangleList);

	if ((rGizmo.m_selectedGizmoAxis & AxisGizmo::eFlag_TranslatePlaneXY)
		|| (rGizmo.m_hoveredGizmoAxis & AxisGizmo::eFlag_TranslatePlaneXY))
	{
		addColor(rGizmo.m_selectedPlaneColor);
	}
	else
	{
		addColor(rGizmo.m_planeColor);
	}

	addVertex(0, 0, 0);
	addVertex(0, rGizmo.m_gizmoPlaneSize, 0);
	addVertex(rGizmo.m_gizmoPlaneSize, rGizmo.m_gizmoPlaneSize, 0);

	addVertex(rGizmo.m_gizmoPlaneSize, rGizmo.m_gizmoPlaneSize, 0);
	addVertex(rGizmo.m_gizmoPlaneSize, 0, 0);
	addVertex(0, 0, 0);
	endPrimitives();

	// plane edges
	beginPrimitives(eGfxPrimitive_LineList);
	
	if ((rGizmo.m_selectedGizmoAxis & AxisGizmo::eFlag_TranslatePlaneXY)
		|| (rGizmo.m_hoveredGizmoAxis & AxisGizmo::eFlag_TranslatePlaneXY))
	{
		addColor(1, 1, 0, 1);
	}
	else
	{
		addColor(1, 0, 0, 1);
	}

	addVertex(rGizmo.m_gizmoPlaneSize, 0, 0);
	addVertex(rGizmo.m_gizmoPlaneSize, rGizmo.m_gizmoPlaneSize, 0);
	
	if ((rGizmo.m_selectedGizmoAxis & AxisGizmo::eFlag_TranslatePlaneXY)
		|| (rGizmo.m_hoveredGizmoAxis & AxisGizmo::eFlag_TranslatePlaneXY))
	{
		addColor(1, 1, 0, 1);
	}
	else
	{
		addColor(0, 1, 0, 1);
	}

	addVertex(0, rGizmo.m_gizmoPlaneSize, 0);
	addVertex(rGizmo.m_gizmoPlaneSize, rGizmo.m_gizmoPlaneSize, 0);
	endPrimitives();

	// draw plane YZ
	beginPrimitives(eGfxPrimitive_TriangleList);

	if ((rGizmo.m_selectedGizmoAxis & AxisGizmo::eFlag_TranslatePlaneYZ)
		|| (rGizmo.m_hoveredGizmoAxis & AxisGizmo::eFlag_TranslatePlaneYZ))
	{
		addColor(rGizmo.m_selectedPlaneColor);
	}
	else
	{
		addColor(rGizmo.m_planeColor);
	}

	addVertex(0, 0, 0);
	addVertex(0, 0, rGizmo.m_gizmoPlaneSize);
	addVertex(0, rGizmo.m_gizmoPlaneSize, rGizmo.m_gizmoPlaneSize);

	addVertex(0, rGizmo.m_gizmoPlaneSize, rGizmo.m_gizmoPlaneSize);
	addVertex(0, rGizmo.m_gizmoPlaneSize, 0);
	addVertex(0, 0, 0);
	endPrimitives();

	// plane edges
	beginPrimitives(eGfxPrimitive_LineList);

	if ((rGizmo.m_selectedGizmoAxis & AxisGizmo::eFlag_TranslatePlaneYZ)
		|| (rGizmo.m_hoveredGizmoAxis & AxisGizmo::eFlag_TranslatePlaneYZ))
	{
		addColor(1, 1, 0, 1);
	}
	else
	{
		addColor(0, 1, 0, 1);
	}

	addVertex(0, rGizmo.m_gizmoPlaneSize, 0);
	addVertex(0, rGizmo.m_gizmoPlaneSize, rGizmo.m_gizmoPlaneSize);

	if ((rGizmo.m_selectedGizmoAxis & AxisGizmo::eFlag_TranslatePlaneYZ)
		|| (rGizmo.m_hoveredGizmoAxis & AxisGizmo::eFlag_TranslatePlaneYZ))
	{
		addColor(1, 1, 0, 1);
	}
	else
	{
		addColor(0, 0, 1, 1);
	}

	addVertex(0, 0, rGizmo.m_gizmoPlaneSize);
	addVertex(0, rGizmo.m_gizmoPlaneSize, rGizmo.m_gizmoPlaneSize);
	endPrimitives();

	// draw plane ZX
	beginPrimitives(eGfxPrimitive_TriangleList);

	if ((rGizmo.m_selectedGizmoAxis & AxisGizmo::eFlag_TranslatePlaneZX)
		|| (rGizmo.m_hoveredGizmoAxis & AxisGizmo::eFlag_TranslatePlaneZX))
	{
		addColor(rGizmo.m_selectedPlaneColor);
	}
	else
	{
		addColor(rGizmo.m_planeColor);
	}

	addVertex(0, 0, 0);
	addVertex(0, 0, rGizmo.m_gizmoPlaneSize);
	addVertex(rGizmo.m_gizmoPlaneSize, 0, rGizmo.m_gizmoPlaneSize);

	addVertex(rGizmo.m_gizmoPlaneSize, 0, rGizmo.m_gizmoPlaneSize);
	addVertex(rGizmo.m_gizmoPlaneSize, 0, 0);
	addVertex(0, 0, 0);
	endPrimitives();

	// plane edges
	beginPrimitives(eGfxPrimitive_LineList);

	if ((rGizmo.m_selectedGizmoAxis & AxisGizmo::eFlag_TranslatePlaneZX)
		|| (rGizmo.m_hoveredGizmoAxis & AxisGizmo::eFlag_TranslatePlaneZX))
	{
		addColor(1, 1, 0, 1);
	}
	else
	{
		addColor(1, 0, 0, 1);
	}

	addVertex(rGizmo.m_gizmoPlaneSize, 0, 0);
	addVertex(rGizmo.m_gizmoPlaneSize, 0, rGizmo.m_gizmoPlaneSize);

	if ((rGizmo.m_selectedGizmoAxis & AxisGizmo::eFlag_TranslatePlaneZX)
		|| (rGizmo.m_hoveredGizmoAxis & AxisGizmo::eFlag_TranslatePlaneZX))
	{
		addColor(1, 1, 0, 1);
	}
	else
	{
		addColor(0, 0, 1, 1);
	}

	addVertex(0, 0, rGizmo.m_gizmoPlaneSize);
	addVertex(rGizmo.m_gizmoPlaneSize, 0, rGizmo.m_gizmoPlaneSize);
	endPrimitives();

	// draw axis
	beginPrimitives(eGfxPrimitive_LineList);

	if ((rGizmo.m_hoveredGizmoAxis & AxisGizmo::eFlag_TranslateX)
		|| (rGizmo.m_selectedGizmoAxis & AxisGizmo::eFlag_TranslateX))
	{
		addColor(rGizmo.m_selectedAxisColor);
	}
	else
	{
		addColor(1, 0, 0);
	}

	addVertex(rGizmo.m_gizmoPlaneSize / 2, 0, 0);
	addVertex(rGizmo.m_axisSize, 0, 0);

	if ((rGizmo.m_hoveredGizmoAxis & AxisGizmo::eFlag_TranslateY)
		|| (rGizmo.m_selectedGizmoAxis & AxisGizmo::eFlag_TranslateY))
	{
		addColor(rGizmo.m_selectedAxisColor);
	}
	else
	{
		addColor(0, 1, 0);
	}

	addVertex(0, rGizmo.m_gizmoPlaneSize / 2, 0);
	addVertex(0, rGizmo.m_axisSize, 0);

	if ((rGizmo.m_hoveredGizmoAxis & AxisGizmo::eFlag_TranslateZ)
		|| (rGizmo.m_selectedGizmoAxis & AxisGizmo::eFlag_TranslateZ))
	{
		addColor(rGizmo.m_selectedAxisColor);
	}
	else
	{
		addColor(0, 0, 1);
	}

	addVertex(0, 0, rGizmo.m_gizmoPlaneSize / 2.0f);
	addVertex(0, 0, rGizmo.m_axisSize);

	endPrimitives();

	//
	// draw scale handles
	//

	if (rGizmo.m_bDrawScaleGizmo)
	{
		f32 scaleHandlesPosition = rGizmo.m_bDrawTranslateGizmo ? rGizmo.m_scaleHandlesPosition : 1.0f;
		setMatrixMode(eMatrixMode_World);

		if ((rGizmo.m_selectedGizmoAxis & AxisGizmo::eFlag_ScaleX)
			|| (rGizmo.m_hoveredGizmoAxis & AxisGizmo::eFlag_ScaleX))
		{
			addColor(1, 0, 0);
		}
		else
		{
			addColor(0.5, 0, 0);
		}

		loadIdentity();
		translate(rGizmo.m_translation.x, rGizmo.m_translation.y, rGizmo.m_translation.z);

		if (!bWorldMode)
		{
			multiplyMatrix(toMatrix(rGizmo.m_rotation));
		}

		scale(rGizmo.m_axisScale, rGizmo.m_axisScale, rGizmo.m_axisScale);
		translate(rGizmo.m_axisSize * scaleHandlesPosition, 0, 0);
		drawSolidBox(rGizmo.m_scaleKnobSize, rGizmo.m_scaleKnobSize, rGizmo.m_scaleKnobSize);

		if ((rGizmo.m_selectedGizmoAxis & AxisGizmo::eFlag_ScaleY)
			|| (rGizmo.m_hoveredGizmoAxis & AxisGizmo::eFlag_ScaleY))
		{
			addColor(0, 1, 0);
		}
		else
		{
			addColor(0, 0.5f, 0);
		}

		loadIdentity();
		translate(rGizmo.m_translation.x, rGizmo.m_translation.y, rGizmo.m_translation.z);

		if (!bWorldMode)
		{
			multiplyMatrix(toMatrix(rGizmo.m_rotation));
		}

		scale(rGizmo.m_axisScale, rGizmo.m_axisScale, rGizmo.m_axisScale);
		translate(0, rGizmo.m_axisSize * scaleHandlesPosition, 0);
		drawSolidBox(rGizmo.m_scaleKnobSize, rGizmo.m_scaleKnobSize, rGizmo.m_scaleKnobSize);

		if ((rGizmo.m_selectedGizmoAxis & AxisGizmo::eFlag_ScaleZ)
			|| (rGizmo.m_hoveredGizmoAxis & AxisGizmo::eFlag_ScaleZ))
		{
			addColor(0, 1, 1);
		}
		else
		{
			addColor(0, 0, 0.5f);
		}

		loadIdentity();
		translate(rGizmo.m_translation.x, rGizmo.m_translation.y, rGizmo.m_translation.z);

		if (!bWorldMode)
		{
			multiplyMatrix(toMatrix(rGizmo.m_rotation));
		}

		scale(rGizmo.m_axisScale, rGizmo.m_axisScale, rGizmo.m_axisScale);
		translate(0, 0, rGizmo.m_axisSize * scaleHandlesPosition);
		drawSolidBox(rGizmo.m_scaleKnobSize, rGizmo.m_scaleKnobSize, rGizmo.m_scaleKnobSize);
	}

	//
	// draw rotation handles
	//

	if (rGizmo.m_bDrawRotationGizmo)
	{
		Vec3 pt;

		rGizmo.m_rotationAxisCirclePoints[0].clear();
		rGizmo.m_rotationAxisCirclePoints[1].clear();
		rGizmo.m_rotationAxisCirclePoints[2].clear();

		// X disc
		loadIdentity();
		translate(rGizmo.m_translation.x, rGizmo.m_translation.y, rGizmo.m_translation.z);
		scale(rGizmo.m_axisScale, rGizmo.m_axisScale, rGizmo.m_axisScale);

		if (!bWorldMode)
		{
			multiplyMatrix(toMatrix(rGizmo.m_rotation));
		}

		translate(rGizmo.m_axisSize * rGizmo.m_rotationHandlesPosition, 0, 0);

		if (rGizmo.m_selectedGizmoAxis == AxisGizmo::eFlag_RotateX
			|| rGizmo.m_hoveredGizmoAxis == AxisGizmo::eFlag_RotateX)
		{
			beginPrimitives(eGfxPrimitive_TriangleFan);
			addColor(1, 1, 0, 0.3f);
			addVertex(0, 0, 0);

			for (f32 i = 0; i <= kConst_2PI; i += kConst_2PI / kAxisGizmoRotationCirclePoints)
			{
				addColor(1, 1, 0, 0.5f);
				pt.set(
					0,
					sinf(i) * rGizmo.m_rotationHandleRadius,
					cosf(i) * rGizmo.m_rotationHandleRadius);
				addVertex(pt);
			}

			pt.set(
				0,
				sinf(kConst_2PI) * rGizmo.m_rotationHandleRadius,
				cosf(kConst_2PI) * rGizmo.m_rotationHandleRadius);
			addVertex(pt);
			endPrimitives();
		}

		// draw X circle
		beginPrimitives(eGfxPrimitive_LineStrip);

		for (f32 i = 0; i <= kConst_2PI; i += kConst_2PI / kAxisGizmoRotationCirclePoints)
		{
			if (rGizmo.m_selectedGizmoAxis == AxisGizmo::eFlag_RotateX
				|| rGizmo.m_hoveredGizmoAxis == AxisGizmo::eFlag_RotateX)
			{
				addColor(1, 1, 0, 1);
			}
			else
			{
				addColor(0.5f, 0, 0, 1);
			}

			pt.set(0, sinf(i)*rGizmo.m_rotationHandleRadius, cosf(i)*rGizmo.m_rotationHandleRadius);
			rGizmo.m_rotationAxisCirclePoints[0].append(pt);
			addVertex(pt);
		}

		pt.set(0, sinf(kConst_2PI)*rGizmo.m_rotationHandleRadius, cosf(kConst_2PI)*rGizmo.m_rotationHandleRadius);
		addVertex(pt);
		rGizmo.m_rotationAxisCirclePoints[0].append(pt);
		endPrimitives();

		if (rGizmo.m_selectedGizmoAxis == AxisGizmo::eFlag_RotateX
			|| rGizmo.m_hoveredGizmoAxis == AxisGizmo::eFlag_RotateX)
		{
			addColor(1.0f, 1.0f, 0.0f, 1.0f);
		}
		else
		{
			addColor(0.5f, 0.0f, 0.0f, 1.0f);
		}

		drawSolidEllipsoid(rGizmo.m_rotationSphereRadius, rGizmo.m_rotationSphereRadius, rGizmo.m_rotationSphereRadius);

		// Y disc
		loadIdentity();
		translate(rGizmo.m_translation.x, rGizmo.m_translation.y, rGizmo.m_translation.z);
		scale(rGizmo.m_axisScale, rGizmo.m_axisScale, rGizmo.m_axisScale);

		if (!bWorldMode)
		{
			multiplyMatrix(toMatrix(rGizmo.m_rotation));
		}

		translate(0, rGizmo.m_axisSize * rGizmo.m_rotationHandlesPosition, 0);

		if (rGizmo.m_selectedGizmoAxis == AxisGizmo::eFlag_RotateY
			|| rGizmo.m_hoveredGizmoAxis == AxisGizmo::eFlag_RotateY)
		{
			beginPrimitives(eGfxPrimitive_TriangleFan);
			addColor(1, 1, 0, 0.3f);
			addVertex(0, 0, 0);

			for (f32 i = 0; i <= kConst_2PI; i += kConst_2PI / kAxisGizmoRotationCirclePoints)
			{
				addColor(1, 1, 0, 0.3f);
				pt.set(sinf(i)*rGizmo.m_rotationHandleRadius, 0, cosf(i)*rGizmo.m_rotationHandleRadius);
				addVertex(pt);
			}

			pt.set(sinf(kConst_2PI)*rGizmo.m_rotationHandleRadius, 0, cosf(kConst_2PI)*rGizmo.m_rotationHandleRadius);
			addVertex(pt);
			endPrimitives();
		}

		// Y circle
		beginPrimitives(eGfxPrimitive_LineStrip);

		for (f32 i = 0; i <= kConst_2PI; i += kConst_2PI / kAxisGizmoRotationCirclePoints)
		{
			if (rGizmo.m_selectedGizmoAxis == AxisGizmo::eFlag_RotateY
				|| rGizmo.m_hoveredGizmoAxis == AxisGizmo::eFlag_RotateY)
			{
				addColor(1, 1, 0, 1);
			}
			else
			{
				addColor(0, 0.5f, 0, 1);
			}

			pt.set(sinf(i)*rGizmo.m_rotationHandleRadius, 0, cosf(i)*rGizmo.m_rotationHandleRadius);
			rGizmo.m_rotationAxisCirclePoints[1].append(pt);
			addVertex(pt);
		}

		pt.set(sinf(kConst_2PI)*rGizmo.m_rotationHandleRadius, 0, cosf(kConst_2PI)*rGizmo.m_rotationHandleRadius);
		addVertex(pt);
		rGizmo.m_rotationAxisCirclePoints[1].append(pt);
		endPrimitives();

		if (rGizmo.m_selectedGizmoAxis == AxisGizmo::eFlag_RotateY
			|| rGizmo.m_hoveredGizmoAxis == AxisGizmo::eFlag_RotateY)
		{
			addColor(1.0f, 1.0f, 0.0f, 1.0f);
		}
		else
		{
			addColor(0.0f, 0.5f, 0.0f, 1.0f);
		}

		drawSolidEllipsoid(
			rGizmo.m_rotationSphereRadius,
			rGizmo.m_rotationSphereRadius,
			rGizmo.m_rotationSphereRadius);

		// Z disc
		loadIdentity();
		translate(rGizmo.m_translation.x, rGizmo.m_translation.y, rGizmo.m_translation.z);
		scale(rGizmo.m_axisScale, rGizmo.m_axisScale, rGizmo.m_axisScale);

		if (!bWorldMode)
		{
			multiplyMatrix(toMatrix(rGizmo.m_rotation));
		}

		translate(0, 0, rGizmo.m_axisSize * rGizmo.m_rotationHandlesPosition);

		if (rGizmo.m_selectedGizmoAxis == AxisGizmo::eFlag_RotateZ
			|| rGizmo.m_hoveredGizmoAxis == AxisGizmo::eFlag_RotateZ)
		{
			beginPrimitives(eGfxPrimitive_TriangleFan);
			addColor(1, 1, 0, 0.3f);
			addVertex(0, 0, 0);

			for (f32 i = 0; i <= kConst_2PI; i += kConst_2PI / kAxisGizmoRotationCirclePoints)
			{
				addColor(1, 1, 0, 0.3f);
				pt.set(sinf(i)*rGizmo.m_rotationHandleRadius, cosf(i)*rGizmo.m_rotationHandleRadius, 0);
				addVertex(pt);
			}

			pt.set(sinf(kConst_2PI)*rGizmo.m_rotationHandleRadius, cosf(kConst_2PI)*rGizmo.m_rotationHandleRadius, 0);
			addVertex(pt);
			endPrimitives();
		}

		// Z circle
		beginPrimitives(eGfxPrimitive_LineStrip);

		for (f32 i = 0; i <= kConst_2PI; i += kConst_2PI / kAxisGizmoRotationCirclePoints)
		{
			if (rGizmo.m_selectedGizmoAxis == AxisGizmo::eFlag_RotateZ
				|| rGizmo.m_hoveredGizmoAxis == AxisGizmo::eFlag_RotateZ)
			{
				addColor(1, 1, 0, 1);
			}
			else
			{
				addColor(0, 0, 0.5f, 1);
			}

			pt.set(sinf(i)*rGizmo.m_rotationHandleRadius, cosf(i)*rGizmo.m_rotationHandleRadius, 0);
			rGizmo.m_rotationAxisCirclePoints[2].append(pt);
			addVertex(pt);
		}

		pt.set(sinf(kConst_2PI)*rGizmo.m_rotationHandleRadius, cosf(kConst_2PI)*rGizmo.m_rotationHandleRadius, 0);
		addVertex(pt);
		rGizmo.m_rotationAxisCirclePoints[2].append(pt);
		endPrimitives();

		if (rGizmo.m_selectedGizmoAxis == AxisGizmo::eFlag_RotateZ
			|| rGizmo.m_hoveredGizmoAxis == AxisGizmo::eFlag_RotateZ)
		{
			addColor(1.0f, 1.0f, 0.0f, 1.0f);
		}
		else
		{
			addColor(0.0f, 0.0f, 0.5f, 1.0f);
		}

		drawSolidEllipsoid(
			rGizmo.m_rotationSphereRadius,
			rGizmo.m_rotationSphereRadius,
			rGizmo.m_rotationSphereRadius);
	}

	//
	// draw translate axis cones
	//
	if (rGizmo.m_bDrawTranslateGizmo)
	{
		loadIdentity();
		translate(rGizmo.m_translation.x, rGizmo.m_translation.y, rGizmo.m_translation.z);
		scale(rGizmo.m_axisScale, rGizmo.m_axisScale, rGizmo.m_axisScale);

		if (!bWorldMode)
		{
			multiplyMatrix(toMatrix(rGizmo.m_rotation));
		}

		translate(0, 0, 0);

		f32 i;

		// draw X cone
		beginPrimitives(eGfxPrimitive_TriangleFan);

		if ((rGizmo.m_selectedGizmoAxis & AxisGizmo::eFlag_TranslateX)
			|| (rGizmo.m_hoveredGizmoAxis & AxisGizmo::eFlag_TranslateX))
		{
			addColor(1, 0, 0);
		}
		else
		{
			addColor(0.5f, 0, 0);
		}

		addVertex(rGizmo.m_axisSize + 0.1f, 0, 0);

		for (i = 0; i <= kConst_2PI; i += kConst_2PI / rGizmo.m_axisConeSegments)
		{
			addVertex(
				rGizmo.m_axisSize - rGizmo.m_axisConeHeight,
				sinf(i) * rGizmo.m_axisConeRadius,
				cosf(i) * rGizmo.m_axisConeRadius);
		}

		endPrimitives();

		// draw Y cone
		beginPrimitives(eGfxPrimitive_TriangleFan);

		if ((rGizmo.m_selectedGizmoAxis & AxisGizmo::eFlag_TranslateY)
			|| (rGizmo.m_hoveredGizmoAxis & AxisGizmo::eFlag_TranslateY))
		{
			addColor(0, 1, 0);
		}
		else
		{
			addColor(0, 0.5, 0);
		}

		addVertex(0, rGizmo.m_axisSize + 0.1f, 0);

		for (i = 0; i <= kConst_2PI; i += kConst_2PI / rGizmo.m_axisConeSegments)
		{
			addVertex(
				sinf(i) * rGizmo.m_axisConeRadius,
				rGizmo.m_axisSize - rGizmo.m_axisConeHeight,
				cosf(i) * rGizmo.m_axisConeRadius);
		}
		
		endPrimitives();

		// draw Z cone
		beginPrimitives(eGfxPrimitive_TriangleFan);

		if ((rGizmo.m_selectedGizmoAxis & AxisGizmo::eFlag_TranslateZ)
			|| (rGizmo.m_hoveredGizmoAxis & AxisGizmo::eFlag_TranslateZ))
		{
			addColor(0, 1, 1);
		}
		else
		{
			addColor(0, 0, 0.5);
		}

		addVertex(0, 0, rGizmo.m_axisSize + 0.1f);

		for (i = 0; i <= kConst_2PI; i += kConst_2PI / rGizmo.m_axisConeSegments)
		{
			addVertex(
				sinf(i) * rGizmo.m_axisConeRadius,
				cosf(i) * rGizmo.m_axisConeRadius,
				rGizmo.m_axisSize - rGizmo.m_axisConeHeight);
		}

		endPrimitives();
	}
}

void GfxManager::drawViewAxis(AxisGizmo& rGizmo)
{
	setBlending(false);
	setProjection2D(0, 0, viewWidth(), viewHeight());
	disableTexturing();
	setDepthTest(false);
	setDepthWrite(false);
	setCullMode(eCullMode_None);
	setFillMode(eFillMode_Solid);

	switch (m_pCamera->projection())
	{
	case Camera::eProjection_Front:
		{
			drawViewAxisWithColors(rGizmo, Color(1, 0, 0), Color(0, 1, 0));
			break;
		}

	case Camera::eProjection_Perspective:
		{
			//TODO: draw a little 3D axis ?
			break;
		}

	case Camera::eProjection_Left:
		{
			drawViewAxisWithColors(rGizmo, Color(0, 0.5f, 1), Color(0, 1, 0));
			break;
		}

	case Camera::eProjection_Top:
		{
			drawViewAxisWithColors(rGizmo, Color(1, 0, 0), Color(0, 0.5f, 1), true);
			break;
		}
	}
}

void GfxManager::drawViewAxisWithColors(AxisGizmo& rGizmo, const Color& rcX, const Color& rcY, bool specialZ)
{
	f32 yoffs = (specialZ ? rGizmo.m_arrowBodySize : 0);

	beginPrimitives(eGfxPrimitive_LineList);
	// X
	addColor(rcX);
	addVertex(rGizmo.m_arrowOffset, rGizmo.m_arrowOffset + yoffs);
	addColor(rcX);
	addVertex(rGizmo.m_arrowOffset + rGizmo.m_arrowBodySize, rGizmo.m_arrowOffset + yoffs);

	// Y
	addColor(rcY);
	addVertex(rGizmo.m_arrowOffset, rGizmo.m_arrowOffset);
	addColor(rcY);
	addVertex(rGizmo.m_arrowOffset, rGizmo.m_arrowOffset + rGizmo.m_arrowBodySize);
	endPrimitives();

	// arrows heads
	beginPrimitives(eGfxPrimitive_TriangleList);
	// X-arrow head
	addColor(rcX);
	addVertex(rGizmo.m_arrowOffset + rGizmo.m_arrowBodySize, rGizmo.m_arrowOffset + yoffs);
	addColor(rcX);
	addVertex(
		rGizmo.m_arrowOffset + rGizmo.m_arrowBodySize - rGizmo.m_arrowSize,
		rGizmo.m_arrowOffset + rGizmo.m_arrowAperture + yoffs);
	addColor(rcX);
	addVertex(
		rGizmo.m_arrowOffset + rGizmo.m_arrowBodySize - rGizmo.m_arrowSize,
		rGizmo.m_arrowOffset - rGizmo.m_arrowAperture + yoffs);

	// Y-arrow head
	if (!specialZ)
	{
		addColor(rcY);
		addVertex(rGizmo.m_arrowOffset, rGizmo.m_arrowOffset + rGizmo.m_arrowBodySize);
		addColor(rcY);
		addVertex(
			rGizmo.m_arrowOffset + rGizmo.m_arrowAperture,
			rGizmo.m_arrowOffset + rGizmo.m_arrowBodySize - rGizmo.m_arrowSize);
		addColor(rcY);
		addVertex(
			rGizmo.m_arrowOffset - rGizmo.m_arrowAperture,
			rGizmo.m_arrowOffset + rGizmo.m_arrowBodySize - rGizmo.m_arrowSize);
	}
	else
	{
		addColor(rcY);
		addVertex(rGizmo.m_arrowOffset, rGizmo.m_arrowOffset);
		addColor(rcY);
		addVertex(
			rGizmo.m_arrowOffset - rGizmo.m_arrowAperture,
			rGizmo.m_arrowOffset + rGizmo.m_arrowSize);
		addColor(rcY);
		addVertex(
			rGizmo.m_arrowOffset + rGizmo.m_arrowAperture,
			rGizmo.m_arrowOffset + rGizmo.m_arrowSize);
	}

	endPrimitives();
}
*/

}