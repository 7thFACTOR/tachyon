#include <string.h>
#include "input/mouse.h"

namespace engine
{
Mouse::Mouse()
{
	type = InputDeviceType::Mouse;
	memset(buttons, 0, sizeof(buttons));
}

}