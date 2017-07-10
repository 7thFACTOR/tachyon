#pragma once
#include "common.h"
#include <iostream>
#include <al/al.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisenc.h>
#include <vorbis/vorbisfile.h>

class OpenAlProvider;

struct OggFileInfo
{
	char*   dataPtr;
	int     dataSize;
	int     dataRead;
};

class NYTRO_API OpenAlSoundBufferData : public SoundBufferData
{
public:

	OpenAlSoundBufferData();
	~OpenAlSoundBufferData();

	Result          load();
	Result          unload();

	ALuint          m_soundBuffer, m_oggBuffers[2];
	ov_callbacks    vorbisCallbacks;
	OggVorbis_File  oggStream;
	vorbis_info*    vorbisInfo;
	vorbis_comment* vorbisComment;
	OggFileInfo     oggMemoryFile;
	int             m_oggFormat;
	int             m_length;
	int             m_frequency;
	int             m_bIsOGG;
};

class NYTRO_API OpenAlSoundSourceNode : public SoundSourceNode
{
public:

	OpenAlSoundSourceNode();
	~OpenAlSoundSourceNode();

	void computeTransform();

	void            play();
	void            stop();

	float           getVolume();
	int             getTime();
	int             getLength();
	int             getFrequency();
	void            getVolumeLevels(float& rLeft, float& rRight);
	int             getPan();
	float           getPitch();
	bool            isMuted();
	bool            isPaused();
	bool            isSurround();
	bool            isPlaying();
	Vector3D&       getVelocity();

	void            setVolume(float aValue);
	void            setTime(int aValue);
	void            setFrequency(int aValue);
	void            setMuted(bool aValue);
	void            setPan(int aValue);
	void            setPaused(bool aValue);
	void            setSurround(bool aValue);
	void            setLoopMode(int aValue);
	void            setChannel(int aValue);
	void            setVelocity(Vector3D& rVel);
	void            setPitch(float aValue);
	void            setFromResource(Resource* pRes);

	ALuint          m_soundSource;
	int             m_oggStatus;
	bool            m_bCanBeDeleted, m_bExitOGGThread,
					m_bLoopOGG;
	int             m_looping;
	OpenAlProvider* m_pOALProvider;
	float           m_volume, m_pitch;
};

#endif