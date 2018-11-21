// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "opengl_globals.h"
#include "graphics/opengl/opengl_gpu_program.h"
#include "graphics/opengl/opengl_texture.h"
#include "core/globals.h"
#include "graphics/graphics.h"
#include "graphics/texture.h"
#include "base/logger.h"
#include "base/timer.h"
#include "core/resource_repository.h"
#include "base/util.h"
#include "base/profiler.h"

namespace engine
{
OpenglGpuProgram::OpenglGpuProgram()
{}

OpenglGpuProgram::~OpenglGpuProgram()
{
	freeGL();
}

void OpenglGpuProgram::setMatrixUniform(const String& constName, const Matrix& mtx)
{
	GLint loc = glGetUniformLocation(oglProgram, constName.c_str());
	CHECK_OPENGL_ERROR;

	if (loc != -1)
	{
		glUniformMatrix4fv(loc, 1, GL_TRUE, (const f32*)&mtx.m[0]);
		CHECK_OPENGL_ERROR;
	}
}

void OpenglGpuProgram::setMatrixArrayUniform(const String& constName, const Matrix* matrices, u32 count)
{
	GLint loc = glGetUniformLocation(oglProgram, constName.c_str());
	CHECK_OPENGL_ERROR;

	if (loc != -1)
	{
		glUniformMatrix4fv(loc, count, GL_TRUE, (const f32*)&matrices->m[0]);
		CHECK_OPENGL_ERROR;
	}
}

void OpenglGpuProgram::setIntUniform(const String& constName, i32 value)
{
	GLint loc = glGetUniformLocation(oglProgram, constName.c_str());
	CHECK_OPENGL_ERROR;

	if (loc != -1)
	{
		glUniform1i(loc, value);
		CHECK_OPENGL_ERROR;
	}
}

void OpenglGpuProgram::setIntArrayUniform(const String& constName, i32* intValues, u32 count)
{
	GLint loc = glGetUniformLocation(oglProgram, constName.c_str());
	CHECK_OPENGL_ERROR;

	if (loc != -1)
	{
		switch (count)
		{
		case 2:
			{
				glUniform2iv(loc, 1, intValues);
				break;
			}
		case 3:
			{
				glUniform3iv(loc, 1, intValues);
				break;
			}
		case 4:
			{
				glUniform4iv(loc, 1, intValues);
				break;
			}
		default:
			{
				glUniform1iv(loc, count, intValues);
				break;
			}
		};
		CHECK_OPENGL_ERROR;
	}
}

void OpenglGpuProgram::setFloatUniform(const String& constName, f32 value)
{
	GLint loc = glGetUniformLocation(oglProgram, constName.c_str());
	CHECK_OPENGL_ERROR;

	if (loc != -1)
	{
		glUniform1f(loc, value);
		CHECK_OPENGL_ERROR;
	}
}

void OpenglGpuProgram::setFloatArrayUniform(const String& constName, f32* floatValues, u32 count)
{
	GLint loc = glGetUniformLocation(oglProgram, constName.c_str());
	CHECK_OPENGL_ERROR;

	if (loc != -1)
	{
		switch (count)
		{
		case 2:
			{
				glUniform2fv(loc, 1, floatValues);
				break;
			}
		case 3:
			{
				glUniform3fv(loc, 1, floatValues);
				break;
			}
		case 4:
			{
				glUniform4fv(loc, 1, floatValues);
				break;
			}
		default:
			{
				break;
			}
		};
		CHECK_OPENGL_ERROR;
	}
}

void OpenglGpuProgram::setVec2Uniform(const String& constName, const Vec2& val)
{
	GLint loc = glGetUniformLocation(oglProgram, constName.c_str());
	CHECK_OPENGL_ERROR;
	glUniform2fv(loc, 1, &val.x);
	CHECK_OPENGL_ERROR;
}

void OpenglGpuProgram::setColorUniform(const String& constName, const Color& val)
{
	GLint loc = glGetUniformLocation(oglProgram, constName.c_str());
	CHECK_OPENGL_ERROR;
	glUniform4fv(loc, 1, &val.r);
	CHECK_OPENGL_ERROR;
}

void OpenglGpuProgram::setVec3Uniform(const String& constName, const Vec3& val)
{
	GLint loc = glGetUniformLocation(oglProgram, constName.c_str());
	CHECK_OPENGL_ERROR;
	glUniform3fv(loc, 1, &val.x);
	CHECK_OPENGL_ERROR;
}

void OpenglGpuProgram::setSamplerUniform(Texture* tex, const String& constName, i32 stage)
{
	if (!tex)
	{
		return;
	}

	OpenglTexture* oglTex = (OpenglTexture*)tex;

	GLint loc = glGetUniformLocation(oglProgram, constName.c_str());
	CHECK_OPENGL_ERROR;

	glActiveTexture(GL_TEXTURE0 + stage);
	CHECK_OPENGL_ERROR;
	glBindTexture(oglTex->oglTexType, oglTex->oglTexId);
	CHECK_OPENGL_ERROR;

	if (loc != -1)
	{
		glUniform1i(loc, stage);
		CHECK_OPENGL_ERROR;
	}
}

void OpenglGpuProgram::bind()
{
	glUseProgram(oglProgram);
	CHECK_OPENGL_ERROR;
}

void OpenglGpuProgram::unbind()
{
	glUseProgram(0);
	CHECK_OPENGL_ERROR;
}

void OpenglGpuProgram::setFromResource(ResourceId resId)
{
	ScopedResourceMapping<GpuProgramResource> program(resId);

	if (!program)
	{
		return;
	}

	freeGL();

	resourceId = resId;
	oglProgram = glCreateProgram();
	CHECK_OPENGL_ERROR;

	if (!program->shaderSource[(u32)ShaderType::Pixel].isEmpty())
	{
		oglPixelShader = glCreateShader(GL_FRAGMENT_SHADER);
		CHECK_OPENGL_ERROR;
		const char* srcCode = program->shaderSource[(u32)ShaderType::Pixel].c_str();
		glShaderSource(oglPixelShader, 1, &srcCode, nullptr);
		CHECK_OPENGL_ERROR;
		glCompileShader(oglPixelShader);
		//TODO: add glGetShaderInfoLog here too
		CHECK_OPENGL_ERROR;
		glAttachShader(oglProgram, oglPixelShader);
		CHECK_OPENGL_ERROR;
	}
		
	if (!program->shaderSource[(u32)ShaderType::Vertex].isEmpty())
	{
		oglVertexShader = glCreateShader(GL_VERTEX_SHADER);
		CHECK_OPENGL_ERROR;
		const char* srcCode = program->shaderSource[(u32)ShaderType::Vertex].c_str();
		glShaderSource(oglVertexShader, 1, &srcCode, nullptr);
		CHECK_OPENGL_ERROR;
		glCompileShader(oglVertexShader);
		//TODO: add glGetShaderInfoLog here too
		CHECK_OPENGL_ERROR;
		glAttachShader(oglProgram, oglVertexShader);
		CHECK_OPENGL_ERROR;
	}
		
	if (!program->shaderSource[(u32)ShaderType::Geometry].isEmpty())
	{
		oglGeometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		CHECK_OPENGL_ERROR;
		const char* srcCode = program->shaderSource[(u32)ShaderType::Geometry].c_str();
		glShaderSource(oglGeometryShader, 1, &srcCode, nullptr);
		CHECK_OPENGL_ERROR;
		glCompileShader(oglGeometryShader);
		//TODO: add glGetShaderInfoLog here too
		CHECK_OPENGL_ERROR;
		glAttachShader(oglProgram, oglGeometryShader);
		CHECK_OPENGL_ERROR;
	}
		
	if (!program->shaderSource[(u32)ShaderType::Hull].isEmpty())
	{
		oglTesselationControlShader = glCreateShader(GL_TESS_CONTROL_SHADER);
		CHECK_OPENGL_ERROR;
		const char* srcCode = program->shaderSource[(u32)ShaderType::Hull].c_str();
		glShaderSource(oglTesselationControlShader, 1, &srcCode, nullptr);
		CHECK_OPENGL_ERROR;
		glCompileShader(oglTesselationControlShader);
		//TODO: add glGetShaderInfoLog here too
		CHECK_OPENGL_ERROR;
		glAttachShader(oglProgram, oglTesselationControlShader);
		CHECK_OPENGL_ERROR;
	}
		
	if (!program->shaderSource[(u32)ShaderType::Domain].isEmpty())
	{
		oglTesselationEvaluationShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
		CHECK_OPENGL_ERROR;
		const char* srcCode = program->shaderSource[(u32)ShaderType::Domain].c_str();
		glShaderSource(oglTesselationEvaluationShader, 1, &srcCode, nullptr);
		CHECK_OPENGL_ERROR;
		glCompileShader(oglTesselationEvaluationShader);
		//TODO: add glGetShaderInfoLog here too
		CHECK_OPENGL_ERROR;
		glAttachShader(oglProgram, oglTesselationEvaluationShader);
		CHECK_OPENGL_ERROR;
	}

	B_BEGIN_PROFILE_SECTION(getProfiler(), UploadProgramShaders);
	B_LOG_INFO("Linking program " << oglProgram);
	glLinkProgram(oglProgram);
	B_LOG_INFO("Linking program done " << oglProgram);
	CHECK_OPENGL_ERROR;

	{
		GLchar errorLog[1024] = {0};
		glGetProgramInfoLog(oglProgram, 1024, NULL, errorLog);
		CHECK_OPENGL_ERROR;

		if (0 != strcmp(errorLog, ""))
		{
			B_LOG_INFO("Linking program: " << errorLog);
		}
	}

	if (!glIsProgram(oglProgram))
	{
		B_LOG_ERROR("Program ID: " << oglProgram << " is not a valid OpenGL program");
	}
	CHECK_OPENGL_ERROR;


	B_LOG_DEBUG("Use program " << oglProgram);
	glUseProgram(oglProgram);
	CHECK_OPENGL_ERROR;
	fillAttributesGL();

	B_LOG_DEBUG("Validate program " << oglProgram);
	glValidateProgram(oglProgram);
	CHECK_OPENGL_ERROR;

	GLint success = GL_FALSE;
	glGetProgramiv(oglProgram, GL_VALIDATE_STATUS, &success);
	CHECK_OPENGL_ERROR;

	if (success == GL_FALSE)
	{
		GLchar errorLog[1024] = { 0 };
		glGetProgramInfoLog(oglProgram, 1024, NULL, errorLog);
		CHECK_OPENGL_ERROR;
		B_LOG_ERROR("Error validating program: " << errorLog);
		return;
	}
}

static GpuBufferElementType findComponentBySemantic(const String& attrName, i32& index)
{
	index = 0;

	if (attrName == "POSITION" || attrName == "cg_Vertex")
	{
		return GpuBufferElementType::Position;
	}
	else if (attrName == "NORMAL")
	{
		return GpuBufferElementType::Normal;
	}
	else if (attrName == "TANGENT")
	{
		return GpuBufferElementType::Tangent;
	}
	else if (attrName == "BINORMAL")
	{
		return GpuBufferElementType::Bitangent;
	}
	else if (attrName == "COLOR")
	{
		return GpuBufferElementType::Color;
	}
	else if (attrName.find("TEXCOORD") != String::noIndex)
	{
		index = atoi(attrName.subString(attrName.length() - 1, 1).c_str());
		return GpuBufferElementType::TexCoord;
	}

	return GpuBufferElementType::Unknown;
}

void OpenglGpuProgram::fillAttributesGL()
{
	GLint attrib_count, max_name_len = 0;
	
	glGetProgramiv(oglProgram, GL_ACTIVE_ATTRIBUTES, &attrib_count);
	glGetProgramiv(oglProgram, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_name_len);

	char* attrName = new char[max_name_len];

	for (i32 l = 0; l < attrib_count; ++l)
	{
		GLsizei attrNameLen = 0;
		i32 attrSize = 0;
		GLenum attrGLType = 0;
		i32 index = 0;
		i32 location = 0;

		glGetActiveAttrib(oglProgram, l, max_name_len, &attrNameLen, &attrSize, &attrGLType, attrName);
		location = glGetAttribLocation(oglProgram, attrName);

		if (location == -1)
		{
			B_LOG_DEBUG("Skipped attrib '" << attrName << "'");
			continue;
		}

		OpenGLVertexAttribute atr;
		
		atr.name = attrName;
		atr.semantic = findComponentBySemantic(attrName, index);
		atr.location = location;
		atr.index = index;
		oglAttributes.append(atr);
		B_LOG_DEBUG("Attrib '" << attrName << "' location:" << location << ", as " << (u32)findComponentBySemantic(attrName, index) << ", index " << index);
	}
}

void OpenglGpuProgram::freeGL()
{
	glDeleteShader(oglPixelShader);
	glDeleteShader(oglVertexShader);
	glDeleteShader(oglGeometryShader);
	glDeleteShader(oglTesselationControlShader);
	glDeleteShader(oglTesselationEvaluationShader);
	glDeleteProgram(oglProgram);
}

}