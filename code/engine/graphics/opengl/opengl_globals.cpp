#include "opengl_globals.h"
#include "base/logger.h"
#include "base/util.h"

namespace engine
{
void openglCheckError(const String& where)
{
	GLuint err = glGetError();
	String str;

	switch (err)
	{
	case GL_INVALID_ENUM:
		str = "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.";
		break;
	case GL_INVALID_VALUE:
		str = "GL_INVALID_VALUE: A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.";
		break;
	case GL_INVALID_OPERATION:
		str = "GL_INVALID_OPERATION: The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.";
		break;
	case GL_STACK_OVERFLOW:
		str = "GL_STACK_OVERFLOW: This command would cause a stack overflow. The offending command is ignored and has no other side effect than to set the error flag.";
		break;
	case GL_STACK_UNDERFLOW:
		str = "GL_STACK_UNDERFLOW: This command would cause a stack underflow. The offending command is ignored and has no other side effect than to set the error flag.";
		break;
	case GL_OUT_OF_MEMORY:
		str = "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded.";
		break;
	case GL_TABLE_TOO_LARGE:
		str = "GL_TABLE_TOO_LARGE: The specified table exceeds the implementation's maximum supported table size. The offending command is ignored and has no other side effect than to set the error flag.";
		break;
	};

	if (err != GL_NO_ERROR)
	{
		B_LOG_ERROR("[" << where << "] OpenGL: code#" << err << ": " << str);
	}
}

GLenum openglTranslateEnum(GpuBufferMapType type)
{
	switch (type)
	{
	case GpuBufferMapType::Read: return GL_READ_ONLY; break;
	case GpuBufferMapType::Write: return GL_WRITE_ONLY; break;
	case GpuBufferMapType::ReadWrite: return GL_READ_WRITE; break;
	}

	return 0;
}

GLenum openglTranslateEnum(GpuBufferElementComponentType type)
{
	switch (type)
	{
	case GpuBufferElementComponentType::Float: return GL_FLOAT; break;
	case GpuBufferElementComponentType::Int8: return GL_UNSIGNED_BYTE; break;
	case GpuBufferElementComponentType::Int16: return GL_UNSIGNED_SHORT; break;
	case GpuBufferElementComponentType::Int32: return GL_UNSIGNED_INT; break;
	}

	return 0;
}

}