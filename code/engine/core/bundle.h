// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/dictionary.h"
#include "base/string.h"
#include "base/defines.h"
#include "base/types.h"
#include "core/defines.h"
#include "core/types.h"
#include "base/file.h"

#ifdef USE_ZLIB
#include "contrib/minizip/zip.h"
#include "contrib/minizip/unzip.h"
#include "zutil.h"
#endif

namespace base
{
	class Stream;
}

namespace engine
{
using namespace base;

enum class BundleMode
{
	//! the mode will be chosen based on startup arguments and/or sys_build_type variable
	Auto,
	//! the bundle and their list (.TOC) has all the resources on disk as individual files, good for reloading and editor
	LooseFiles,
	//! the files and their list are packed as chunks in one big bundle file
	OneBigFile,

	Count
};

struct BundleTocEntry
{
	ResourceType type = ResourceType::None;
	//! offset inside bundle
	u64 offset = 0;
	//! uncompressed resource size
	u64 size = 0;
	//! compressed resource size
	u64 compressedSize = 0;
	//! resource filename
	String name;
};

class E_API Bundle
{
public:
	Bundle();
	virtual ~Bundle();
	bool open(
		const String& filename,
		BundleMode mode,
		const String& password);
	void close();
	bool reloadToc();
	ResourceId getResourceId(const String& resourceName) const;
	bool hasResourceId(ResourceId resId) const;
	const String& getResourceName(ResourceId resId) const;
	ResourceType getResourceType(ResourceId resId) const;
	const String& getFilename() const { return bundleFilename; }
	Stream* startLoadResource(ResourceId resId);
	void endLoadResource(ResourceId resId, Stream* stream);
	const Dictionary<ResourceId, BundleTocEntry>& getTocEntries() const { return tocEntries; }
	
protected:
	String bundlePassword;
	String bundleFilename;
	String bundlePath;
	bool opened = false;
	bool isCompressed = false;
	u64 startOfResourceFileData = 0;
	BundleMode bundleMode = BundleMode::Auto;
	File bundleDataFile;
	Dictionary<ResourceId, BundleTocEntry> tocEntries;
#if defined(E_USE_ZLIB)
	unzFile zipHandle;
#endif
};

}