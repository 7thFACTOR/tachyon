#include "common.h"

OpenALProvider::OpenALProvider()
{
	alutInit(0, NULL);
	alGetError();
	setListener(Vector3D(0, 0, 0), Vector3D(0, 0, 0),
				Vector3D(0, 0, -1), Vector3D(0, 1, 0));

	// Generate as many sources as possible (up to 64)

	for (int i = 0; i < OAL_MAX_SOUND_SOURCES; i++)
	{
		ALuint src, error;

		alGenSources(1, &src);

		if ((error = alGetError()) != AL_NO_ERROR)
		{
			break;
		}
		else
		{
			m_sources.push_back(src);
			m_usedSources.push_back(NULL);
		}
	}

	echo("OpenAL generated %d stock sound sources", m_sources.size());
}

void OpenALProvider::requestSourcePlay(OpenALSoundSourceNode* pNode)
{
	vector<OpenALSoundSourceNode*>::iterator it = m_sourcePlayRequesters.begin();

	while (it != m_sourcePlayRequesters.end())
	{
		if (*it == pNode)
			return;
		it++;
	}

	m_sourcePlayRequesters.push_back(pNode);
}

OpenALProvider::~OpenALProvider()
{
}

void* OpenALProvider::newInstanceOf(int aInterfaceID, int aSubTypeID)
{
	switch (aInterfaceID)
	{
	case INTERFACE_ID_SOUND_BUFFER_DATA :
	{
		OpenALSoundBufferData* pSndBuffer = new OpenALSoundBufferData;
		return pSndBuffer;
	}

	case INTERFACE_ID_SOUND_SOURCE_NODE :
	{
		OpenALSoundSourceNode* pSndSrc = new OpenALSoundSourceNode;
		pSndSrc->m_pOALProvider = this;
		return pSndSrc;
	}
	}

	return NULL;
}

Result OpenALProvider::initialize()
{
	return ALL_OK;
}

Result OpenALProvider::release()
{
	echo("Exiting OpenAL");
	alutExit();
	echo("Done.");
	return ALL_OK;
}

void OpenALProvider::getListener(Vector3D& rPos, Vector3D& rVel, Vector3D& rForward, Vector3D& rTop)
{
}

float OpenALProvider::getDistanceFactor()
{
	return 0;
}

float OpenALProvider::getDopplerFactor()
{
	return 0;
}

float OpenALProvider::getRolloffFactor()
{
	return 0;
}

float OpenALProvider::getCPUUsage()
{
	return 0;
}

void OpenALProvider::setFrequency(int aValue)
{
}

void OpenALProvider::setBufferSize(int aValue)
{
}

void OpenALProvider::setMaxChannels(int aValue)
{
}

void OpenALProvider::setListener(Vector3D& rPos, Vector3D& rVel, Vector3D& rForward, Vector3D& rTop)
{
	Vector3D orientation[2] = { rForward * -1, rTop };

	alListenerfv(AL_POSITION, rPos);
	alListenerfv(AL_VELOCITY, rVel);
	alListenerfv(AL_ORIENTATION, &orientation[0].x);
}

void OpenALProvider::setDistanceFactor(float aValue)
{
}

void OpenALProvider::setDopplerFactor(float aValue)
{
}

void OpenALProvider::setRolloffFactor(float aValue)
{
}

void OpenALProvider::setDriver(int aIndex)
{
}

void OpenALProvider::setOutput(int aIndex)
{
}

bool sortSourcesByPriority(OpenALSoundSourceNode* pA, OpenALSoundSourceNode* pB)
{
	return pA->getPriority() < pB->getPriority();
}

bool sortSourcesByDistance(OpenALSoundSourceNode* pA, OpenALSoundSourceNode* pB)
{
	Vector3D a, b, pos, tmp;

	if (!pA || !pB)
		return false;

	pA->getWorldMatrix().transform(pA->getTranslation(), a);
	pB->getWorldMatrix().transform(pB->getTranslation(), b);

	getEngine()->getSoundProvider()->getListener(pos, tmp, tmp, tmp);

	return a.getDistance(pos) < b.getDistance(pos);
}

void OpenALProvider::update3DSystem()
{
	ALint var;

	//--- check sources and if not playing set unused
	for (int i = 0; i < m_sources.size(); i++)
	{
		alGetSourcei(m_sources[i], AL_SOURCE_STATE, &var);

		if (var != AL_PLAYING)
		{
			if (m_usedSources[i])
			{
				if (m_usedSources[i]->getResource())
				{
					if (((OpenALSoundBufferData*)m_usedSources[i]->getResource())->m_bIsOGG &&
						m_usedSources[i]->m_oggStatus == 1)
					{
						continue;
					}
				}

				freeSource(i);
			}
		}
	}

	if (!m_sourcePlayRequesters.size())
		return;

	stable_sort(m_sourcePlayRequesters.begin(), m_sourcePlayRequesters.end(), sortSourcesByDistance);
	stable_sort(m_sourcePlayRequesters.begin(), m_sourcePlayRequesters.end(), sortSourcesByPriority);

	vector<OpenALSoundSourceNode*>::iterator it = m_sourcePlayRequesters.begin();

	while (it != m_sourcePlayRequesters.end())
	{
		getFreeSourceSlot(*it);
		it++;
	}

	m_sourcePlayRequesters.clear();
}

void OpenALProvider::freeSource(int aValue)
{
	if (m_usedSources[aValue])
	{
		alSourceStop(m_sources[aValue]);
		m_usedSources[aValue]->m_soundSource = 0;
		m_usedSources[aValue] = NULL;
	}
}

void OpenALProvider::freeSourceByHandle(ALuint aValue)
{
	for (unsigned int i = 0; i < m_sources.size(); i++)
	{
		if (m_sources[i] == aValue)
		{
			alSourceStop(m_sources[i]);

			if (m_usedSources[i])
			{
				m_usedSources[i]->m_soundSource = 0;
				m_usedSources[i] = NULL;
			}
			return;
		}
	}
}


bool compareSoundNodes(OpenALSoundSourceNode* pA, OpenALSoundSourceNode*pB)
{
	Vector3D a, b, pos, tmp;

	pA->getWorldMatrix().transform(pA->getTranslation(), a);
	pB->getWorldMatrix().transform(pB->getTranslation(), b);

	getEngine()->getSoundProvider()->getListener(pos, tmp, tmp, tmp);

	if (pA->getPriority() > pB->getPriority())
		return true;

	return a.getDistance(pos) >= b.getDistance(pos);
}

int OpenALProvider::getFreeSourceSlot(OpenALSoundSourceNode* pNode)
{
	if (!pNode)
		return 0;

	int slot = -1;

	//--- first try to find an existing free slot
	for (int i = 0; i < m_usedSources.size(); i++)
	{
		if (!m_usedSources[i])
		{
			slot = i;
			break;
		}
	}

	//--- oh well, steal a source slot from someone
	if (slot == -1)
	{
		for (int i = 0; i < m_usedSources.size(); i++)
		{
			if (m_usedSources[i])
			{
				if (m_usedSources[i]->getResource())
				{
					if (((OpenALSoundBufferData*)m_usedSources[i]->getResource())->m_bIsOGG &&
						m_usedSources[i]->m_oggStatus == 1)
						continue;
				}

				if (compareSoundNodes(m_usedSources[i], pNode))
				{
					slot = i;
					break;
				}
			}
		}
	}

	if (slot != -1)
	{
		alSourceStop(m_sources[slot]);

		if (m_usedSources[slot])
		{
			m_usedSources[slot]->m_soundSource = 0;
		}

		m_usedSources[slot] = pNode;
		m_usedSources[slot]->m_soundSource = m_sources[slot];

		if (pNode->getResource())
		{
			OpenALSoundBufferData* pBuffer = (OpenALSoundBufferData*) pNode->getResource();

			if (pBuffer->m_soundBuffer)
			{
				alSourcei(m_sources[slot], AL_BUFFER, pBuffer->m_soundBuffer);
			}

			alSourcef(m_sources[slot], AL_GAIN, pNode->m_volume);
			alSourcef(m_sources[slot], AL_PITCH, pNode->m_pitch);

			if (pBuffer->m_bIsOGG)
			{
				alSourcei(m_sources[slot], AL_LOOPING, 0);
				pNode->m_oggStatus = 1;
			}
			else
			{
				alSourcei(m_sources[slot], AL_LOOPING, pNode->m_looping);
				alSourcePlay(m_sources[slot]);
			}
		}
	}

	return slot;
}
