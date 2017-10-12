// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "audio/audio.h"
#include "SFML/Audio/Listener.hpp"
#include "audio/types.h"

namespace engine
{
void Audio::setGlobalVolume(f32 volume)
{
	sf::Listener::setGlobalVolume(volume);
}

f32 Audio::getGlobalVolume()
{
	return sf::Listener::getGlobalVolume();
}

void Audio::setListenerPosition(const Vec3& position)
{
	sf::Listener::setPosition(position.x, position.y, position.z);
}

Vec3 Audio::getListenerPosition()
{
	auto pos = sf::Listener::getPosition();

	return { pos.x, pos.y, pos.z };
}

void Audio::setListenerDirection(const Vec3& direction)
{
	sf::Listener::setDirection(direction.x, direction.y, direction.z);
}

Vec3 Audio::getListenerDirection()
{
	auto dir = sf::Listener::getDirection();

	return { dir.x, dir.y, dir.z };
}

void Audio::setUpVector(const Vec3& up)
{
	sf::Listener::setUpVector(up.x, up.y, up.z);
}

Vec3 Audio::getUpVector()
{
	auto up = sf::Listener::getUpVector();

	return { up.x, up.y, up.z };
}

}