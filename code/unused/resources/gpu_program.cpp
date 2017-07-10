#include "resources/gpu_program.h"
#include "render/render_manager.h"
#include "core/core.h"
#include "base/util.h"
#include "base/assert.h"
#include "base/logger.h"
#include "base/variable.h"
#include "core/resource_manager.h"

namespace engine
{
B_REGISTER_VARIABLE(gpuProgramCacheEnabled, VariantTypes::Bool, "True if the gpu program caching (reusing precompiled Cg program code) is enabled", false, 0);
B_REGISTER_ABSTRACT_CLASS(GpuProgram);

GpuProgram::GpuProgram()
{
	m_pController = nullptr;
}

GpuProgram::~GpuProgram()
{
}

bool GpuProgram::unload()
{
	B_SAFE_DELETE(m_pController);
	m_pCurrentRenderMethod = nullptr;

	for (size_t i = 0, iCount = m_renderMethods.size(); i < iCount; ++i)
	{
		delete m_renderMethods[i];
	}

	m_renderMethods.clear();

	return true;
}

bool GpuProgram::load(Stream* pStream)
{
	Resource::load(pStream);

	//check if this gpu program is already in the cache, if its enabled
	if (variable<bool>("gpuProgramCacheEnabled"))
	{
		//TODO: check if cache file 'gpuprogramcache/some.ngpuprogramcache' exists
		// can load blobs for DX11
	}

	String str;

	while (!pStream->eof())
	{
		ChunkHeader chunk;

		if (!pStream->readChunkHeader(&chunk))
			break;

		switch (chunk.id)
		{
		case eGpuProgramChunk_Header:
			{
				pStream->readString(str); // controller class name

				if (!str.isEmpty())
				{
					m_pController = (GpuProgramController*) newClassInstance(str.c_str());
				}
				break;
			}
		case eGpuProgramChunk_RenderMethods:
			{
				i16 methodCount;

				pStream->readInt16(methodCount);
				B_LOG_DEBUG("Reading %d render methods", methodCount);

				for (int i = 0; i < methodCount; ++i)
				{
					GpuProgramRenderMethod* pRenderMethod = new GpuProgramRenderMethod();
					
					if (pRenderMethod->load(pStream))
					{
						m_renderMethods.append(pRenderMethod);
						
						if (pRenderMethod->isDefault())
						{
							m_pCurrentRenderMethod = pRenderMethod;
						}
					}
					else
					{
						B_LOG_ERROR("Could not load gpu program render method #%d", i);
						delete pRenderMethod;
					}
				}

				B_LOG_DEBUG("End reading gpu program render methods");
				break;
			}
		default:
			pStream->ignoreChunk(&chunk);
		}
	}
	
	if (!m_pCurrentRenderMethod && !m_renderMethods.isEmpty())
	{
		m_pCurrentRenderMethod = m_renderMethods[0];
	}

	return true;
}

GpuProgramController* GpuProgram::controller() const
{
	return m_pController;
}

void GpuProgram::bind()
{
}

void GpuProgram::unbind()
{
}

const Array<GpuProgramRenderMethod*>& GpuProgram::renderMethods() const
{
	return m_renderMethods;
}

bool GpuProgram::setCurrentRenderMethod(const char* pName)
{
	for (size_t i = 0, iCount = m_renderMethods.size(); i < iCount; ++i)
	{
		if (m_renderMethods[i]->name() == pName)
		{
			m_pCurrentRenderMethod = m_renderMethods[i];
			return true;
		}
	}

	return false;
}

void GpuProgram::setCurrentRenderMethod(GpuProgramRenderMethod* pRenderMethod)
{
	m_pCurrentRenderMethod = pRenderMethod;
}

void GpuProgram::setController(GpuProgramController* pCtrl)
{
	m_pController = pCtrl;
}

GpuProgramRenderMethod* GpuProgram::currentRenderMethod() const
{
	return m_pCurrentRenderMethod;
}

GpuProgramRenderMethod* GpuProgram::renderMethod(const char* pRenderMethodName) const
{
	for (size_t i = 0, iCount = m_renderMethods.size(); i < iCount; ++i)
	{
		if (m_renderMethods[i]->name() == pRenderMethodName)
		{
			return m_renderMethods[i];
		}
	}

	return nullptr;
}
}