#include "opengl_globals.h"
#ifdef BASE_PLATFORM_WINDOWS
#include <windows.h>
#endif
#include "opengl_gpu_buffer.h"
#include "base/logger.h"
#include "render/opengl/opengl_gpu_program.h"
#include "base/util.h"
#include "render/texcoord.h"
#include "opengl_graphics.h"

namespace engine
{
OpenGLGpuBuffer::OpenGLGpuBuffer()
	: GpuBuffer()
{
	oglBufferId = -1;
	mappedOffset = 0;
	mappedDataSize = 0;
}

OpenGLGpuBuffer::~OpenGLGpuBuffer()
{
	if (mapped)
	{
		unmap();
	}

	free();
}

void OpenGLGpuBuffer::initialize(u32 size, void* data)
{
	glGenBuffers(1, &oglBufferId);
	CHECK_OPENGL_ERROR;
	
	if (size)
	{
		resize(size, data);
	}
}

void* OpenGLGpuBuffer::map(u32 offset, u32 size, GpuBufferMapType mapType)
{
	B_ASSERT(!mapped);
	glBindBuffer(oglGpuBufferType[(int)type], oglBufferId);
	CHECK_OPENGL_ERROR;
	GLenum oglMapType = openglTranslateEnum(mapType);

	if (offset == 0)
	{
	#ifndef GLES
		mappedData = glMapBuffer(oglGpuBufferType[(int)type], oglMapType);
	#else
		mappedData = glMapBufferOES(oglGpuBufferType[(int)type], oglMapType);
	#endif
		CHECK_OPENGL_ERROR;
	}
	else
	{
		u32 mapTypeRange = 0;

		if (mapType == GpuBufferMapType::Write)
			mapTypeRange = GL_MAP_WRITE_BIT;
		else if (mapType == GpuBufferMapType::Read)
			mapTypeRange = GL_MAP_READ_BIT;
		else if (mapType == GpuBufferMapType::ReadWrite)
			mapTypeRange = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;

	#ifndef GLES
		if (glMapBufferRange)
		{
			mappedData = glMapBufferRange(oglGpuBufferType[(int)type], offset, size, mapTypeRange);
			CHECK_OPENGL_ERROR;
		}
		else
		{
			oglMappedRangeBuffer.resize(bufferSize);
			mappedOffset = offset;
			mappedDataSize = size;
			mappedData = (void*)(&oglMappedRangeBuffer[0] + mappedOffset);
		}
	#else
		//TODO: no range in OES?
		mappedData = glMapBufferOES(oglGpuBufferType[(int)type], offset, size, mapTypeRange);
	#endif
		CHECK_OPENGL_ERROR;
	}

	B_ASSERT(mappedData);
	mapped = true;

	return mappedData;
}

void OpenGLGpuBuffer::unmap()
{
	B_ASSERT(mapped);
	glBindBuffer(oglGpuBufferType[(int)type], oglBufferId);
	CHECK_OPENGL_ERROR;
	bool ok = true;

#ifndef GLES
	if (mappedOffset && mappedData)
	{
		B_ASSERT(glBufferSubData);
		glBufferSubData(oglGpuBufferType[(int)type], mappedOffset, mappedDataSize, (u8*)mappedData);
		CHECK_OPENGL_ERROR;
		mappedOffset = 0;
		mappedDataSize = 0;
	}
	else
	{
		ok = glUnmapBuffer(oglGpuBufferType[(int)type]) != 0;
	}
#else
	ok = glUnmapBufferOES(oglGpuBufferType[(int)type]) != 0;
#endif
	CHECK_OPENGL_ERROR;
	
	mapped = false;
	mappedData = nullptr;
	B_ASSERT(ok);
}

void OpenGLGpuBuffer::bind()
{
	glBindBuffer(oglGpuBufferType[(int)type], oglBufferId);
	CHECK_OPENGL_ERROR;
}

void OpenGLGpuBuffer::unbind()
{
	glBindBuffer(oglGpuBufferType[(int)type], 0);
	CHECK_OPENGL_ERROR;
}

void OpenGLGpuBuffer::resize(u32 size, void* data)
{
	if (mapped)
	{
		unmap();
	}

	GLenum bufType = oglGpuBufferType[(int)type];
	
	glBindBuffer(bufType, oglBufferId);
	CHECK_OPENGL_ERROR;
	glBufferData(bufType, size, data, usage == GpuBufferUsageType::Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	CHECK_OPENGL_ERROR;
	bufferSize = size;
}

void OpenGLGpuBuffer::setupVertexDeclarationGL(OpenGLGpuProgram& program, u32 offset)
{
	#define OGL_VBUFFER_OFFSET(i) ((void*)(i))

	glBindBuffer(oglGpuBufferType[(int)type], oglBufferId);
	CHECK_OPENGL_ERROR;
	u32 stride = desc.getStride();
	
	for (size_t i = 0; i < desc.getElements().size(); ++i)
	{
		auto elem = desc.getElements()[i];
		OpenGLVertexAttribute* attr = program.findAttribute(elem.type, elem.index);
		
		if (!attr)
		{
			continue;
		}

		u32 offsetSum = (offset * stride) + elem.offset;

		glEnableVertexAttribArray(attr->location);
		CHECK_OPENGL_ERROR;
		glVertexAttribPointer(
			attr->location, elem.componentCount, openglTranslateEnum(elem.componentType), GL_FALSE,
			stride, OGL_VBUFFER_OFFSET(offsetSum));
		CHECK_OPENGL_ERROR;
		
		if (glVertexAttribDivisor)
			glVertexAttribDivisor(attr->location, 0);

		CHECK_OPENGL_ERROR;
	}
}

void OpenGLGpuBuffer::free()
{
	if (oglBufferId != -1)
	{
		glDeleteBuffers(1, &oglBufferId);
		CHECK_OPENGL_ERROR;
		oglBufferId = -1;
	}
}

#ifndef NDEBUG
void OpenGLGpuBuffer::debug()
{
	u8* data = (u8*)map(0, 0, GpuBufferMapType::Read);

	B_ASSERT(data);

	if (type == GpuBufferType::Indices)
	{
		B_LOG_DEBUG("Index buffer:");
	}
	else if (type == GpuBufferType::Vertices)
	{
		B_LOG_DEBUG("Vertex buffer:");
	}

	for (size_t i = 0; i < elementCount; i++)
	{
		for (auto& el : desc.getElements())
		{
			switch (el.type)
			{
			case GpuBufferElementType::Index:
				B_LOG_DEBUG("Index: " << *(VertexIndexType*)data);
				break;
			case GpuBufferElementType::Position:
				B_LOG_DEBUG("Pos: " << ((Vec3*)data)->x << "," << ((Vec3*)data)->y << "," << ((Vec3*)data)->z);
				break;
			case GpuBufferElementType::Normal:
				B_LOG_DEBUG("Nrm: " << ((Vec3*)data)->x << "," << ((Vec3*)data)->y << "," << ((Vec3*)data)->z);
				break;
			case GpuBufferElementType::Tangent:
				B_LOG_DEBUG("Tan: " << ((Vec3*)data)->x << "," << ((Vec3*)data)->y << "," << ((Vec3*)data)->z);
				break;
			case GpuBufferElementType::Bitangent:
				B_LOG_DEBUG("Bit: " << "," << ((Vec3*)data)->x << "," << ((Vec3*)data)->y << "," << ((Vec3*)data)->z);
				break;
			case GpuBufferElementType::Color:
				B_LOG_DEBUG("Col: " << ((Color*)data)->r << "," << ((Color*)data)->g << "," << ((Color*)data)->b << "," << ((Color*)data)->a);
				break;
			case GpuBufferElementType::TexCoord:
				B_LOG_DEBUG("UV: " << ((TexCoord*)data)->x << "," << ((TexCoord*)data)->y);
				break;
			default:
				break;
			}

			data += el.getElementSize();
		}
	}

	unmap();
}
#endif

}