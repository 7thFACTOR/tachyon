#include "fmod_common.h"
#include "base/logger.h"
#include "base/string.h"

namespace engine
{
using namespace base;

void checkFmodError(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		B_LOG_ERROR("Error: (" << result << ") " << FMOD_ErrorString(result) << "\n");
	}
}

}