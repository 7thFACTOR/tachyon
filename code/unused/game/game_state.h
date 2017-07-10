#pragma once

namespace engine
{
struct GameState
{
	bool gameplayActive = false;
	GameState state = GameState::Intro;
	GameState previousGameState = GameState::Intro;
};

}