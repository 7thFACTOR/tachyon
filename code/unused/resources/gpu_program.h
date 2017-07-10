#pragma once
#include "base/array.h"
#include "base/string.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/rtti.h"
#include "core/resource.h"
#include "base/variant.h"
#include "render/gpu_program_constant.h"
#include "render/gpu_program_render_method.h"

namespace engine
{
class GpuProgram;
class RenderLayer;
class GpuProgramController;

enum EGpuProgramChunks
{
	eGpuProgramChunk_Header,
	eGpuProgramChunk_RenderMethods
};

//! A gpu program containing pixel/vertex/geom/domain/hull/compute shaders
class E_API GpuProgram : public Resource
{
public:
	B_RUNTIME_CLASS;
	GpuProgram();
	virtual ~GpuProgram();

	//! load the gpu program from a file
	virtual bool load(Stream* pStream);
	//! unload the gpu program
	virtual bool unload();
	//! begin rendering a vertex buffer with this program, with current active render method
	virtual void bind();
	virtual void commitConstants() = 0;
	//! end rendering with this gpu program
	virtual void unbind();
	//! \return number of render methods
	virtual const Array<GpuProgramRenderMethod*>& renderMethods() const;
	//! \return the gpu program's controller or nullptr if none set
	virtual GpuProgramController* controller() const;
	GpuProgramRenderMethod* currentRenderMethod() const;
	GpuProgramRenderMethod* renderMethod(const char* pRenderMethodName) const;

	//! set the active render method by its name
	virtual bool setCurrentRenderMethod(const char* pName);
	//! set the active render method by ptr
	virtual void setCurrentRenderMethod(GpuProgramRenderMethod* pRenderMethod);
	//! set the gpu program controller, you must delete the controller, it will not be deleted by the engine
	virtual void setController(GpuProgramController* pCtrl);

protected:
	Array<GpuProgramRenderMethod*> m_renderMethods;
	GpuProgramController* m_pController;
	GpuProgramRenderMethod* m_pCurrentRenderMethod;
};
}