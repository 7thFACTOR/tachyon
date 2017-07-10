#include "resources/sound_buffer.h"
#include "audio/audio_manager.h"
#include "core/core.h"
#include "core/resource_manager.h"

namespace engine
{
B_REGISTER_DERIVED_TAGGED_CLASS(SoundBuffer, Resource, ResourceTypes::Sound);

SoundBuffer::SoundBuffer()
{
	m_pData = nullptr;
	m_length = 0;
}

SoundBuffer::~SoundBuffer()
{
	unload();
}

bool SoundBuffer::load(Stream* pStream)
{
	Resource::load(pStream);
	m_pData = new u8 [(u32)pStream->size()];
	B_ASSERT(m_pData);
	pStream->read(m_pData, pStream->size());
	m_length = (u32)pStream->size();

	return true;
}

bool SoundBuffer::unload()
{
	if (m_pData)
	{
		delete [] m_pData;
		m_length = 0;
		m_pData = nullptr;
	}

	return true;
}

}