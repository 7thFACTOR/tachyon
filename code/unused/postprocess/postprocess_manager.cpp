#include "render/postprocess/postprocess_manager.h"
#include "render/renderer.h"
#include "core/core.h"
#include "core/resource_manager.h"
#include "base/assert.h"
#include "base/logger.h"
#include "render/resources/material_resource.h"
#include "render/scene/scene_renderer.h"

namespace engine
{
PostProcessMap::PostProcessMap()
{
	scale = 1.0f;
	bDepthMap = false;
	width = 0;
	height = 0;
	texture = kInvalidResourceId;
	format = TextureFormat::RGBA8;
	sysTexId = -1;
	bAutoScale = true;
}

PostProcessMap::~PostProcessMap()
{
	free();
}

bool PostProcessMap::create()
{
	free();
	//pTexture = render().newTexture();
	//B_ASSERT(pTexture);

	//if (!pTexture)
	//	return false;

	//u32 screenWidth = render().videoMode().width;
	//u32 screenHeight = render().videoMode().height;
	//u32 texWidth = width ? width : screenWidth;
	//u32 texHeight = height ? height : screenHeight;

	//if (scale != 1.0f)
	//{
	//	texWidth = (f32) scale * screenWidth;
	//	texHeight = (f32) scale * screenHeight;
	//}

	//pTexture->setName(name.c_str());
	//pTexture->setRenderTargetAutoScaling(bAutoScale, scale);

	//if (fileName.isEmpty())
	//{
	//	pTexture->setAsRenderTarget(
	//						format,
	//						texWidth, texHeight,
	//						bColorMap, bDepthMap, bStencilMap);
	//}
	//else
	//{
	//	pTexture->setFileName(fileName.c_str());
	//}

	//bool result = pTexture->load();

	//return result;
	return false;
}

void PostProcessMap::free()
{
	resources().unload(texture);
}

//---

PostProcess::PostProcess()
{
	m_bActive = true;
}

PostProcess::~PostProcess()
{
	free();
}

void PostProcess::free()
{}

const Map<String, PostProcessMap*>& PostProcess::inputMaps() const
{
	return m_inputMaps;
}

const Map<String, PostProcessMap*>& PostProcess::outputMaps() const
{
	return m_outputMaps;
}

void PostProcess::setActive(bool bActive)
{
	m_bActive = bActive;
}

bool PostProcess::isActive() const
{
	return m_bActive;
}

void PostProcess::setSortKey(u32 key)
{
	m_sortKey = key;
}

u32 PostProcess::sortKey() const
{
	return m_sortKey;
}

//---

PostProcessManager::PostProcessManager()
{
}

PostProcessManager::~PostProcessManager()
{
}

void PostProcessManager::free()
{
	unregisterMaps();

	for (size_t i = 0, iCount = m_maps.size(); i < iCount; ++i)
	{
		m_maps[i]->free();
		B_SAFE_DELETE(m_maps[i]);
	}

	m_maps.clear();

	for (size_t i = 0, iCount = m_postProcesses.size(); i < iCount; ++i)
	{
		B_SAFE_DELETE(m_postProcesses[i]);
	}

	m_postProcesses.clear();
}

PostProcessMap* PostProcessManager::map(const String& name) const
{
	for (size_t i = 0, iCount = m_maps.size(); i < iCount; ++i)
	{
		if (m_maps[i]->name == name)
		{
			return m_maps[i];
		}
	}

	return nullptr;
}

PostProcess* PostProcessManager::stage(const String& name) const
{
	for (size_t i = 0, iCount = m_postProcesses.size(); i < iCount; ++i)
	{
		if (m_postProcesses[i]->name() == name)
		{
			return m_postProcesses[i];
		}
	}

	return nullptr;
}

const Array<PostProcessMap*>& PostProcessManager::maps() const
{
	return m_maps;
}

const Array<PostProcess*>& PostProcessManager::postProcesses() const
{
	return m_postProcesses;
}

const Map<String, PostProcessMap*>& PostProcessManager::mapAliases() const
{
	return m_mapAliases;
}

void PostProcessManager::setMapAlias(const String& name, PostProcessMap* pMap)
{
	m_mapAliases[name] = pMap;
}

void PostProcessManager::registerMaps()
{
	B_LOG_INFO("=== Registering post process maps:");

	for (size_t i = 0, iCount = m_maps.size(); i < iCount; ++i)
	{
		B_LOG_INFO("    Registering map as system map: postprocess:%s",
			m_maps[i]->name.c_str());
		
		u32 id = renderer().registerSystemTexture(
			m_maps[i]->texture, 
			(String("postprocess:") + m_maps[i]->name).c_str());
		
		if (id)
		{
			m_maps[i]->sysTexId = id;
		}
	}
}

void PostProcessManager::unregisterMaps()
{
	B_LOG_INFO("=== Unregistering post process maps:");

	for (size_t i = 0, iCount = m_maps.size(); i < iCount; ++i)
	{
		B_LOG_INFO(
			"    Unregistering post process map from system maps:"
			" postprocess:%s",
			m_maps[i]->name.c_str());
		renderer().unregisterSystemTexture(m_maps[i]->sysTexId);
	}
}

void PostProcessManager::recreateMaps()
{
	B_LOG_INFO("Recreating post process maps...");

	for (size_t i = 0, iCount = m_maps.size(); i < iCount; ++i)
	{
		m_maps[i]->free();
		m_maps[i]->create();
	}
}

bool PostProcessManager::addMap(PostProcessMap* pMap)
{
	auto iter = m_maps.find(pMap); 

	if (iter != m_maps.end())
	{
		return false;
	}

	m_maps.append(pMap);

	return true;
}

bool PostProcessManager::addPostProcess(PostProcess* pPostProcess)
{
	auto iter = m_postProcesses.find(pPostProcess); 

	if (iter != m_postProcesses.end())
	{
		return false;
	}

	m_postProcesses.append(pPostProcess);

	return true;
}

void PostProcessManager::removeMaps()
{
	m_maps.clear();
}

void PostProcessManager::removePostProcesses()
{
	m_postProcesses.clear();
}

void PostProcessManager::deleteMaps()
{
	for (size_t i = 0; i < m_maps.size(); ++i)
	{
		delete m_maps[i];
	}

	m_maps.clear();
}

void PostProcessManager::deletePostProcesses()
{
	for (size_t i = 0; i < m_postProcesses.size(); ++i)
	{
		delete m_postProcesses[i];
	}

	m_postProcesses.clear();
}

void PostProcessManager::removeMap(PostProcessMap* pMap)
{
	auto iter = m_maps.find(pMap); 

	if (iter == m_maps.end())
	{
		return;
	}

	m_maps.erase(iter);
}

void PostProcessManager::removePostProcess(PostProcess* pPostProcess)
{
	auto iter = m_postProcesses.find(pPostProcess); 

	if (iter == m_postProcesses.end())
	{
		return;
	}

	m_postProcesses.erase(iter);
}

void PostProcessManager::process()
{
	for (size_t i = 0; i < m_postProcesses.size(); ++i)
	{
		PostProcess* pPostProcess = m_postProcesses[i];

		pPostProcess->process();
	}
}

}