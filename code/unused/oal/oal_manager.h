#pragma once
#include "common.h"

#define OAL_MAX_SOUND_SOURCES 128

class OpenAlSoundSourceNode;

class NYTRO_API OpenAlProvider : public AudioProvider
{
public:

	OpenAlProvider();
	~OpenAlProvider();

	Result              initialize();
	Result              release();
	void*               newInstanceOf(int aInterfaceID, int aSubTypeID);

	void                getListener(SoundListener& rListener);
	float               getDistanceFactor();
	float               getDopplerFactor();
	float               getRolloffFactor();
	float               getCPUUsage();

	void                setFrequency(int32 aValue);
	void                setBufferSize(uint32 aValue);
	void                setMaxChannels(uint32 aValue);
	void                setListener(const SoundListener& rListener);
	void                setDistanceFactor(float aValue);
	void                setDopplerFactor(float aValue);
	void                setRolloffFactor(float aValue);
	void                setCurrentDriver(uint32 aIndex);
	void                setCurrentOutput(uint32 aIndex);
	void                update3DSystem();

	void                requestSourcePlay(OpenALSoundSourceNode* pNode);
	void                freeSource(int aSrc);
	void                freeSourceByHandle(ALuint aValue);
	int                 getFreeSourceSlot(OpenAlSoundSourceNode* pNode);

	vector<ALuint>                  m_sources;
	vector<OpenAlSoundSourceNode*>  m_usedSources;
	vector<OpenAlSoundSourceNode*>  m_sourcePlayRequesters;
};

#endif