#pragma once
#include "base/defines.h"
#include "core/defines.h"
#include "base/types.h"
#include "base/class_registry.h"

namespace engine
{
class GpuProgram;
class RenderLayer;

//! A gpu program controller will control a gpu program, for example there can be
//! an ocean gpu program controller who controls the tide, water rendering, etc. by setting constants
class E_API GpuProgramController : public base::ClassInfo
{
public:
	GpuProgramController();
	virtual ~GpuProgramController();

	//! control the given program's properties, constants, etc.
	virtual bool control(GpuProgram* pProgram);
};
}