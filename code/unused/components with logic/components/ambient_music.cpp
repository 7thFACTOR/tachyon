#include "game/components/ambient_music.h"
#include "audio/sound_source.h"

namespace engine
{
void AmbientMusicComponent::update(f32 deltaTime)
{
	if (!gameSoundTracks.isEmpty())
	{
		// cross fade game sound track if timer is rolling
		if (bIsCrossFadingGameSoundTrack && gameSoundTrackCrossFadeTimer < 1.0f)
		{
			if (gameSoundTracks[currentGameSoundTrack])
			{
				if (!gameSoundTracks[currentGameSoundTrack]->isPlaying())
				{
					gameSoundTracks[currentGameSoundTrack]->playSound();
				}

				gameSoundTracks[currentGameSoundTrack]->setVolume(1.0f - gameSoundTrackCrossFadeTimer);

				if (gameSoundTracks[currentGameSoundTrack]->volume() <= 0.0f)
				{
					gameSoundTracks[currentGameSoundTrack]->pauseSound();
				}
			}

			if (gameSoundTracks[nextGameSoundTrack])
			{
				if (!gameSoundTracks[currentGameSoundTrack]->isPlaying())
				{
					gameSoundTracks[currentGameSoundTrack]->playSound();
				}

				gameSoundTracks[nextGameSoundTrack]->setVolume(gameSoundTrackCrossFadeTimer);

				if (gameSoundTracks[nextGameSoundTrack]->volume() <= 0.0f)
				{
					gameSoundTracks[nextGameSoundTrack]->pauseSound();
				}
			}

			gameSoundTrackCrossFadeTimer += deltaTime * gameSoundTrackCrossFadeSpeed;
		}
		else if (bIsCrossFadingGameSoundTrack && gameSoundTrackCrossFadeTimer > 1.0f)
		{
			bIsCrossFadingGameSoundTrack = false;
			gameSoundTrackCrossFadeTimer = 1.0f;
			currentGameSoundTrack = nextGameSoundTrack;
		}
	}
}

void AmbientMusicComponent::setGameSoundTrack(const String& name, SoundSource* pObject)
{
	gameSoundTracks[name] = pObject;
}

void AmbientMusicComponent::changeGameSoundTrack(const String& name)
{
	nextGameSoundTrack = name;
	gameSoundTrackCrossFadeTimer = 0;
	bIsCrossFadingGameSoundTrack = true;
}

void AmbientMusicComponent::stopGameSoundTrack(bool bFadeOut)
{
	nextGameSoundTrack = "";

	if (bFadeOut)
	{
		gameSoundTrackCrossFadeTimer = 0;
		bIsCrossFadingGameSoundTrack = true;
		changeGameSoundTrack("");
	}
	else
	{
		gameSoundTrackCrossFadeTimer = 0;
		bIsCrossFadingGameSoundTrack = false;

		if (gameSoundTracks[currentGameSoundTrack])
		{
			gameSoundTracks[currentGameSoundTrack]->pauseSound();
		}

		if (gameSoundTracks[nextGameSoundTrack])
		{
			gameSoundTracks[nextGameSoundTrack]->pauseSound();
		}
	}
}

void AmbientMusicComponent::removeGameSoundTrack(const String& name)
{
	auto iter = gameSoundTracks.find(name);

	if (iter != gameSoundTracks.end())
	{
		gameSoundTracks.erase(iter);
	}
}

void AmbientMusicComponent::removeAllGameSoundTracks()
{
	gameSoundTracks.clear();
}

}