#pragma once
#include "base/string.h"
#include "base/array.h"
#include "base/map.h"
#include "base/defines.h"
#include "base/types.h"
#include "game/component.h"
#include "game/types.h"

namespace engine
{
using namespace base;
class SoundSource;

struct AmbientMusicComponent : Component
{
	enum
	{
		Type = ComponentType::AmbientMusic,
		DefaultCount = kMaxComponentCountPerType
	};

	Map<String, SoundSource*> gameSoundTracks;
	f32 gameSoundTrackCrossFadeTimer, gameSoundTrackCrossFadeSpeed;
	bool bIsCrossFadingGameSoundTrack;
	String currentGameSoundTrack, nextGameSoundTrack;

	void update(f32 deltaTime);
	//! add and set a new named game sound track
	//! \param pName the symbolic name of the sound track 
	//! ( e.g.: "BattleStarting", "Battle", "Normal" or other user-defined names ),
	//! this name will be later used to handle the soundtrack
	//! \param pObject the sound source object to use when playing the soundtrack 
	//! ( you must create it and set the sound file for the track )
	void setGameSoundTrack(const String& name, SoundSource* pObject);
	//! change the current game music
	//! \param the name of the music
	void changeGameSoundTrack(const String& name);
	//! stop the current playing sound track, immediately or with fading
	//! \param bFadeOut fade until silence
	void stopGameSoundTrack(bool bFadeOut = true);
	void removeGameSoundTrack(const String& name);
	void removeAllGameSoundTracks();
};

}