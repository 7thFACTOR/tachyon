#include "animation_events.h"
#include "game/flowgraph/flowgraph.h"
#include "system/core.h"
#include "system/assert.h"
#include "system/resource_manager.h"
#include "audio/audio_manager.h"
#include "audio/sound_source.h"
#include "audio/sound_buffer_data.h"

namespace nytro
{
SoundAnimationEvent::SoundAnimationEvent()
{
	m_pSound = nullptr;
	m_pParent = nullptr;
	m_bFollowParent = true;
}

SoundAnimationEvent::~SoundAnimationEvent()
{
	N_SAFE_DELETE(m_pSound);
}

void SoundAnimationEvent::onTrigger()
{
	if (!m_pSound)
	{
		m_pSound = audio().newSoundSource();
		N_ASSERT(m_pSound);

		if (m_pSound)
		{
			m_pSound->setSoundBufferData(resources().load<SoundBufferData>(m_soundFile.c_str()));
		}
	}

	if (m_pSound)
	{
		m_pSound->stopSound();
		m_pSound->setVolume(m_volume);
		m_pSound->playSound();
	}
}

//---

FlowGraphAnimationEvent::FlowGraphAnimationEvent()
{
}

FlowGraphAnimationEvent::~FlowGraphAnimationEvent()
{
}

void FlowGraphAnimationEvent::onTrigger()
{
	if (m_pGraph)
	{
		m_pGraph->start();
	}
}

} //ns