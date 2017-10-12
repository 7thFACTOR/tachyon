// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "base/file.h"
#include "core/bundle.h"
#include "core/globals.h"
#include "base/util.h"
#include "base/assert.h"
#include "base/logger.h"
#include "base/platform.h"
#include "base/variable.h"

namespace engine
{
class BundleFileMapper : public Stream
{
public:
	FILE* bundleDataFile = nullptr;
	u64 bundleFileSize = 0;
	u64 bundleFileOffset = 0;

	u64 getSize() const override
	{
		return bundleFileSize;
	}

	u64 getPosition() const override
	{
		if (bundleDataFile)
		{
			return ftell(bundleDataFile) - bundleFileOffset;
		}

		return 0;
	}

	bool read(void* buffer, u64 bufferSize) override
	{
		B_ASSERT(buffer);

		if (bundleDataFile)
		{
			return fread(buffer, bufferSize, 1, bundleDataFile) == bufferSize;
		}

		return true;
	}

	bool write(const void* buffer, u64 bufferSize) override
	{
		B_ASSERT(buffer);

		if (bundleDataFile)
		{
			return fwrite(buffer, 1, bufferSize, bundleDataFile) == bufferSize;
		}

		return false;
	}

	bool seek(SeekMode mode, u64 offset) override
	{
		if (bundleDataFile)
		{
			if (mode == SeekMode::Set)
			{
				fseek(bundleDataFile, bundleFileOffset + offset, (int)mode);
			}
			else if (mode == SeekMode::Current)
			{
				u64 curPos = ftell(bundleDataFile);
				fseek(bundleDataFile, curPos + offset, (int)mode);
			}

			return feof(bundleDataFile) != 1;
		}

		return false;
	}

	bool isEndOfStream() const override
	{
		if (bundleDataFile)
		{
			u64 crtPos = ftell(bundleDataFile);

			return feof(bundleDataFile)
				|| ((i64)crtPos - (i64)bundleFileOffset > bundleFileSize)
				|| ((i64)crtPos - (i64)bundleFileOffset < 0);
		}

		return true;
	}
};

//////////////////////////////////////////////////////////////////////////

Bundle::Bundle()
{
	opened = false;
	bundleMode = BundleMode::OneBigFile;
}

Bundle::~Bundle()
{
	close();
}

bool Bundle::open(const String& filename, BundleMode mode, const String& password)
{
	if (mode == BundleMode::Auto)
	{
		if (getVariableRegistry().getVariableValue<String>("sys_build_type") == "shipping")
		{
			mode = BundleMode::OneBigFile;
		}
		else
		{
			mode = BundleMode::LooseFiles;
		}
	}

	bundleMode = mode;
	bundlePassword = password;
	bundleFilename = filename;
	bundlePath = getFilenamePath(filename);

	if (!reloadToc())
	{
		return false;
	}

	if (mode == BundleMode::OneBigFile)
	{
		String bundleDataFilename;

		bundleDataFilename = getFilenameBasename(filename);
		bundleDataFilename = mergePathBasenameExtension(bundlePath, bundleDataFilename, ".bundle");

		if (!bundleDataFile.open(bundleDataFilename, FileOpenFlags::BinaryRead))
		{
			B_LOG_ERROR("Cannot open the bundle data file: " << bundleDataFilename);
			return false;
		}
		else
		{
			B_LOG_SUCCESS("Opened the bundle data file: " << bundleDataFilename);
		}
	}

	return true;
}

void Bundle::close()
{
	opened = false;
	bundleDataFile.close();
	tocEntries.clear();
}

bool Bundle::reloadToc()
{
	File toc;
	String tocFile = bundleFilename;

	// if we're in dev mode, the TOC is a separate file
	if (bundleMode == BundleMode::LooseFiles)
	{
		tocFile += ".toc";
	}
	else
	{
		tocFile += ".bundle";
	}

	tocEntries.clear();

	if (!toc.open(tocFile, FileOpenFlags::BinaryRead))
	{
		B_LOG_ERROR("Cannot load the bundle toc file: " << tocFile);
		return false;
	}

	u32 entries;
	ResourceId resId;

	isCompressed = false;
	
	toc >> isCompressed;
	toc.readUint32(entries);

	B_LOG_DEBUG("TOC file contains " << entries << " entries");
	BundleTocEntry entry;

	for (u32 i = 0; i < entries; ++i)
	{
		toc >> resId;
		toc >> entry.type;
		toc >> entry.offset;
		toc >> entry.size;
		toc >> entry.compressedSize;
		toc >> entry.name;
		tocEntries.add(resId, entry);
		/*B_LOG_DEBUG("Resource #" << resId
			<< ": name: " << entry.name
			<< " type: " << (u32)entry.type
			<< " offs: " << entry.offset
			<< " size: " << entry.size
			<< " csize: " << entry.compressedSize);*/
	}
	
	if (bundleMode == BundleMode::OneBigFile)
	{
		startOfResourceFileData = toc.getPosition();
	}
	else
	{
		startOfResourceFileData = 0;
	}

	B_LOG_INFO("Bundle '" << bundleFilename << "' loaded, " << (u32)tocEntries.size() << " entries");

	return true;
}

ResourceId Bundle::getResourceId(const String& resourceName) const
{
	for (auto entry : tocEntries)
	{
		if (entry.value.name == resourceName)
		{
			return entry.key;
		}
	}

	return nullResourceId;
}

bool Bundle::hasResourceId(ResourceId resId) const
{
	return tocEntries.contains(resId);
}

const String& Bundle::getResourceName(ResourceId resId) const
{
	size_t index = tocEntries.findIndex(resId);
	B_ASSERT(index != invalidDictionaryIndex);
	return tocEntries.valueAt(index).name;
}

ResourceType Bundle::getResourceType(ResourceId resId) const
{
	size_t index = tocEntries.findIndex(resId);
	B_ASSERT(index != invalidDictionaryIndex);
	return tocEntries.valueAt(index).type;
}

Stream* Bundle::startLoadResource(ResourceId resId)
{
	auto& entry = tocEntries[resId];

	if (bundleMode == BundleMode::OneBigFile)
	{
		BundleFileMapper* mappedFile = new BundleFileMapper();
		
		bundleDataFile.seek(SeekMode::Set, startOfResourceFileData + entry.offset);
		mappedFile->bundleFileOffset = startOfResourceFileData + entry.offset;
		mappedFile->bundleFileSize = entry.compressedSize;
		mappedFile->bundleDataFile = bundleDataFile.getFileHandle();

		return mappedFile;
	}
	else if (bundleMode == BundleMode::LooseFiles)
	{
		//TODO: use a File pool to avoid fragmentation?
		File* file = new File();
		String path = getFilenamePath(entry.name);
		String resFilename = mergePathName(bundlePath, path + "/" + toString(resId));

		if (file->open(resFilename, FileOpenFlags::BinaryRead))
		{
			return file;
		}

		B_LOG_DEBUG("Cannot open resource file " << resFilename << " (" << entry.name << ")");

		delete file;
	}

	return nullptr;
}

void Bundle::endLoadResource(ResourceId resId, Stream* stream)
{
	B_ASSERT(stream);
	delete stream;
}

}