#include "common.h"
#include "model_processor.h"
#include "base/logger.h"
#include "base/platform.h"
#include "base/util.h"
#include "base/json.h"
//#include "resources/model.h"

//using namespace engine;
namespace ac
{
ModelProcessor::ModelProcessor()
{
}

ModelProcessor::~ModelProcessor()
{
}

const char* ModelProcessor::supportedFileExtensions() const
{
	return ".model";
}

AssetProcessor::EType ModelProcessor::type() const
{
	return eType_Compiler;
}

bool ModelProcessor::process(const char* pSrcFilename, const char* pDestPath, const ArgsParser& rArgs)
{
	String filename;
	FileWriter file;
	JsonDocument doc;
	String str;

	if (!doc.loadAndParse(pSrcFilename))
	{
		return false;
	}

	extractFileName(pSrcFilename, filename);
	filename = mergePathFile(pDestPath, filename.c_str());

	if (isFilesLastTimeSame(pSrcFilename, filename.c_str()))
	{
		B_LOG_DEBUG("Skipping %s", pSrcFilename);
		return true;
	}

	file.openFile(filename.c_str());

	//file.beginChunk(eModelChunk_Header, 1);
	file.writeString("MODEL");
	str = doc.findStringValue("physicsMaterial");
	file.writeString(str.c_str());
	file.endChunk();

	//file.beginChunk(eModelChunk_Meshes, 1);
	JsonNode* jsnMeshes = doc.findNodeByPath("meshes");

	if (!jsnMeshes)
	{
		return false;
	}

	file.writeInt16(jsnMeshes->arrayValues().size());

	for (size_t i = 0; i < jsnMeshes->arrayValues().size(); ++i)
	{
		JsonNode* jsnMesh = jsnMeshes->arrayValues().at(i)->asNode();

		if (jsnMesh)
		{
			file.writeString(jsnMesh->valueOf("name").asString());
			str = jsnMesh->valueOf("file").asString();
			file.writeString(str.c_str());
		}
	}

	file.endChunk();

//	file.beginChunk(eModelChunk_LODs, 1);
	JsonNode* jsnLods = doc.findNodeByPath("lods");

	if (!jsnLods)
	{
		return false;
	}

	file.writeInt16(jsnLods->arrayValues().size());

	for (size_t i = 0; i < jsnLods->arrayValues().size(); ++i)
	{
		JsonNode* jsnLod = jsnLods->arrayValues().at(i)->asNode();

		if (jsnLod)
		{
			file.writeFloat(jsnLod->valueOf("fromDistance").asFloat());
			file.writeString(jsnLod->valueOf("mesh").asString());

			JsonNode* jsnParts = jsnLod->valueOf("parts").asNode();

			file.writeInt16(jsnParts ? jsnParts->arrayValues().size() : 0);

			if (jsnParts)
			{
				for (size_t j = 0; j < jsnParts->arrayValues().size(); ++j)
				{
					JsonNode* jsnPart = jsnParts->arrayValues().at(j)->asNode();

					if (jsnPart)
					{
						file.writeString(jsnPart->valueOf("name").asString());
						str = jsnPart->valueOf("defaultMaterial").asString();
						file.writeString(str.c_str());

						JsonNode* jsnClusters = jsnPart->valueOf("clusters").asNode();

						file.writeInt16(jsnClusters ? jsnClusters->arrayValues().size() : 0);

						if (jsnClusters)
						{
							for (size_t k = 0; k < jsnClusters->arrayValues().size(); ++k)
							{
								JsonNode* jsnCluster = jsnClusters->arrayValues().at(k)->asNode();

								if (jsnCluster)
								{
									str = jsnCluster->valueOf("material").asString();
									file.writeString(str.c_str());
								}
							}
						}
					}
				}
			}
		}
	}

	file.endChunk();
	file.closeFile();
	setSameFileTime(pSrcFilename, filename.c_str());

	return true;
}
}