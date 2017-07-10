#include <string.h>
#include "game/systems/render/render_system.h"
#include "core/core.h"
#include "game/game_manager.h"
#include "render/resources/gpu_program_resource.h"
#include "render/resources/material_resource.h"
#include "core/core.h"
#include "base/assert.h"
#include "base/logger.h"
#include "base/profiler.h"
#include "base/qsort.h"
#include "base/variable.h"
#include "game/component.h"
#include "game/game_manager.h"
#include "render/gpu_program_constant.h"
#include "render/renderer.h"
#include "render/scene/shadow_manager.h"
#include "render/postprocess/postprocess_manager.h"
#include "base/math/conversion.h"
#include "physics/physics_manager.h"
#include "physics/character.h"
#include "physics/space.h"
#include "input/mouse.h"
#include "input/input_manager.h"
#include "base/array.h"
#include "game/component_type_manager.h"
#include "game/types.h"
#include "render/scene/scene.h"

#include "game/components/camera.h"
#include "game/components/transform.h"
#include "game/components/mesh.h"
#include "game/components/first_person_view.h"

#include "render/scene/mesh_node.h"
#include "render/scene/camera_node.h"

namespace engine
{
B_REGISTER_CLASS(RenderSystem);
	
E_DECLARE_COMPONENT_MANAGER(TransformComponent, ComponentType::Transform);
E_DECLARE_COMPONENT_MANAGER(CameraComponent, ComponentType::Camera);
E_DECLARE_COMPONENT_MANAGER(FirstPersonViewComponent, ComponentType::FirstPersonView);
E_DECLARE_COMPONENT_MANAGER(MeshComponent, ComponentType::Mesh);

RenderSystem::RenderSystem()
{
	E_REGISTER_COMPONENT_MANAGER(TransformComponent);
	E_REGISTER_COMPONENT_MANAGER(CameraComponent);
	E_REGISTER_COMPONENT_MANAGER(FirstPersonViewComponent);
	E_REGISTER_COMPONENT_MANAGER(MeshComponent);
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::update()
{
	for (size_t i = 0, iCount = m_transformComponents.size(); i < iCount; ++i)
	{
		Transform::update(m_transformComponents[i]);
	}

	for (size_t i = 0, iCount = m_cameraComponents.size(); i < iCount; ++i)
	{
		Camera::update(m_cameraComponents[i]);
	}

	for (size_t i = 0, iCount = m_meshComponents.size(); i < iCount; ++i)
	{
		Mesh::update(m_meshComponents[i]);
	}
}

void RenderSystem::onEntityCreated(EntityId aId)
{
}

void RenderSystem::onEntityRemoved(EntityId aId)
{
}

void RenderSystem::onEntityParentChange(EntityId entity, EntityId oldParent, EntityId newParent)
{
	auto newParentXform = game().component<TransformComponent>(newParent);
	auto xform = game().component<TransformComponent>(entity);
	auto camera = game().component<CameraComponent>(entity);
	auto mesh = game().component<MeshComponent>(entity);
	
	SceneNodeId newParentNode = newParentXform ? newParentXform->nodeId : 0; 
	
	if (xform)
	{
		renderer().scene()->changeNodeParent(xform->nodeId, newParentNode);
	}

	if (camera)
	{
		renderer().scene()->changeNodeParent(camera->nodeId, newParentNode);
	}

	if (mesh)
	{

		//TODO!
		//for (size_t i = 0; i < mesh->partNodeIds.size(); ++i)
		//{
		//	if (!mesh->partNodeIds.keyAt(i)->pParent)
		//	{
		//		game().scene()->changeNodeParent(mesh->partNodeIds.valueAt(i), newParentNode);
		//	}
		//}
	}
}

//void addMeshPartNode(MeshComponent* pComp, EntityId entity, MeshPart* part, SceneNodeId parentNode)
//{
//	MeshNode* pNode = new MeshNode();
//	pNode->setName("MESH_PART");
//	pNode->setEntity(entity);
//	SceneNodeId nodeId = render().scene()->addNode(parentNode, pNode);
//	render().scene()->updateNode(nodeId, MeshNode::eUpdateData_MeshPart, &part, sizeof(part));
//	//TODO
//	//pComp->partNodeIds[part] = nodeId;
//
//	//if (!part->pParent)
//	//{
//	//	pComp->rootNodeIds.appendUnique(nodeId);
//	//}
//
//	//for (size_t i = 0; i < part->children.size(); ++i)
//	//{
//	//	addMeshPartNode(pComp, entity, part->children[i], nodeId);
//	//}
//}

void RenderSystem::onComponentAdded(EntityId entity, Component* pComp)
{
	switch (pComp->typeId)
	{
	case ComponentType::Transform:
		{
			auto comp = (TransformComponent*)pComp;
			m_transformComponents.appendUnique(comp);

			SceneNode* pNode = new SceneNode();
			pNode->m_name = "xform";
			//pNode->m_p(entity);
			SceneNodeId parentNode = parentTransformSceneNodeId(entity, true);
			comp->nodeId = renderer().scene()->addNode(parentNode, pNode);
			break;
		}
	case ComponentType::Camera:
		{
			auto comp = (CameraComponent*)pComp;
			m_cameraComponents.appendUnique(comp);

			CameraNode* pNode = new CameraNode();
			pNode->m_name = "camera";
			//pNode->setEntity(entity);
			SceneNodeId parentNode = parentTransformSceneNodeId(entity);

			if (1 == parentNode)
			{
				TransformComponent* xform = game().component<TransformComponent>(entity);

				if (xform)
				{
					parentNode = xform->nodeId;
				}
			}

			comp->nodeId = renderer().scene()->addNode(parentNode, pNode);
			break;
		}
	case ComponentType::Mesh:
		{
			auto comp = (MeshComponent*)pComp;
			m_meshComponents.appendUnique(comp);
			break;
		}
	}
}

void RenderSystem::onComponentRemoved(EntityId aId, Component* pComp)
{
	switch (pComp->typeId)
	{
	case ComponentType::Transform:
		{
			auto comp = (TransformComponent*)pComp;
			auto iter = m_transformComponents.find(comp);

			if (iter != m_transformComponents.end())
			{
				m_transformComponents.erase(iter);
			}

			renderer().scene()->deleteNode(comp->nodeId);
			break;
		}

	case ComponentType::Camera:
		{
			auto comp = (CameraComponent*)pComp;
			auto iter = m_cameraComponents.find(comp);

			if (iter != m_cameraComponents.end())
			{
				m_cameraComponents.erase(iter);
			}

			renderer().scene()->deleteNode(comp->nodeId);
			break;
		}

	case ComponentType::Mesh:
		{
			auto comp = (MeshComponent*)pComp;
			auto iter = m_meshComponents.find(comp);

			if (iter != m_meshComponents.end())
			{
				m_meshComponents.erase(iter);
			}

			//TODO
			//for (size_t i = 0; i < comp->rootNodeIds.size(); ++i)
			//{
			//	game().scene()->deleteNode(comp->rootNodeIds[i]);
			//}

			break;
		}
	}
}

SceneNodeId RenderSystem::parentTransformSceneNodeId(EntityId entity, bool bSkipEntityTransformComponent)
{
	EntityId parent = game().entityParent(entity);
	SceneNodeId parentNode = 1; //scene root node is always id 1

	if (!bSkipEntityTransformComponent)
	{
		auto xform = game().component<TransformComponent>(entity);

		if (xform)
		{
			return xform->nodeId;
		}
	}

	// if there is a parent for this entity, then get the parent scene node
	// from the transform component
	if (parent)
	{
		auto xform = game().component<TransformComponent>(parent);

		if (xform)
		{
			parentNode = xform->nodeId;
		}
		else
		{
			// search recursive until find an entity with a transform component
			parentNode = parentTransformSceneNodeId(parent);
		}
	}

	return parentNode;
}

void RenderSystem::Transform::update(TransformComponent* pXform)
{
	if (pXform->bDirty)
	{
		renderer().scene()->updateNode(pXform->nodeId,
			SceneNodeUpdateDataTypes::Transform,
			&pXform->data,
			sizeof(SceneNodeTransform));
		pXform->bDirty = false;
	}
}

void RenderSystem::Transform::move(TransformComponent* pXform, const Vec3& delta)
{
	if (delta.isEquivalent(Vec3(), 0.0001f))
	{
		return;
	}

	Matrix m = renderer().scene()->nodeWorldMatrix(pXform->nodeId);
	Vec3 deltaLocal;

	m.invert();
	m.rotationOnly().transform(delta, deltaLocal);
	pXform->data.translation += deltaLocal;
	pXform->data.bDirty = true;
	pXform->bDirty = true;
}

void RenderSystem::syncCameraToScene(CameraComponent* pCamera)
{
	if (pCamera->bDirty)
	{
		renderer().scene()->updateNode(pCamera->nodeId,
			CameraNode::eUpdateData_CameraData,
			&pCamera->data,
			sizeof(CameraData));
		pCamera->bDirty = false;
	}
}

void RenderSystem::Mesh::update(MeshComponent* pMesh)
{
	if (pMesh->bDirty)
	{
		//TODO
		//for (size_t i = 0; i < pMesh->rootNodeIds.size(); ++i)
		//{
		//	game().scene()->deleteNode(pMesh->rootNodeIds[i]);
		//}

		//SceneNodeId parentNode = parentTransformSceneNodeId(pMesh->entityId);

		//if (pMesh->mesh)
		//{
		//	for (size_t i = 0; i < pMesh->mesh->meshParts().size(); ++i)
		//	{
		//		auto part = pMesh->mesh->meshParts()[i];

		//		if (!part->pParent)
		//		{
		//			addMeshPartNode(pMesh, pMesh->entityId, part, parentNode);
		//		}
		//	}

		//	pMesh->bDirty = false;
		//}
	}
}

bool RenderSystem::FirstPersonView::createPhysicsCapsule(FirstPersonViewComponent* pFpView, f32 aCapsuleRadius, f32 aCapsuleHeight)
{
	if (!&physics())
	{
		return false;
	}

	pFpView->standupHeight = pFpView->avatarCurrentHeight = aCapsuleHeight;
	pFpView->pCharacter = physics().currentSpace()->createCharacter();
	pFpView->pCharacter->create(aCapsuleRadius, aCapsuleHeight);

	return true;
}

void RenderSystem::FirstPersonView::destroyPhysicsCapsule(FirstPersonViewComponent* pFpView)
{
	B_SAFE_DELETE(pFpView->pCharacter);
}

void RenderSystem::FirstPersonView::update(FirstPersonViewComponent* pFpView, CameraComponent* pCamera, TransformComponent* pTransform)
{
	Vec3 pt;
	Matrix m;

	if (!pFpView->pCharacter)
	{
		return;
	}

	B_ASSERT(pCamera);
	Mouse* pMouse = (Mouse*)input().device(InputDeviceTypes::Mouse);

	// input from mouse
	if (pMouse)
	{
		pFpView->m_pitchSmoother.addValue(pMouse->delta().y);
		pFpView->m_yawSmoother.addValue(pMouse->delta().x);

		f32 pd = pFpView->m_pitchSmoother.compute();
		f32 yd = pFpView->m_yawSmoother.compute();

		// rotate camera view direction by pitch
		Camera::addPitchAngle(pCamera, pTransform, -pd * pFpView->lookAroundSpeedMultiplier);
		// rotate camera view direction by yaw
		Camera::addYawAngle(pCamera, pTransform, -yd * pFpView->lookAroundSpeedMultiplier);
	}

	Vec3 avatarPos = pFpView->pCharacter ? pFpView->pCharacter->translation() : pTransform->data.translation;
	Vec3 avatarDeltaMove;

	// jump
	if (pFpView->bJumping)
	{
		//TODO constant here
		pFpView->jumpCurrentForce += 3.0f * gameDeltaTime();

		const f32 G = physics().currentSpace()->gravity().y;
		f32 jumpVec = G * pFpView->jumpCurrentForce	* pFpView->jumpCurrentForce	
			+ pFpView->jumpInitialForce * pFpView->jumpCurrentForce;

		avatarDeltaMove.y += (jumpVec - G) * gameDeltaTime();
	}

	// crouch
	if (pFpView->bCrouching)
	{
		f32 deltaY = pFpView->crouchSpeed * gameDeltaTime();
		pFpView->avatarCurrentHeight -= deltaY;

		if (pFpView->avatarCurrentHeight <= pFpView->crouchHeight)
		{
			pFpView->bCrouching = false;
			pFpView->bCrouched = true;
			pFpView->avatarCurrentHeight = pFpView->crouchHeight;
			B_CALL_OBSERVERS_METHOD_OF(pFpView, FirstPersonViewObserver, onCrouchEnd(pFpView));
		}

		pFpView->pCharacter->setTranslation(Vec3(avatarPos.x, avatarPos.y - deltaY / 2.0f, avatarPos.z));
		pFpView->pCharacter->setCapsuleHeight(pFpView->avatarCurrentHeight);
	}

	// stand up
	if (pFpView->bStandingUp)
	{
		f32 deltaY = pFpView->standUpSpeed * gameDeltaTime();
		pFpView->avatarCurrentHeight += deltaY;

		if (pFpView->avatarCurrentHeight >= pFpView->standupHeight)
		{
			pFpView->bStandingUp = false;
			pFpView->avatarCurrentHeight = pFpView->standupHeight;
			B_CALL_OBSERVERS_METHOD_OF(pFpView, FirstPersonViewObserver, onStandUpEnd(pFpView));
		}

		pFpView->pCharacter->setTranslation(Vec3(avatarPos.x, avatarPos.y + deltaY, avatarPos.z));
		pFpView->pCharacter->setCapsuleHeight(pFpView->avatarCurrentHeight);
	}

	if (!pFpView->bJumping)
	{
		pFpView->walkBounceAngle +=
			gameDeltaTime() * pFpView->walkBounceDir
			* pFpView->walkBounceSpeed * (pFpView->bIsPlayerRunning ? pFpView->runningMultiplier : 1)
			* pFpView->bIsPlayerMoving;
		pt.set(-sinf(pFpView->walkBounceAngle) / pFpView->walkAngleAperture, cosf(pFpView->walkBounceAngle) / pFpView->walkAngleAperture, 1);

		if (pFpView->bDisableWalkBounce)
		{
			pt.set(0, 0, 1);
		}

		m.lookAt(Vec3(), pt, Vec3(0.0f, 0.0f, 1.0f));

		if (pFpView->walkBounceAngle > kPi / 2.0f + kPi)
		{
			pFpView->walkBounceDir = -1.0f;
			pFpView->walkBounceAngle = kPi / 2.0f + kPi;
			B_CALL_OBSERVERS_METHOD_OF(pFpView, FirstPersonViewObserver, onFootStep(pFpView));
		}

		if (pFpView->walkBounceAngle < kPi / 2.0f)
		{
			pFpView->walkBounceDir = 1.0f;
			pFpView->walkBounceAngle = kPi / 2.0f;
			B_CALL_OBSERVERS_METHOD_OF(pFpView, FirstPersonViewObserver, onFootStep(pFpView));
		}

		if (pFpView->strafeRollDir != 0)
		{
			if (fabsf(pFpView->strafeRoll) < pFpView->strafeRollAngle)
				pFpView->strafeRoll = B_LERP(pFpView->strafeRoll,
									pFpView->strafeRollDir * pFpView->strafeRollAngle,
									gameDeltaTime() * pFpView->strafeRollSpeed
									* (pFpView->bIsPlayerRunning ? pFpView->runningMultiplier : 1));
			else
				pFpView->strafeRoll = pFpView->strafeRollDir * pFpView->strafeRollAngle;
		}
		else
		{
			pFpView->strafeRoll = B_LERP(pFpView->strafeRoll,
								0, gameDeltaTime()
								* pFpView->strafeRollSpeed
								* (pFpView->bIsPlayerRunning ? pFpView->runningMultiplier : 1));
		}
	}

	// prepare camera matrices and other camera variables
	pTransform->data.roll = pFpView->strafeRoll;

	// compute effects
	if (pFpView->bShakeEffect)
	{
		Quat q1, q2, q;

		if (pFpView->shakeEffectTimer >= 1)
		{
			toQuat(sinf(randomFloat(-kPi, kPi)) * pFpView->shakeStrength, 1, 0, 0, q1);
			toQuat(cosf(randomFloat(-kPi, kPi)) * pFpView->shakeStrength, 0, 1, 0, q2);
			q = q1 * q2;
			pFpView->m_shakeCurrentRot = pFpView->m_shakeFinalRot;
			pFpView->m_shakeFinalRot = q;
			pFpView->shakeEffectTimer = 0;
		}
		else
		{
			pFpView->m_shakeCurrentRot.blend(pFpView->shakeEffectTimer, pFpView->m_shakeFinalRot);
			pFpView->shakeEffectTimer += gameDeltaTime() * pFpView->shakeSpeed;
		}

		m *= toMatrix(pFpView->m_shakeCurrentRot);
	}

	if (pFpView->bFovWarpEffect)
	{
		pFpView->m_fovWarpCurrentAngle += pFpView->fovWarpDir * pFpView->fovWarpSpeed * gameDeltaTime();
		f32 f1, f2;

		f2 = pFpView->fovWarpToAngle;
		f1 = pFpView->fovWarpNormal;

		if (pFpView->fovWarpDir == 1.0f && pFpView->m_fovWarpCurrentAngle > f1)
		{
			pCamera->data.fov = f1;
			pFpView->bFovWarpEffect = false;
		}
		else if (pFpView->fovWarpDir == -1.0f && pFpView->m_fovWarpCurrentAngle < f2)
		{
			pCamera->data.fov = f2;
			pFpView->bFovWarpEffect = false;
		}
		else
		{
			pCamera->data.fov = pFpView->m_fovWarpCurrentAngle;
		}
	}

	if (pFpView->pCharacter)
	{
		Vec3 finalDir = pFpView->avatarMoveDirection + avatarDeltaMove;

		pFpView->pCharacter->moveCharacter(finalDir, pFpView->moveMinDistance, pFpView->moveAutoStepSharpness);

		if (pFpView->pCharacter->isCollidingGround() && pFpView->bJumping)
		{
			pFpView->bJumping = false;
			B_CALL_OBSERVERS_METHOD_OF(pFpView, FirstPersonViewObserver, onJumpEnd(pFpView));
		}
	}

	//pTransform->worldMatrix = pTransform->worldMatrix * m;
}

void RenderSystem::FirstPersonView::reset(FirstPersonViewComponent* pFpView)
{
	pFpView->bIsPlayerMoving = false;
	pFpView->bIsPlayerRunning = false;
	pFpView->strafeRollDir = 0;
	pFpView->avatarMoveDirection.clear();
}

void RenderSystem::FirstPersonView::startEffect(
	FirstPersonViewComponent* pFpView,
	CameraEffect aEffect,
	f32 aParam1,
	f32 aParam2,
	f32 aParam3)
{
	switch (aEffect)
	{
	case CameraEffects::Shake:
		{
			pFpView->bShakeEffect = true;
			pFpView->shakeEffectTimer = 0;
			break;
		}

	case CameraEffects::FovWarp:
		{
			if (aParam1 != 0)
			{
				pFpView->fovWarpDir = aParam1;
			}
			else
			{
				pFpView->fovWarpDir = pFpView->fovWarpDir == 1 ? -1 : 1;
			}

			pFpView->bFovWarpEffect = true;
			//TODO: get fov from camera
			pFpView->m_fovWarpCurrentAngle = 50.0f;//->fov();
			break;
		}
	}
}

void RenderSystem::FirstPersonView::stopEffect(FirstPersonViewComponent* pFpView, CameraEffect aEffect)
{
	switch (aEffect)
	{
	case CameraEffects::Shake:
		{
			pFpView->bShakeEffect = false;
			break;
		}

	case CameraEffects::FovWarp:
		{
			pFpView->bFovWarpEffect = false;
			break;
		}
	}
}

bool RenderSystem::FirstPersonView::isEffectRunning(FirstPersonViewComponent* pFpView, CameraEffect aEffect)
{
	switch (aEffect)
	{
	case CameraEffects::Shake:
		{
			return pFpView->bShakeEffect;
			break;
		}

	case CameraEffects::FovWarp:
		{
			return pFpView->bFovWarpEffect;
			break;
		}
	}

	return false;
}

void RenderSystem::FirstPersonView::move(
	FirstPersonViewComponent* pFpView,
	const Vec3& rDelta,
	f32 aMinDistance,
	f32 aAutoStepSharpness)
{
	pFpView->avatarMoveDirection = rDelta;
	pFpView->moveMinDistance = aMinDistance;
	pFpView->moveAutoStepSharpness = aAutoStepSharpness;
}

void RenderSystem::FirstPersonView::jump(FirstPersonViewComponent* pFpView)
{
	if (!pFpView->pCharacter)
	{
		return;
	}

	if (pFpView->bJumping)
	{
		return;
	}

	if (pFpView->bCrouched)
	{
		FirstPersonView::standUp(pFpView);
		return;
	}

	pFpView->bCrouched = false;

	if (!pFpView->pCharacter->isCollidingGround() || pFpView->pCharacter->isCollidingUp())
	{
		return;
	}

	pFpView->bJumping = true;
	pFpView->jumpCurrentForce = 0;
	B_CALL_OBSERVERS_METHOD_OF(pFpView, FirstPersonViewObserver, onJumpStart(pFpView));
}

void RenderSystem::FirstPersonView::crouch(FirstPersonViewComponent* pFpView)
{
	if (!pFpView->pCharacter)
	{
		return;
	}

	if (pFpView->bJumping
		|| pFpView->bCrouching
		|| pFpView->bCrouched)
	{
		return;
	}

	if (!pFpView->pCharacter->isCollidingGround())
	{
		return;
	}

	pFpView->bCrouching = true;

	if (!pFpView->bStandingUp)
	{
		pFpView->avatarCurrentHeight = pFpView->standupHeight;
	}
	else
	{
		pFpView->bStandingUp = false;
		B_CALL_OBSERVERS_METHOD_OF(pFpView, FirstPersonViewObserver, onStandUpEnd(pFpView));
	}

	B_CALL_OBSERVERS_METHOD_OF(pFpView, FirstPersonViewObserver, onCrouchStart(pFpView));
}

void RenderSystem::FirstPersonView::standUp(FirstPersonViewComponent* pFpView)
{
	if (!pFpView->pCharacter)
	{
		return;
	}

	if (pFpView->bJumping
		|| pFpView->bStandingUp
		|| !pFpView->bCrouched
		|| !pFpView->pCharacter->isCollidingGround())
	{
		return;
	}

	pFpView->bStandingUp = true;
	pFpView->bCrouched = false;

	if (pFpView->bCrouching)
	{
		pFpView->bCrouching = false;
		B_CALL_OBSERVERS_METHOD_OF(pFpView, FirstPersonViewObserver, onCrouchEnd(pFpView));
	}

	B_CALL_OBSERVERS_METHOD_OF(pFpView, FirstPersonViewObserver, onStandUpStart(pFpView));
}

bool RenderSystem::FirstPersonView::canStandUp(FirstPersonViewComponent* pFpView)
{
	//TODO
	return false;
}

void RenderSystem::FirstPersonView::lean(FirstPersonViewComponent* pFpView, LeaningDirection aDir)
{
	//TODO
}

//void RenderSystem::free(ModelInstancerComponent* pComp)
//{
//	// delete all instance buffers
//	for (u32 i = 0, iCount = pComp->instanceBuffers.size(); i < iCount; ++i)
//	{
//		N_SAFE_DELETE(pComp->instanceBuffers[i]);
//	}
//
//	pComp->pModelData = nullptr;
//	pComp->instanceBuffers.clear();
//	pComp->renderPackets.clear();
//}

//void setFromResource(ModelInstancer* pComp, ModelData* pRes)
//{
//	free();
//	N_ASSERT(pRes);
//	m_pModelData = pRes;
//
//	if (!pRes)
//		return;
//
//	N_ASSERT(m_levelOfDetail >= 0 && m_levelOfDetail < m_pModelData->levelsOfDetail().size());
//
//	// get the current LOD mesh
//	MeshData* pMesh = m_pModelData->levelsOfDetail()[m_levelOfDetail]->m_pMesh;
//
//	// lets create the instances buffers for each submesh (vertexbuffer) in the mesh
//	//TODO: model instancer
//	/*
//	for( uint32 i = 0, iCount = pMesh->getSubMeshes().size(); i < iCount; i++ )
//	{
//		GeometryInstanceBuffer* pInstanceBuffer = (GeometryInstanceBuffer*)getGraphicsProvider()->newInstanceOf( GraphicsProvider::eClassId_GeometryInstanceBuffer );
//		RenderPacket packet;
//		Material* pMaterial = m_pModelData->getLevelsOfDetail()[m_levelOfDetail]->m_subMeshesMaterials[i];
//
//		N_ASSERT( pInstanceBuffer );
//
//		if( !pInstanceBuffer )
//			continue;
//
//		pMaterial->preCacheResources();
//		m_instanceBuffers.append(pInstanceBuffer);
//
//		packet.m_pObject = this;
//		packet.m_pGeometryBuffer = pMesh->subMeshes()[i];
//		packet.m_pInstanceBuffer = pInstanceBuffer;
//		packet.m_pMaterial = pMaterial;
//		m_renderPackets.push_back( packet );
//	}*/
//}
//
//void ModelInstancer::setInstanceCount(u32 aCount)
//{
//	for (u32 i = 0; i < m_instanceBuffers.size(); ++i)
//	{
//		m_instanceBuffers[i]->setInstanceCount(aCount);
//	}
//}
//
//void ModelInstancer::setInstanceData(void* pData, u32 aStartInstanceIndex, u32 aInstanceCount)
//{
//	N_ASSERT(pData);
//	MeshData* pMesh = m_pModelData->levelsOfDetail()[m_levelOfDetail]->m_pMesh;
//
//	//TODO: model instancer set data
//	/*
//	for( uint32 i = 0, iCount = m_instanceBuffers.size(); i < iCount; ++i )
//	{
//		// first build the buffer with the current count and instance size
//		m_instanceBuffers[i]->create( m_instanceAttributes, m_instanceBuffers[i]->getInstanceCount(), pMesh->getSubMeshes()[i]->getVertexBuffer() );
//		uint8* pInstData = m_instanceBuffers[i]->getInstanceDataBuffer();
//
//		N_ASSERT( pInstData );
//		N_ASSERT( pData );
//
//		if( pInstData && pData )
//		{
//			memcpy( pInstData, pData, m_instanceBuffers[i]->getInstanceDataBufferSize() );
//		}
//
//		m_instanceBuffers[i]->uploadInstanceDataBuffer( aStartInstanceIndex, aInstanceCount );
//	}*/
//}
//
//void ModelInstancer::updateInstanceData(void* pData, u32 aStartInstanceIndex, u32 aInstanceCount)
//{
//	N_ASSERT(pData);
//
//	for (u32 i = 0, iCount = m_instanceBuffers.size(); i < iCount; ++i)
//	{
//		// we already created the buffer
//		u8* pInstData = m_instanceBuffers[i]->instanceDataBuffer(false);
//
//		if (pInstData && pData)
//		{
//			memcpy(pInstData, pData, m_instanceBuffers[i]->instanceDataBufferSize());
//		}
//
//		m_instanceBuffers[i]->uploadInstanceDataBuffer(aStartInstanceIndex, aInstanceCount);
//	}
//}
//
//const Array<GeometryInstanceBuffer*>& ModelInstancer::instanceBuffers() const
//{
//	return m_instanceBuffers;
//}

}