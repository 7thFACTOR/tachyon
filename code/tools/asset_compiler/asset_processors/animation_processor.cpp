// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "../common.h"
#include "base/math/vec3.h"
#include "base/util.h"
#include "base/logger.h"
#include "base/file.h"
#include "base/json.h"
#include "animation_processor.h"
#include "../asset_compiler.h"
#include "core/resources/animation_resource.h"
#include "graphics/types.h"
#include "../project.h"

namespace ac
{
using namespace base;
using namespace engine;

AnimationProcessor::AnimationProcessor()
{
	Array<String> extensions;
	
	extensions.append(".animation");
	supportedAssetType = SupportedAssetType(ResourceType::Animation, extensions);
}

AnimationProcessor::~AnimationProcessor()
{}

bool AnimationProcessor::process(Asset& asset, JsonDocument& assetCfg)
{
	File file;

	if (!file.open(asset.deployFilename, FileOpenFlags::BinaryWrite))
		return false;

	JsonDocument srcDoc(asset.absFilename);

	auto jsonLayers = srcDoc.getArray("layers");

	file << srcDoc.getF32("startTime");
	file << srcDoc.getF32("endTime");
	file << (u32)jsonLayers->size();

	for (size_t i = 0; i < jsonLayers->size(); i++)
	{
		file << jsonLayers->at(i)->asObject()->getF32("weight");
		auto jsonNodes = jsonLayers->at(i)->asObject()->getArray("nodes");
		file << (u32)jsonNodes->size();

		for (size_t j = 0; j < jsonNodes->size(); j++)
		{
			file << jsonNodes->at(j)->asObject()->getString("name");

			auto writeCurve = [](File& file, JsonObject* jsonCurve, AnimationCurveType curveType)
			{
				file << (u32)curveType;
				file << (u32)jsonCurve->getArray("keys")->size();

				for (size_t k = 0; k < jsonCurve->getArray("keys")->size(); k++)
				{
					file << jsonCurve->getArray("keys")->at(k)->asObject()->getF32("time");
					file << jsonCurve->getArray("keys")->at(k)->asObject()->getF32("value");
				}
			};

			auto jsonTranslationX = jsonNodes->at(j)->asObject()->getObject("translation.x");
			auto jsonTranslationY = jsonNodes->at(j)->asObject()->getObject("translation.y");
			auto jsonTranslationZ = jsonNodes->at(j)->asObject()->getObject("translation.z");
			auto jsonRotationX = jsonNodes->at(j)->asObject()->getObject("rotation.x");
			auto jsonRotationY = jsonNodes->at(j)->asObject()->getObject("rotation.y");
			auto jsonRotationZ = jsonNodes->at(j)->asObject()->getObject("rotation.z");
			auto jsonScaleX = jsonNodes->at(j)->asObject()->getObject("scale.x");
			auto jsonScaleY = jsonNodes->at(j)->asObject()->getObject("scale.y");
			auto jsonScaleZ = jsonNodes->at(j)->asObject()->getObject("scale.z");
			auto jsonVisibility = jsonNodes->at(j)->asObject()->getObject("visibility");

			u32 numCurves = 0;

			if (jsonTranslationX) numCurves++;
			if (jsonTranslationY) numCurves++;
			if (jsonTranslationZ) numCurves++;
			if (jsonRotationX) numCurves++;
			if (jsonRotationY) numCurves++;
			if (jsonRotationZ) numCurves++;
			if (jsonScaleX) numCurves++;
			if (jsonScaleY) numCurves++;
			if (jsonScaleZ) numCurves++;
			if (jsonVisibility) numCurves++;

			file << numCurves;
			
			if (jsonTranslationX)
				writeCurve(file, jsonTranslationX, AnimationCurveType::TranslationX);

			if (jsonTranslationY)
				writeCurve(file, jsonTranslationY, AnimationCurveType::TranslationY);

			if (jsonTranslationZ)
				writeCurve(file, jsonTranslationZ, AnimationCurveType::TranslationZ);

			if (jsonRotationX)
				writeCurve(file, jsonRotationX, AnimationCurveType::RotationX);

			if (jsonRotationY)
				writeCurve(file, jsonRotationY, AnimationCurveType::RotationY);

			if (jsonRotationZ)
				writeCurve(file, jsonRotationZ, AnimationCurveType::RotationZ);

			if (jsonScaleX)
				writeCurve(file, jsonScaleX, AnimationCurveType::ScaleX);

			if (jsonScaleY)
				writeCurve(file, jsonScaleY, AnimationCurveType::ScaleY);

			if (jsonScaleZ)
				writeCurve(file, jsonScaleZ, AnimationCurveType::ScaleZ);

			if (jsonVisibility)
				writeCurve(file, jsonVisibility, AnimationCurveType::Visibility);
		}
	}

	file.close();
	B_LOG_DEBUG("Done writing animation.");

	return true;
}

}