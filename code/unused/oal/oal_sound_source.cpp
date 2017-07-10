#include "common.h"

#define OGG_PCM_BUFFER_SIZE ( 4096 * 60 )

void stream_check(int code)
{
	int error = alGetError();

	if (error != AL_NO_ERROR)
	{
		echo("OpenAL OGG error 0x%X (%d) was raised from thread section [%d]", error, error, code);
	}
}

bool stream_ogg_buffer(OpenALSoundSourceNode* pSrc, ALuint buffer)
{
	char pcm[ OGG_PCM_BUFFER_SIZE ];
	int  size = 0;
	int  section;
	int  res;

	if (!pSrc)
		return false;

	if (!pSrc->getResource())
		return false;

	OpenALSoundBufferData* pBuffer = (OpenALSoundBufferData*) pSrc->getResource();

	while (size < OGG_PCM_BUFFER_SIZE && !pSrc->m_bExitOGGThread)
	{
		res = ov_read(&pBuffer->oggStream,
					  pcm + size,
					  OGG_PCM_BUFFER_SIZE - size,
					  0, 2, 1, &section);

		if (res > 0)
		{
			size += res;
		}
		else
		{
			if (res < 0)
			{
				break;
			}
			else
			{
				break;
			}
		}
	}

	if (size == 0)
		return false;

	alBufferData(buffer, pBuffer->m_oggFormat,
				 pcm, size, pBuffer->m_frequency);

	stream_check(1);

	return true;
}

bool stream_isPlaying(OpenALSoundSourceNode* pSrc)
{
	if (!pSrc->m_soundSource)
		return false;

	ALenum state;

	alGetSourcei(pSrc->m_soundSource, AL_SOURCE_STATE, &state);

	return (state == AL_PLAYING);
}

bool stream_playback(OpenALSoundSourceNode* pSrc)
{
	if (!pSrc)
		return false;

	if (!pSrc->getResource())
		return false;

	OpenALSoundBufferData* pBuffer = (OpenALSoundBufferData*) pSrc->getResource();

	if (stream_isPlaying(pSrc))
		return true;

	if (!stream_ogg_buffer(pSrc, pBuffer->m_oggBuffers[0]))
	{
		return false;
	}

	if (!stream_ogg_buffer(pSrc, pBuffer->m_oggBuffers[1]))
	{
		return false;
	}

	alSourceQueueBuffers(pSrc->m_soundSource, 2, pBuffer->m_oggBuffers);
	alSourcePlay(pSrc->m_soundSource);

	return true;
}

bool stream_update(OpenALSoundSourceNode* pSrc)
{
	int processed;
	bool active = true;

	alGetSourcei(pSrc->m_soundSource, AL_BUFFERS_PROCESSED, &processed);

	while (processed-- && !pSrc->m_bExitOGGThread)
	{
		ALuint buffer;

		alSourceUnqueueBuffers(pSrc->m_soundSource, 1, &buffer);
		stream_check(2);

		active = stream_ogg_buffer(pSrc, buffer);

		alSourceQueueBuffers(pSrc->m_soundSource, 1, &buffer);
		stream_check(3);

		sleep(1);
	}

	return active;
}

void stream_empty(OpenALSoundSourceNode* pSrc)
{
	int queued;

	alGetSourcei(pSrc->m_soundSource, AL_BUFFERS_QUEUED, &queued);

	while (queued-- && !pSrc->m_bExitOGGThread)
	{
		ALuint buffer;
		alSourceUnqueueBuffers(pSrc->m_soundSource, 1, &buffer);
		stream_check(4);
	}
}

n_dword stream_ogg(void* pData)
{
	OpenALSoundSourceNode* pSrc = (OpenALSoundSourceNode*) pData;

	if (!pSrc)
		return false;

	if (!pSrc->getResource())
		return false;

	OpenALSoundBufferData* pBuffer = (OpenALSoundBufferData*) pSrc->getResource();

	pSrc->m_bCanBeDeleted = false;
	pSrc->m_bExitOGGThread = false;

	while (1)
	{
		if ((pSrc->m_oggStatus != 1 && !pSrc->m_bExitOGGThread) ||
			(!pSrc->m_soundSource && !pSrc->m_bExitOGGThread))
		{
			sleep(10);
			continue;
		}
		else
		{
			break;
		}
	}

	if (!stream_playback(pSrc))
	{
		echo("ERROR: OpenAL: cannot playback OGG stream");
	}
	else
		while (!pSrc->m_bExitOGGThread)
		{
			if (pSrc->m_oggStatus != 1 && !pSrc->m_bExitOGGThread)
			{
				sleep(10);
				continue;
			}

			//--- update
			if (!stream_update(pSrc))
			{
				pSrc->m_oggStatus = 0;
			}

			if (!stream_isPlaying(pSrc))
			{
				if (!stream_playback(pSrc))
				{
					pSrc->m_oggStatus = 0;
				}
			}

			if (pSrc->m_oggStatus != 1)
			{
				ov_raw_seek(&pBuffer->oggStream, 0);

				if (pSrc->m_bLoopOGG)
				{
					pSrc->m_oggStatus = 1;
				}
			}

			sleep(1);
		}

	stream_empty(pSrc);

	ov_clear(&pBuffer->oggStream);
	pSrc->m_bCanBeDeleted = true;

	return 0;
}

//---------------------------------------------------------------------------------
// Function : VorbisRead
// Purpose  : Callback for the Vorbis read function
// Info     :
//---------------------------------------------------------------------------------

size_t VorbisRead(void *ptr            /* ptr to the data that the vorbis files need*/,
				  size_t byteSize       /* how big a byte is*/,
				  size_t sizeToRead    /* How much we can read*/,
				  void *datasource      /* this is a pointer to the data we passed into ov_open_callbacks (our OggFileInfo struct*/)
{
	size_t              spaceToEOF;         // How much more we can read till we hit the EOF marker
	size_t              actualSizeToRead;   // How much data we are actually going to read from memory
	OggFileInfo*            vorbisData;         // Our vorbis data, for the typecast

	// Get the data in the right format
	vorbisData = (OggFileInfo*)datasource;

	// Calculate how much we need to read.  This can be sizeToRead*byteSize or less depending on how near the EOF marker we are
	spaceToEOF = vorbisData->dataSize - vorbisData->dataRead;

	if ((sizeToRead * byteSize) < spaceToEOF)
		actualSizeToRead = (sizeToRead * byteSize);
	else
		actualSizeToRead = spaceToEOF;

	// A simple copy of the data from memory to the datastruct that the vorbis libs will use
	if (actualSizeToRead)
	{
		// Copy the data from the start of the file PLUS how much we have already read in
		memcpy(ptr, (char*)vorbisData->dataPtr + vorbisData->dataRead, actualSizeToRead);
		// Increase by how much we have read by
		vorbisData->dataRead += (actualSizeToRead);
	}

	// Return how much we read (in the same way fread would)
	return actualSizeToRead;
}

//---------------------------------------------------------------------------------
// Function : VorbisSeek
// Purpose  : Callback for the Vorbis seek function
// Info     :
//---------------------------------------------------------------------------------
int VorbisSeek(void *datasource     /*this is a pointer to the data we passed into ov_open_callbacks (our OggFileInfo struct*/,
			   ogg_int64_t offset   /*offset from the point we wish to seek to*/,
			   int whence           /*where we want to seek to*/)
{
	size_t              spaceToEOF;     // How much more we can read till we hit the EOF marker
	ogg_int64_t         actualOffset;   // How much we can actually offset it by
	OggFileInfo*            vorbisData;     // The data we passed in (for the typecast)

	// Get the data in the right format
	vorbisData = (OggFileInfo*)datasource;

	// Goto where we wish to seek to
	switch (whence)
	{
	case SEEK_SET: // Seek to the start of the data file
		// Make sure we are not going to the end of the file
		if (vorbisData->dataSize >= offset)
			actualOffset = offset;
		else
			actualOffset = vorbisData->dataSize;
		// Set where we now are
		vorbisData->dataRead = (int)actualOffset;
		break;
	case SEEK_CUR: // Seek from where we are
		// Make sure we dont go past the end
		spaceToEOF = vorbisData->dataSize - vorbisData->dataRead;
		if (offset < spaceToEOF)
			actualOffset = (offset);
		else
			actualOffset = spaceToEOF;
		// Seek from our currrent location
		vorbisData->dataRead += actualOffset;
		break;
	case SEEK_END: // Seek from the end of the file
		vorbisData->dataRead = vorbisData->dataSize + 1;
		break;
	default:
		printf("*** ERROR *** Unknown seek command in VorbisSeek\n");
		break;
	};

	return 0;
}

//---------------------------------------------------------------------------------
// Function : VorbisClose
// Purpose  : Callback for the Vorbis close function
// Info     :
//---------------------------------------------------------------------------------
int VorbisClose(void *datasource /*this is a pointer to the data we passed into ov_open_callbacks (our OggFileInfo struct*/)
{
	// This file is called when we call ov_close.  If we wanted, we could free our memory here, but
	// in this case, we will free the memory in the main body of the program, so dont do anything
	return 1;
}

//---------------------------------------------------------------------------------
// Function : VorbisTell
// Purpose  : Classback for the Vorbis tell function
// Info     :
//---------------------------------------------------------------------------------
long VorbisTell(void *datasource /*this is a pointer to the data we passed into ov_open_callbacks (our OggFileInfo struct*/)
{
	OggFileInfo*    vorbisData;

	// Get the data in the right format
	vorbisData = (OggFileInfo*)datasource;

	// We just want to tell the vorbis libs how much we have read so far
	return vorbisData->dataRead;
}
/************************************************************************************************************************
End of Vorbis callback functions
***********************************************************************************************************************/

//---

OpenALSoundBufferData::OpenALSoundBufferData()
{
	m_soundBuffer = 0;
	m_bIsOGG = false;
	m_oggBuffers[0] = 0;
	m_oggBuffers[1] = 0;
	vorbisInfo = 0;
	vorbisComment = 0;
	m_oggFormat = 0;
	m_length = 0;
	m_frequency = 0;
	m_bIsOGG = false;
}

OpenALSoundBufferData::~OpenALSoundBufferData()
{
	unload();
}

Result OpenALSoundBufferData::load()
{
	ALenum format;
	ALsizei size;
	ALvoid* data;
	ALsizei freq;
	ALboolean loop;

	m_length = 0;

	alGetError();
	alGenBuffers(1, &m_soundBuffer);

	if (alGetError() != AL_NO_ERROR)
	{
		alDeleteBuffers(1, &m_soundBuffer);
		echo("OpenAL: Could not create a sound buffer for '%s'", m_fileName.c_str());
		return ENGINE_ERROR_GENERIC;
	}

	string ext;

	getFileExtension(m_fileName.c_str(), ext);

	SmartPtr<File> pFile;

	if (getEngine()->getFileProvider()->openFile(&pFile, m_fileName.c_str(), File::OPEN_BIN | File::OPEN_READ) != ALL_OK)
	{
		echo("OpenAL: could not open sound file '%s'", m_fileName.c_str());
		return ENGINE_ERROR_GENERIC;
	}

	char* buffer = NULL;
	toLowerCase(ext);
	m_bIsOGG = false;

	if (ext == "wav")
	{
		buffer = new char [ pFile->getFileSize() ];
		pFile->read(buffer, pFile->getFileSize());

		alutLoadWAVMemory((ALbyte*) buffer, &format, &data, &size, &freq, &loop);
		alBufferData(m_soundBuffer, format, data, size, freq);
		alutUnloadWAV(format, data, size, freq);

		m_length = size;
		m_frequency = freq;

		delete [] buffer;
	}
	else if (ext == "ogg")
	{
		int sizeOfFile = pFile->getFileSize();

		// Save the data into memory
		oggMemoryFile.dataPtr = new char [ sizeOfFile ];
		oggMemoryFile.dataRead = 0;
		oggMemoryFile.dataSize = sizeOfFile;

		pFile->read(oggMemoryFile.dataPtr, sizeOfFile);

		vorbisCallbacks.read_func = VorbisRead;
		vorbisCallbacks.close_func = VorbisClose;
		vorbisCallbacks.seek_func = VorbisSeek;
		vorbisCallbacks.tell_func = VorbisTell;

		if (ov_open_callbacks(&oggMemoryFile,
							  &oggStream,
							  NULL, 0,
							  vorbisCallbacks) != 0)
		{
			echo("ERROR: OpenAL: could not read OGG file: %s", m_fileName.c_str());
		}

		vorbisInfo = ov_info(&oggStream, -1);
		vorbisComment = ov_comment(&oggStream, -1);
		m_length = ov_time_total(&oggStream, -1);
		m_frequency = vorbisInfo->rate;

		if (vorbisInfo->channels == 1)
		{
			m_oggFormat = AL_FORMAT_MONO16;
		}
		else
		{
			m_oggFormat = AL_FORMAT_STEREO16;
		}

		alGenBuffers(2, m_oggBuffers);

		m_bIsOGG = true;
	}
	else
	{
		echo("OpenAL provider: Unknown sound file type '%s'", ext.c_str());
	}

	return ALL_OK;
}

Result OpenALSoundBufferData::unload()
{
	if (m_soundBuffer)
	{
		alDeleteBuffers(1, &m_soundBuffer);
	}

	if (m_oggBuffers[0])
		alDeleteBuffers(1, &m_oggBuffers[0]);

	if (m_oggBuffers[1])
		alDeleteBuffers(1, &m_oggBuffers[1]);

	m_soundBuffer = 0;
	m_oggBuffers[0] = 0;
	m_oggBuffers[1] = 0;

	if (oggMemoryFile.dataPtr)
	{
		delete[] oggMemoryFile.dataPtr;
		oggMemoryFile.dataPtr = NULL;
	}

	return ALL_OK;
}

//---

OpenALSoundSourceNode::OpenALSoundSourceNode() : SoundSourceNode()
{
	m_className = "SoundSourceNode";
	setNumberedName("SoundSourceNode");

	m_volume = 1;
	m_pitch = 1;
	m_soundSource = 0;
	m_bCanBeDeleted = false;
	m_bExitOGGThread = false;
	m_oggStatus = 0;
	m_looping = 0;
}

OpenALSoundSourceNode::~OpenALSoundSourceNode()
{
	if (m_pParent)
		m_pParent->removeChild(this);

	m_bExitOGGThread = true;

	if (m_soundSource)
		m_pOALProvider->freeSourceByHandle(m_soundSource);

	vector<OpenALSoundSourceNode*>::iterator it = m_pOALProvider->m_sourcePlayRequesters.begin();

	while (it != m_pOALProvider->m_sourcePlayRequesters.end())
	{
		if (*it == this)
		{
			m_pOALProvider->m_sourcePlayRequesters.erase(it);
			break;
		}

		it++;
	}

	it = m_pOALProvider->m_usedSources.begin();

	while (it != m_pOALProvider->m_usedSources.end())
	{
		if (*it == this)
		{
			*it = NULL;
		}

		it++;
	}

	if (m_pResource)
	{
		if (((OpenALSoundBufferData*)m_pResource)->m_bIsOGG)
		{
			while (!m_bCanBeDeleted)
			{
				sleep(1);
			}
		}
	}
}

void OpenALSoundSourceNode::play()
{
	if (m_pResource)
	{
		if (m_soundSource == 0)
		{
			m_pOALProvider->requestSourcePlay(this);
		}

		if (!((OpenALSoundBufferData*)m_pResource)->m_bIsOGG)
		{
			alSourcePlay(m_soundSource);
		}
	}

	m_oggStatus = 1;
}

float OpenALSoundSourceNode::getVolume()
{
	if (!m_soundSource)
		return m_volume;

	float volume;

	alGetSourcef(m_soundSource, AL_GAIN, &volume);

	return volume;
}

int OpenALSoundSourceNode::getTime()
{
	return 0;
}

int OpenALSoundSourceNode::getLength()
{
	if (m_pResource)
	{
		return ((OpenALSoundBufferData*)m_pResource)->m_length;
	}

	return 0;
}

int OpenALSoundSourceNode::getFrequency()
{
	if (!m_soundSource)
		return 0;

	int freq;

	alGetBufferi(m_soundSource, AL_FREQUENCY, &freq);

	return freq;
}

void OpenALSoundSourceNode::getVU(float& rLeft, float& rRight)
{
}

bool OpenALSoundSourceNode::isMuted()
{
	if (!m_soundSource)
		return true;

	int state;

	alGetSourcei(m_soundSource, AL_SOURCE_STATE, &state);

	return state == AL_STOPPED;
}

int OpenALSoundSourceNode::getPan()
{
	return 0;
}

bool OpenALSoundSourceNode::isPaused()
{
	if (!m_soundSource)
		return true;

	int state;

	alGetSourcei(m_soundSource, AL_SOURCE_STATE, &state);

	return state == AL_PAUSED;
}

bool OpenALSoundSourceNode::isSurround()
{
	return true;
}

bool OpenALSoundSourceNode::isPlaying()
{
	if (!m_soundSource)
		return false;

	int state;

	alGetSourcei(m_soundSource, AL_SOURCE_STATE, &state);

	return state == AL_PLAYING;
}

void OpenALSoundSourceNode::setVolume(float aValue)
{
	m_volume = aValue;

	if (!m_soundSource)
		return;

	alSourcef(m_soundSource, AL_GAIN, aValue);
}

void OpenALSoundSourceNode::setTime(int aValue)
{
}

void OpenALSoundSourceNode::setFrequency(int aValue)
{
}

void OpenALSoundSourceNode::setMuted(bool aValue)
{
	if (!m_soundSource)
		return;

	if (aValue)
	{
		alSourcePause(m_soundSource);
	}
}

void OpenALSoundSourceNode::setPan(int aValue)
{
}

void OpenALSoundSourceNode::setPaused(bool aValue)
{
	if (!m_soundSource)
		return;

	if (aValue)
	{
		alSourcePause(m_soundSource);
		m_oggStatus = 2;
	}
	else
	{
		alSourcePlay(m_soundSource);
		m_oggStatus = 1;
	}
}

void OpenALSoundSourceNode::setSurround(bool aValue)
{
}

void OpenALSoundSourceNode::setLoopMode(int aValue)
{
	m_bLoopOGG = aValue;
	m_looping = aValue;

	if (!m_soundSource)
		return;

	if (m_pResource)
	{
		if (((OpenALSoundBufferData*)m_pResource)->m_bIsOGG)
		{
			alSourcei(m_soundSource, AL_LOOPING, 0);
		}
	}

	alSourcei(m_soundSource, AL_LOOPING, aValue);
}

void OpenALSoundSourceNode::setChannel(int aValue)
{
}

void OpenALSoundSourceNode::stop()
{
	if (!m_soundSource)
		return;

	alSourceStop(m_soundSource);
	m_oggStatus = 0;
}

Vector3D& OpenALSoundSourceNode::getVelocity()
{
	return m_velocity;
}

void OpenALSoundSourceNode::setVelocity(Vector3D& rVel)
{
	if (!m_soundSource)
		return;

	m_velocity = rVel;

	alSourcefv(m_soundSource, AL_POSITION, m_translation);
	alSourcefv(m_soundSource, AL_VELOCITY, m_velocity);
}

void OpenALSoundSourceNode::setFromResource(Resource* pRes)
{
	SoundSourceNode::setFromResource(pRes);

	if (!pRes)
		return;

	OpenALSoundBufferData* pBuffer = (OpenALSoundBufferData*) pRes;

	if (pBuffer->m_bIsOGG)
	{
		m_bExitOGGThread = false;
		m_bCanBeDeleted = false;
		m_oggStatus = 0;
		createThread(stream_ogg, this);
	}
}

void OpenALSoundSourceNode::setPitch(float aValue)
{
	m_pitch = aValue;

	if (!m_soundSource)
		return;

	alSourcef(m_soundSource, AL_PITCH, aValue);
}

float OpenALSoundSourceNode::getPitch()
{
	if (!m_soundSource)
		return m_pitch;

	float pitch;

	alGetSourcef(m_soundSource, AL_PITCH, &pitch);

	return pitch;
}

void OpenALSoundSourceNode::computeTransform()
{
	SceneNode::computeTransform();

	if (m_soundSource)
	{
		Vector3D trans;
		m_worldMatrix.transform(m_translation, trans);

		alSourcef(m_soundSource, AL_MAX_DISTANCE, 1000);
		alSourcef(m_soundSource, AL_REFERENCE_DISTANCE, 100);
		alSourcefv(m_soundSource, AL_POSITION, trans);
		alSourcefv(m_soundSource, AL_VELOCITY, m_velocity);
	}
}