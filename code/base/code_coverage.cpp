// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include <stdio.h>
#include "base/platform.h"
#include "base/assert.h"
#include "base/code_coverage.h"
#include "base/util.h"
#include "base/logger.h"
#include "base/dictionary.h"

namespace base
{
CodeCoverage::~CodeCoverage()
{
	for (auto& marker : markers)
	{
		delete marker.value;
	}
}

bool CodeCoverage::beginRecording(
	const String& mustHitMarkersFilename,
	const String& markersFilename,
	bool saveAllMarkers)
{
	mustHitMarkersFile = fopen(mustHitMarkersFilename.c_str(), "rt");
	hitMarkerCount = 0;
	hitMarkersFilename = markersFilename;
	saveAllCodeMarkers = saveAllMarkers;

	if (mustHitMarkersFile)
	{
		String markerName;
		const u32 maxLineCharCount = 1000;
		char line[maxLineCharCount];

		fgets(line, maxLineCharCount, mustHitMarkersFile);
		mustHitMarkerCount = 0;

		while (!feof(mustHitMarkersFile))
		{
			markerName = base::trim(String(line));
			addMarker(markerName);
			++mustHitMarkerCount;
			fgets(line, maxLineCharCount, mustHitMarkersFile);
		}

		fclose(mustHitMarkersFile);
	}
	else
	{
		B_LOG_WARNING("No code coverage \"markers to hit\" file found, '" << mustHitMarkersFilename << "' was specified, will use all registered markers");
		mustHitMarkerCount = markers.size();
	}

	return true;
}

void CodeCoverage::saveResultsFile()
{
	hitMarkersFile = fopen(hitMarkersFilename.c_str(), "wt");

	if (hitMarkersFile)
	{
		fprintf(hitMarkersFile, "%d%%\n", (i32)(100.0f * getCompletionProgress()));

		for (size_t i = 0, iCount = markers.size(); i < iCount; ++i)
		{
			if (markers[i]->tracked || saveAllCodeMarkers)
			{
				fprintf(hitMarkersFile, "%s,%I64d\n", markers[i]->name.c_str(), (i64)markers[i]->hitCount);
			}
		}

		fflush(hitMarkersFile);
		fclose(hitMarkersFile);
	}
}

f32 CodeCoverage::getCompletionProgress()
{
	if (!mustHitMarkerCount)
	{
		return 0.0f;
	}

	return (f32)hitMarkerCount / mustHitMarkerCount;
}

void CodeCoverage::addMarker(const String& name)
{
	CodeCoverageMarker* marker = new CodeCoverageMarker(name);
	markers.add(name, marker);
	marker->tracked = true;
}

void CodeCoverage::hitMarker(const String& name)
{
	if (!markers.contains(name))
	{
		markers.add(name, new CodeCoverageMarker(name));
	}

	++markers[name]->hitCount;

	// save each time we hit a marker for the first time
	if (markers[name]->hitCount == 1)
	{
		saveResultsFile();
		// increment total tracked code marker count, so we can compute hit tracked markers percentage
		++hitMarkerCount;
	}
}

}