// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include <string.h>
#include "logic/component_systems/render/render_component_system.h"
#include "core/globals.h"
#include "logic/logic.h"
#include "core/resources/gpu_program_resource.h"
#include "core/resources/material_resource.h"
#include "base/logger.h"
#include "base/profiler.h"
#include "base/qsort.h"
#include "base/variable.h"
#include "logic/component.h"
#include "graphics/graphics.h"
#include "base/math/conversion.h"
#include "base/array.h"
#include "logic/component_pool.h"
#include "logic/types.h"
#include "base/util.h"
#include "core/resource_repository.h"
#include "graphics/shape_renderer.h"
#include "graphics/mesh.h"
#include "graphics/gpu_program.h"
#include "graphics/gpu_buffer.h"
#include "logic/entity.h"

#include "logic/components/render/camera.h"
#include "logic/components/render/transform.h"
#include "logic/components/render/mesh.h"
#include "logic/components/render/mesh_renderer.h"

namespace engine
{
RenderComponentSystem::RenderComponentSystem()
{
}

RenderComponentSystem::~RenderComponentSystem()
{}

void RenderComponentSystem::updateLocalTransform(TransformComponent& xform)
{
	Matrix mtxRotation, mtxRoll;

	if (!xform.lookAt)
	{
		if (!!(xform.flags & TransformFlags::UseLookDirection))
		{
			mtxRotation.lookTowards(xform.lookDirection, xform.upAxis);
		}
		else
		{
			toMatrix(xform.rotation, mtxRotation);
		}
	}
	else
	{
		BigVec3 wdir = xform.translation - xform.lookAt->translation;

		mtxRotation.lookTowards(Vec3(wdir.x, wdir.y, wdir.z), xform.upAxis);
	}

	if (!!(xform.flags & TransformFlags::UseRollAngle))
	{
		mtxRoll = mtxRotation;
		mtxRotation.setIdentity();
		mtxRotation.setRotationZ(xform.roll);
		mtxRotation.multiply(mtxRoll);
	}

	Matrix mtxTrans, mtxScale;

	mtxTrans.setTranslation(xform.translation.x, xform.translation.y, xform.translation.z);
	mtxScale.setScale(xform.scale.x, xform.scale.y, xform.scale.z);
	xform.localMatrix = mtxScale * mtxRotation * mtxTrans;
}

void RenderComponentSystem::updateWorldTransform(TransformComponent& xform)
{
	if (xform.parent)
	{
		updateLocalTransform(*xform.parent);
		xform.worldMatrix = (xform.localMatrix * xform.parent->worldMatrix);
	}
	else
	{
		xform.worldMatrix = xform.localMatrix;
	}
}

void RenderComponentSystem::update(f32 deltaTime)
{
	if (!shapeRenderer)
	{
		shapeRenderer = new ShapeRenderer();
	}

	getGraphics().resetViewport();
	getGraphics().clear(ClearBufferFlags::All, Color::skyBlue);
	getGraphics().beginDraw();

	B_CALL_OBSERVERS_METHOD(onBeforeWorldRender());

	// update the dirty components computed on the threads to the main component pool line
	auto xforms = getLogic().getComponentPool<TransformComponent>();

	xforms->clearDirtyFlag();

	for (size_t i = 0; i < xforms->getComponentCount(); i++)
	{
		TransformComponent* xform = xforms->getComponentAs<TransformComponent>(i);
		
		updateLocalTransform(*xform);
		updateWorldTransform(*xform);
	}

	size_t entityCount = getLogic().getWorld().getEntities().size();
	auto& entities = getLogic().getWorld().getEntities();

	static CameraComponent* camera = nullptr;

	// find current camera
	// you should search once, only when added and removed and keep a list, optimization
	//TODO: render all cameras, also render the ones that have a render target to their own rt
	for (size_t i = 0; i < entityCount; i++)
	{
		auto entity = entities[i];

		if (!entity->active)
		{
			continue;
		}

		camera = entity->getComponent<CameraComponent>();

		if (camera)
		{
			break;
		}
	}

	if (!camera)
	{
		getGraphics().endDraw();
		getGraphics().present();
		return;
	}

	auto cameraXform = camera->entity->getComponent<TransformComponent>();

	if (cameraXform)
	{
		updateCamera(*camera);
		getGraphics().setProjectionMatrix(camera->projectionMatrix);
		getGraphics().setViewMatrix(camera->viewMatrix);
		getGraphics().setWorldMatrix(Matrix());
	}

	// render the meshes
	//TODO: mesh instancing support for more speed
	
	for (size_t i = 0; i < entityCount; i++)
	{
		auto entity = entities[i];

		if (!entity->active)
		{
			continue;
		}

		auto meshRenderer = entity->getComponent<MeshRendererComponent>();

		if (!meshRenderer)
		{
			continue;
		}

		auto mesh = entity->getComponent<MeshComponent>();
		auto xform = entity->getComponent<TransformComponent>();

		if (!mesh || !xform)
		{
			continue;
		}
				
		if (!mesh->renderMesh)
		{
			mesh->renderMesh = getGraphics().meshes[mesh->meshResourceId];
		}

		if (!mesh->renderMesh)
		{
			continue;
		}

		if (!mesh->meshResource)
		{
			mesh->meshResource = getResources().map<MeshResource>(mesh->meshResourceId);
			getResources().unmap(mesh->meshResourceId);
		}

		if (!mesh->meshResource)
		{
			continue;
		}

		getGraphics().setWorldMatrix(xform->worldMatrix);

		auto maxMtls = meshRenderer->materials.size();

		// render mesh elements (submeshes)
		for (size_t j = 0; j < mesh->meshResource->elements.size(); j++)
		{
			if (j < maxMtls)
			{
				GpuProgram* gpuProgram = setupMaterial(meshRenderer, j);

				if (!gpuProgram)
				{
					continue;
				}

				MeshElement& element = mesh->meshResource->elements[j];
						
				getGraphics().drawMesh(
					gpuProgram,
					mesh->renderMesh,
					nullptr,
					0,
					element.indexOffset,
					element.indexCount,
					element.indexCount);
			}
		}
	}

	B_CALL_OBSERVERS_METHOD(onAfterWorldRender());

	getGraphics().endDraw();
	getGraphics().present();
}

GpuProgram* RenderComponentSystem::setupMaterial(MeshRendererComponent* meshRenderer, u32 matIndex)
{
	ResourceId mat = meshRenderer->materials[matIndex];

	if (meshRenderer->materialResources.isEmpty())
	{
		meshRenderer->materialResources.resize(meshRenderer->materials.size());
		meshRenderer->materialResources.fill(0, meshRenderer->materials.size(), nullptr);
	}

	if (meshRenderer->materialGpuProgramResources.isEmpty())
	{
		meshRenderer->materialGpuProgramResources.resize(meshRenderer->materials.size());
		meshRenderer->materialGpuProgramResources.fill(0, meshRenderer->materials.size(), nullptr);
	}

	if (meshRenderer->materialGpuPrograms.isEmpty())
	{
		meshRenderer->materialGpuPrograms.resize(meshRenderer->materials.size());
		meshRenderer->materialGpuPrograms.fill(0, meshRenderer->materials.size(), nullptr);
	}

	if (!meshRenderer->materialResources[matIndex])
	{
		meshRenderer->materialResources[matIndex] = getResources().map<MaterialResource>(mat);
		getResources().unmap(mat);
	}

	if (!meshRenderer->materialResources[matIndex])
	{
		return nullptr;
	}

	auto gpuProg = meshRenderer->materialResources[matIndex]->gpuProgramId;
	
	if (!meshRenderer->materialGpuPrograms[matIndex])
	{
		meshRenderer->materialGpuPrograms[matIndex] = getGraphics().gpuPrograms[gpuProg];
	}

	if (!meshRenderer->materialGpuPrograms[matIndex])
	{
		return nullptr;
	}

	if (!meshRenderer->materialGpuProgramResources[matIndex])
	{
		meshRenderer->materialGpuProgramResources[matIndex] = getResources().map<GpuProgramResource>(meshRenderer->materialGpuPrograms[matIndex]->getResourceId());
		getResources().unmap(meshRenderer->materialGpuPrograms[matIndex]->getResourceId());
	}

	if (!meshRenderer->materialGpuProgramResources[matIndex])
	{
		return nullptr;
	}

	u32 texUnit = 0;

	auto mtlRes = meshRenderer->materialResources[matIndex];
	auto program = meshRenderer->materialGpuPrograms[matIndex];
	auto programRes = meshRenderer->materialGpuProgramResources[matIndex];
	
	//TODO: optimize
	getGraphics().blendState = mtlRes->blendState;
	getGraphics().depthStencilState = mtlRes->depthStencilState;
	getGraphics().rasterizerState = mtlRes->rasterizerState;
	getGraphics().commitBlendState();
	getGraphics().commitDepthStencilState();
	getGraphics().commitRasterizerState();
	program->bind();

	//TODO: don't set constants that are also set in the material
	for (size_t i = 0; i < programRes->constants.size(); i++)
	{
		GpuProgramConstantData& constant = programRes->constants[i];

		program->setConstant(constant, texUnit);
	}

	texUnit = 0;

	for (size_t i = 0; i < mtlRes->constants.size(); i++)
	{
		GpuProgramConstantData& constant = mtlRes->constants[i];
		program->setConstant(constant, texUnit);
	}

	return program;
}

void RenderComponentSystem::updateCamera(CameraComponent& camera)
{
	switch (camera.projectionType)
	{
	case CameraProjection::Perspective:
		camera.projectionMatrix.perspective(camera.fov, camera.aspectRatio, camera.nearPlane, camera.farPlane);
		break;
	case CameraProjection::Ortho:
		camera.projectionMatrix.setIdentity();
		break;
	case CameraProjection::Top:
		camera.projectionMatrix.ortho(-1, 1, -1, 1, -1, 0);
		//TODO
		break;
	case CameraProjection::Left:
		camera.projectionMatrix.ortho(-1, 1, -1, 1, -1, 0);
		//TODO
		break;
	case CameraProjection::Front:
		camera.projectionMatrix.ortho(-1, 1, -1, 1, -1, 0);
		//TODO
		break;
	default:
		break;
	}

	auto xform = (TransformComponent*)camera.entity->getComponent(StdComponentTypeId_Transform);

	camera.viewMatrix = xform->worldMatrix.getInverted();
}

}