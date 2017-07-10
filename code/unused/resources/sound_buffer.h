#pragma once
#include "base/defines.h"
#include "core/defines.h"
#include "core/resource.h"

namespace engine
{
using namespace base;

namespace SoundChunkIds
{
	enum SoundChunkId
	{
		Header,
		Data
	};
}
typedef SoundChunkIds::SoundChunkId SoundChunkId;

//! Holds the actual sound data
class E_API SoundBuffer : public Resource
{
public:
	B_RUNTIME_CLASS;
	SoundBuffer();
	virtual ~SoundBuffer();
	bool load(Stream* pStream);
	bool unload();
	u8* data() const { return m_pData; }
	u32 length() const { return m_length; }

protected:
	u8* m_pData;
	u32 m_length;
};

}