#include "core/resource.h"
#include "core/core.h"

namespace engine
{
//Resource::Resource()
//{
//	m_dataSize = 0;
//	m_usageCount = 0;
//	m_levelOfDetail = LevelsOfDetail::Low;
//	m_resourceFlags = 0;
//	m_type = ResourceTypes::None;
//}
//
//Resource::~Resource()
//{
//}
//
//bool Resource::load(Stream* pStream)
//{
//	m_resourceFlags |= ResourceFlags::Loaded;
//
//	return false;
//}
//
//bool Resource::unload()
//{
//	m_resourceFlags &= ~ResourceFlags::Loaded;
//
//	return true;
//}
//
//void Resource::lock()
//{
//	m_lock.lock();
//}
//
//void Resource::unlock()
//{
//	m_lock.unlock();
//}
//
//void Resource::setLoaded(bool bIsLoaded)
//{
//	bIsLoaded ? m_resourceFlags |= ResourceFlags::Loaded : m_resourceFlags &= ~ResourceFlags::Loaded;
//}
//
//void Resource::onDependencyUnload(Resource* pDependencyRes)
//{
//}
//
//void Resource::onDependencyLoad(Resource* pDependencyRes)
//{
//}
//
//void Resource::setCurrentLod(LevelOfDetail aLOD)
//{
//	m_levelOfDetail = aLOD;
//}

}