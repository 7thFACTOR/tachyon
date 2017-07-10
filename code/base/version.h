#pragma once
#include "base/platform.h"
#include "base/string.h"

namespace base
{
struct Version
{
	u32 majorVersion = 0;
	u32 minorVersion = 1;
	u32 revision = 0;
	String branch;
	String changeList;

	Version() {}
	Version(u32 major, u32 minor, u32 rev)
		: majorVersion(major)
		, minorVersion(minor)
		, revision(rev)
	{}

	String toString() const
	{
		return String() << majorVersion << "." << minorVersion << "." << revision << " " << branch << " " << changeList;
	}
};

}