#include "render/gpu_program_controller.h"
#include "base/assert.h"
#include "base/string.h"

namespace engine
{
using namespace base;

GpuProgramController::GpuProgramController()
{
}

GpuProgramController::~GpuProgramController()
{
}

bool GpuProgramController::control(GpuProgram* pProgram)
{
	B_ASSERT(pProgram);

	return true;
}
}