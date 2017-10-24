// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/string.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/dictionary.h"

namespace base
{
struct CodeCoverageMarker
{
	CodeCoverageMarker(const String& markerName = "")
		: name(markerName)
		, hitCount(0)
		, tracked(false)
	{}

	String name;
	u64 hitCount = 0;
	bool tracked = false;
};

class B_API CodeCoverage
{
public:
	CodeCoverage()
		: hitMarkerCount(0)
		, mustHitMarkerCount(0)
		, mustHitMarkersFile(nullptr)
		, hitMarkersFile(nullptr)
		, hitMarkersFilename("cc_results.txt")
		, saveAllCodeMarkers(false)
	{}

	~CodeCoverage();

	bool beginRecording(
		const String& mustHitMarkersFilename = "cc_markers.txt",
		const String& hitMarkersFilename = "cc_results.txt",
		bool saveAllMarkers = false);
	void saveResultsFile();
	f32 getCompletionProgress();
	void hitMarker(const String& name);
	void addMarker(const String& name);

protected:
	Dictionary<String, CodeCoverageMarker*> markers;
	u32 hitMarkerCount = 0;
	u32 mustHitMarkerCount = 0;
	FILE* mustHitMarkersFile = nullptr;
	FILE* hitMarkersFile = nullptr;
	String hitMarkersFilename;
	bool saveAllCodeMarkers = false;
};

}