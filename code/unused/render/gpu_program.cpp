#include "render/gpu_program.h"
#include "render/gpu_program_render_method.h"

namespace engine
{
GpuProgram::GpuProgram()
{}

GpuProgram::~GpuProgram()
{}

bool GpuProgram::upload()
{
	return false;
}

void GpuProgram::bind()
{}

void GpuProgram::unbind()
{}

void GpuProgram::commitConstants()
{}

const Array<GpuProgramRenderMethod*>& GpuProgram::getRenderMethods() const
{
	return renderMethods;
}

bool GpuProgram::setCurrentRenderMethod(const String& name)
{
	for (size_t i = 0, iCount = renderMethods.size(); i < iCount; ++i)
	{
		if (renderMethods[i]->getName() == name)
		{
			currentRenderMethod = renderMethods[i];
			return true;
		}
	}

	return false;
}

void GpuProgram::setCurrentRenderMethod(GpuProgramRenderMethod* renderMethod)
{
	currentRenderMethod = renderMethod;
}

GpuProgramRenderMethod* GpuProgram::getCurrentRenderMethod() const
{
	return currentRenderMethod;
}

GpuProgramRenderMethod* GpuProgram::getRenderMethod(const String& name) const
{
	for (size_t i = 0, iCount = renderMethods.size(); i < iCount; ++i)
	{
		if (renderMethods[i]->getName() == name)
		{
			return renderMethods[i];
		}
	}

	return nullptr;
}

}