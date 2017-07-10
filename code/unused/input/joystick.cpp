#include <string.h>
#include "input/joystick.h"

namespace engine
{
Joystick::Joystick()
{
	type = InputDeviceType::Joystick;
	memset(buttons, 0, sizeof(buttons));
}

}