#pragma once
//*****************************************************************************
// Nytro Game Engine
//
// (C) Copyright 7thFACTOR Entertainment
// All rights reserved
//*****************************************************************************
#include "base/string.h"
#include "base/array.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/file.h"

namespace base
{
class File;
class Package;

//! A generic file manager, can handle files on disk or from a package\n
class B_API FileManager
{
public:
	enum EFileOpenPriority
	{
		eFileOpenPriority_PackagesFirst,
		eFileOpenPriority_DiskFirst
	};

	FileManager();
	~FileManager();

	bool initialize();
	bool shutdown();
	//! open a new file,
	//! you'll have to destroy it after you finish with it\n
	//! \return nullptr if file cannot be opened
	File* open(const char* pFilename, i32 aModeFlags = File::eMode_BinaryReadWrite);
	void setOpenFilePriority(EFileOpenPriority aPriority);
	EFileOpenPriority openFilePriority() const;
	//! check if a file exists, given the file path
	bool isFileExists(const char* pFilename) const;
	//! check if a folder exists, given the folder path
	bool isFolderExists(const char* pFolder) const;
	//! add a data folder
	bool addDataFolder(const char* pRoot);
	//! unregister a file location object by name
	void removeDataFolder(const char* pRoot);
	//! unregister all file locations (deletes the StreamLocation vector of pointers)
	void removeAllDataFolders();
	//! \return file locations
	const Array<String>& dataFolders() const;

protected:
	void scanAndAddPackagesFrom(const char* pFolder);
	File* openFromDisk(const char* pFilename, int aFlags);
	File* openFromPackages(const char* pFilename, int aFlags);

	EFileOpenPriority m_priority;
	String m_packagePassword;
	bool m_bIgnorePackages;
	Array<String> m_dataFolders;
	Array<Package*> m_packages;
};
}