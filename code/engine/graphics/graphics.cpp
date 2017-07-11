#include "base/math/conversion.h"
#include "base/logger.h"
#include "base/util.h"
#include "base/variable.h"
#include "base/assert.h"
#include "graphics/texcoord.h"
#include "graphics/shape_renderer.h"
#include "graphics/graphics.h"
#include "graphics/texture.h"
#include "graphics/gpu_program.h"
#include "graphics/gpu_buffer.h"
#include "base/timer.h"
#include "base/variable.h"
#include "input/window.h"
#include "core/resources/gpu_program_resource.h"
#include "core/globals.h"
#include "core/application.h"
#include "core/resource_repository.h"
#include "core/resources/mesh_resource.h"

namespace engine
{
using namespace base;

void Graphics::shutdown()
{
	for (auto item : gpuPrograms)
	{
		delete item.value;
	}

	for (auto item : textures)
	{
		delete item.value;
	}

	for (auto item : meshes)
	{
		delete item.value;
	}

	gpuPrograms.clear();
	textures.clear();
	meshes.clear();
}

u32 Graphics::getVideoAdapterIndex() const
{
	return videoAdapterIndex;
}

const Array<VideoAdapter>& Graphics::getVideoAdapters() const
{
	return videoAdapters;
}

const VideoMode& Graphics::getVideoMode() const
{
	return videoMode;
}

void Graphics::setWorldMatrix(const Matrix& mtx)
{
	worldMatrix = mtx;
	dirtyMatrixFlags |= DirtyMatrixFlags::WorldInverse;
	dirtyMatrixFlags |= DirtyMatrixFlags::WorldInverseTranspose;
	dirtyMatrixFlags |= DirtyMatrixFlags::WorldView;
	dirtyMatrixFlags |= DirtyMatrixFlags::WorldViewInverse;
	dirtyMatrixFlags |= DirtyMatrixFlags::WorldViewInverseTranspose;
	dirtyMatrixFlags |= DirtyMatrixFlags::WorldProjection;
	dirtyMatrixFlags |= DirtyMatrixFlags::WorldViewProjection;
}

void Graphics::setViewMatrix(const Matrix& mtx)
{
	viewMatrix = mtx;
	dirtyMatrixFlags |= DirtyMatrixFlags::ViewInverse;
	dirtyMatrixFlags |= DirtyMatrixFlags::ViewInverseTranspose;
	dirtyMatrixFlags |= DirtyMatrixFlags::ViewRotation;
	dirtyMatrixFlags |= DirtyMatrixFlags::WorldView;
	dirtyMatrixFlags |= DirtyMatrixFlags::WorldViewInverse;
	dirtyMatrixFlags |= DirtyMatrixFlags::WorldViewInverseTranspose;
	dirtyMatrixFlags |= DirtyMatrixFlags::ViewProjection;
	dirtyMatrixFlags |= DirtyMatrixFlags::WorldViewProjection;
}

void Graphics::setProjectionMatrix(const Matrix& mtx)
{
	projectionMatrix = mtx;
	dirtyMatrixFlags |= DirtyMatrixFlags::WorldProjection;
	dirtyMatrixFlags |= DirtyMatrixFlags::ViewProjection;
	dirtyMatrixFlags |= DirtyMatrixFlags::WorldViewProjection;
}

void Graphics::setTextureMatrix(const Matrix& mtx)
{
	textureMatrix = mtx;
}

const Matrix& Graphics::getWorldMatrix() const
{
	return worldMatrix;
}

const Matrix& Graphics::getViewMatrix() const
{
	return viewMatrix;
}

const Matrix& Graphics::getProjectionMatrix() const
{
	return projectionMatrix;
}

const Matrix& Graphics::getTextureMatrix() const
{
	return textureMatrix;
}


void Graphics::setBuiltInGpuProgramConstant(GpuProgram* program, GpuProgramConstantType type)
{
	//TODO: maybe use some table for these? or a ptr to func to set them, saving some time searching in the switch-case
	// GpuConstantUploader<GpuConstantType>
	switch (type)
	{
	case GpuProgramConstantType::WorldViewProjectionMatrix:
		if (!!(dirtyMatrixFlags & DirtyMatrixFlags::WorldViewProjection))
		{
			worldViewProjectionMatrix = worldMatrix;
			worldViewProjectionMatrix = worldViewProjectionMatrix * viewMatrix;
			worldViewProjectionMatrix = worldViewProjectionMatrix * projectionMatrix;
			dirtyMatrixFlags &= ~DirtyMatrixFlags::WorldViewProjection;
		}

		program->setMatrixUniform("mtxWorldViewProjection", worldViewProjectionMatrix);
		break;

	case GpuProgramConstantType::WorldMatrix:
		program->setMatrixUniform("mtxWorld", worldMatrix);
		break;

	case GpuProgramConstantType::ProjectionMatrix:
		program->setMatrixUniform("mtxProjection", projectionMatrix);
		break;

	case GpuProgramConstantType::WorldProjectionMatrix:
		if (!!(dirtyMatrixFlags & DirtyMatrixFlags::WorldProjection))
		{
			worldProjectionMatrix = worldMatrix;
			worldProjectionMatrix = worldProjectionMatrix * projectionMatrix;
			dirtyMatrixFlags &= ~DirtyMatrixFlags::WorldProjection;
		}

		program->setMatrixUniform("mtxWorldProjection", worldProjectionMatrix);
		break;

	case GpuProgramConstantType::ViewProjectionMatrix:
		if (!!(dirtyMatrixFlags & DirtyMatrixFlags::ViewProjection))
		{
			viewProjectionMatrix = viewMatrix;
			viewProjectionMatrix = viewProjectionMatrix * projectionMatrix;
			dirtyMatrixFlags &= ~DirtyMatrixFlags::ViewProjection;
		}

		program->setMatrixUniform("mtxViewProjection", viewProjectionMatrix);
		break;

	case GpuProgramConstantType::ViewMatrix:
		program->setMatrixUniform("mtxView", viewMatrix);
		break;

	case GpuProgramConstantType::WorldInverseMatrix:
		if (!!(dirtyMatrixFlags & DirtyMatrixFlags::WorldInverse))
		{
			worldInverseMatrix = worldMatrix;
			worldInverseMatrix.invert();
			dirtyMatrixFlags &= ~DirtyMatrixFlags::WorldInverse;
		}

		program->setMatrixUniform("mtxWorldInverse", worldInverseMatrix);
		break;

	case GpuProgramConstantType::WorldInverseTransposeMatrix:
		if (!!(dirtyMatrixFlags & DirtyMatrixFlags::WorldInverseTranspose))
		{
			worldInverseTransposeMatrix = worldMatrix;
			worldInverseTransposeMatrix.invert();
			worldInverseTransposeMatrix.transpose();
			dirtyMatrixFlags &= ~DirtyMatrixFlags::WorldInverseTranspose;
		}

		program->setMatrixUniform("mtxWorldInverseTranspose", worldInverseTransposeMatrix);
		break;

	case GpuProgramConstantType::ViewInverseMatrix:
		if (!!(dirtyMatrixFlags & DirtyMatrixFlags::ViewInverse))
		{
			viewInverseMatrix = viewMatrix;
			viewInverseMatrix.invert();
			dirtyMatrixFlags &= ~DirtyMatrixFlags::ViewInverse;
		}

		program->setMatrixUniform("mtxViewInverse", viewInverseMatrix);
		break;

	case GpuProgramConstantType::ViewInverseTransposeMatrix:
		if (!!(dirtyMatrixFlags & DirtyMatrixFlags::ViewInverseTranspose))
		{
			viewInverseTransposeMatrix = viewMatrix;
			viewInverseTransposeMatrix.invert();
			viewInverseTransposeMatrix.transpose();
			dirtyMatrixFlags &= ~DirtyMatrixFlags::ViewInverseTranspose;
		}

		program->setMatrixUniform("mtxViewInverseTranspose", viewInverseTransposeMatrix);
		break;

	case GpuProgramConstantType::ViewRotationMatrix:
		if (!!(dirtyMatrixFlags & DirtyMatrixFlags::ViewRotation))
		{
			viewRotationMatrix = viewMatrix.getRotationOnly();
			dirtyMatrixFlags &= ~DirtyMatrixFlags::ViewRotation;
		}

		program->setMatrixUniform("mtxViewRotation", viewRotationMatrix);
		break;

	case GpuProgramConstantType::WorldViewMatrix:
		if (!!(dirtyMatrixFlags & DirtyMatrixFlags::WorldView))
		{
			worldViewMatrix = worldMatrix;
			worldViewMatrix = worldViewMatrix * viewMatrix;
			dirtyMatrixFlags &= ~DirtyMatrixFlags::WorldView;
		}

		program->setMatrixUniform("mtxWorldView", worldViewMatrix);
		break;

	case GpuProgramConstantType::WorldViewInverseMatrix:
		if (!!(dirtyMatrixFlags & DirtyMatrixFlags::WorldViewInverse))
		{
			worldViewInverseMatrix = worldMatrix * viewMatrix;
			worldViewInverseMatrix.invert();
			dirtyMatrixFlags &= ~DirtyMatrixFlags::WorldViewInverse;
		}

		program->setMatrixUniform("mtxWorldViewInverse", worldViewInverseMatrix);
		break;

	case GpuProgramConstantType::WorldViewInverseTransposeMatrix:
		if (!!(dirtyMatrixFlags & DirtyMatrixFlags::WorldViewInverseTranspose))
		{
			worldViewInverseTransposeMatrix = worldMatrix;
			worldViewInverseTransposeMatrix = worldViewInverseTransposeMatrix * viewMatrix;
			worldViewInverseTransposeMatrix.invert();
			worldViewInverseTransposeMatrix.transpose();
			dirtyMatrixFlags &= ~DirtyMatrixFlags::WorldViewInverseTranspose;
		}

		program->setMatrixUniform("mtxWorldViewInverseTranspose", worldViewInverseTransposeMatrix);
		break;

	case GpuProgramConstantType::TextureMatrix:
		program->setMatrixUniform("mtxTexture", textureMatrix);
		break;

	case GpuProgramConstantType::EngineTime:
		program->setFloatUniform("time", (f32)getClock().getMainTimer().getTime());
		break;

	case GpuProgramConstantType::EngineFrameDeltaTime:
		program->setFloatUniform("frameDeltaTime", (f32)getClock().getTimer(SystemTimer::logic)->getDelta());
		break;

	case GpuProgramConstantType::UnitDomainRotatorTime:
		//TODO
		break;

	case GpuProgramConstantType::EngineFps:
		//TODO
		break;

	case GpuProgramConstantType::RenderTargetSize:
	{
		f32 rtSize[2] = { renderTarget[0]->getWidth(), renderTarget[0]->getHeight() };
		program->setFloatArrayUniform("renderTargetSize", rtSize, 2);
		break;
	}

	case GpuProgramConstantType::ViewportSize:
	{
		Rect vp = getViewport();
		f32 vpSize[2] = { vp.width, getViewport().height };
		program->setFloatArrayUniform("renderTargetSize", vpSize, 2);
		break;
	}

	case GpuProgramConstantType::CameraFov:
		//TODO
		break;

	case GpuProgramConstantType::CameraNearPlane:
		//TODO
		break;

	case GpuProgramConstantType::CameraFarPlane:
		//TODO
		break;

	case GpuProgramConstantType::CameraUpAxis:
		//TODO
		break;

	case GpuProgramConstantType::CameraPosition:
	{
		Vec3 camPos = viewMatrix.getTranslation();
		program->setFloatArrayUniform("cameraPosition", (f32*)&camPos, 3);
		break;
	}

	case GpuProgramConstantType::ColorTexture:
		//TODO
		break;

	case GpuProgramConstantType::DepthTexture:
		//TODO
		break;

	case GpuProgramConstantType::SystemTexture:
		//TODO
		break;

	default:
		B_ASSERT(!"No known built-in gpu program constant");
	}
}

void Graphics::update()
{
	Array<ResourceId> ids;

	// lets grab the fully loaded resources
	getResources().fetchLoadedResourceIds(ResourceType::GpuProgram, ids);

	for (auto resId : ids)
	{
		GpuProgram* program = createGpuProgram();

		gpuPrograms[resId] = program;
		program->setFromResource(resId);
	}

	// lets grab the fully loaded resources
	getResources().fetchLoadedResourceIds(ResourceType::Texture, ids);

	for (auto resId : ids)
	{
		Texture* tex = createTexture();
		textures[resId] = tex;
		tex->setFromResource(resId);
	}

	// lets grab the fully loaded resources
	getResources().fetchLoadedResourceIds(ResourceType::Mesh, ids);

	for (auto resId : ids)
	{
		Mesh* mesh = new Mesh();
		meshes[resId] = mesh;
		mesh->setFromResource(resId);
	}
}

}