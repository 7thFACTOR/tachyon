#include <string.h>
#include "input/keyboard.h"

namespace engine
{
Keyboard::Keyboard()
{
	type = InputDeviceType::Keyboard;
	memset(keyStates, 0, sizeof(keyStates));
}

bool Keyboard::isKeyDown(InputKey key) const
{
	return keyStates[(int)key] == 1;
}

bool Keyboard::isKeyUp(InputKey key) const
{
	return keyStates[(int)key] == 0;
}

void Keyboard::setKeyState(InputKey key, u8 state)
{
	keyStates[(int)key] = state;
}

const u8* Keyboard::getKeyStatesBuffer() const
{
	return keyStates;
}

u32 Keyboard::getKeyStatesBufferSize() const
{
	return sizeof(keyStates[0]) * (int)InputKey::Count;
}

void Keyboard::clearKeyStatesBuffer()
{
	memset(keyStates, 0, sizeof(keyStates));
}

}