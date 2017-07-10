#include <string.h>
#include "base/package.h"
#include "base/file_manager.h"
#include "base/core.h"
#include "base/util.h"
#include "base/assert.h"
#include "base/logger.h"
#include "base/platform.h"
#include "base/disk_file.h"

namespace base
{

FileManager::FileManager()
{
	m_bIgnorePackages = false;
	m_priority = eFileOpenPriority_DiskFirst;
}

FileManager::~FileManager()
{
}

bool FileManager::initialize()
{
	return true;
}

bool FileManager::shutdown()
{
	for (size_t i = 0; i < m_packages.size(); ++i)
	{
		BASE_SAFE_DELETE(m_packages[i]);
	}

	m_packages.clear();
	m_dataFolders.clear();

	return true;
}

File* FileManager::open(const char* pFilename, i32 aModeFlags)
{
	File* pFile = nullptr;

	if (m_priority == eFileOpenPriority_DiskFirst)
	{
		pFile = openFromDisk(pFilename, aModeFlags);

		if (!pFile)
		{
			pFile = openFromPackages(pFilename, aModeFlags);
		}
	}
	else if (m_priority == eFileOpenPriority_PackagesFirst)
	{
		pFile = openFromPackages(pFilename, aModeFlags);

		if (!pFile)
		{
			pFile = openFromDisk(pFilename, aModeFlags);
		}
	}

	return pFile;
}

void FileManager::setOpenFilePriority(EFileOpenPriority aPriority)
{
	m_priority = aPriority;
}

FileManager::EFileOpenPriority FileManager::openFilePriority() const
{
	return m_priority;
}

bool FileManager::isFileExists(const char* pFilename) const
{
	//TODO
	return false;
}

bool FileManager::isFolderExists(const char* pFolder) const
{
	//TODO
	return false;
}

bool FileManager::addDataFolder(const char* pRoot)
{
	if (m_dataFolders.find(pRoot) != m_dataFolders.end())
	{
		N_LOG_DEBUG("Data folder '%s' already registered", pRoot);
		return false;
	}

	m_dataFolders.append(pRoot);

	if (!m_bIgnorePackages)
	{
		scanAndAddPackagesFrom(pRoot);
	}

	return true;
}

void FileManager::removeDataFolder(const char* pRoot)
{
	Array<String>::Iterator iter = m_dataFolders.find(pRoot);
	
	if (iter == m_dataFolders.end())
	{
		N_LOG_DEBUG("Data folder %s not registered", pRoot);
		return;
	}

	m_dataFolders.erase(iter);

	//TODO: remove packages from this folder ?
}

void FileManager::removeAllDataFolders()
{
	m_dataFolders.clear();
}

const Array<String>& FileManager::dataFolders() const
{
	return m_dataFolders;
}

void FileManager::scanAndAddPackagesFrom(const char* pFolder)
{
	Array<FoundFileInfo> paks;

	if (scanFileSystem(pFolder, "*.npak", true, false, paks))
	{
		for (size_t i = 0; i < paks.size(); ++i)
		{
			Package* pPak = new Package();
			FoundFileInfo& file = paks[i];
			
			B_LOG_INFO("Opening package %s...", file.fileName.c_str());

			if (!pPak->open(file.fileName.c_str(), m_packagePassword.c_str()))
			{
				B_LOG_ERROR("Could not open package %s!", file.fileName.c_str());
				delete pPak;
			}
			else
			{
				m_packages.append(pPak);
				N_LOG_SUCCESS("Package %s added", file.fileName.c_str());
			}
		}
	}
}

File* FileManager::openFromDisk(const char* pFilename, int aFlags)
{
	File* pFile = new DiskFile();
	String relPath;

	for (size_t i = 0; i < m_dataFolders.size(); ++i)
	{
		relPath = mergePathFile(m_dataFolders[i].c_str(), pFilename);

		if (pFile->open(relPath.c_str(), aFlags))
		{
			return pFile;
		}
	}

	if (pFile->open(pFilename, aFlags))
	{
		return pFile;
	}
	
	delete pFile;

	return nullptr;
}

File* FileManager::openFromPackages(const char* pFilename, int aFlags)
{
	File* pFile;

	for (size_t i = 0; i < m_packages.size(); ++i)
	{
		pFile = m_packages[i]->openFile(pFilename, aFlags);

		if (pFile)
		{
			return pFile;
		}
	}
	
	return nullptr;
}
}