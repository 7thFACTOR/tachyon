#include "common.h"
#include "prefab_processor.h"
#include "base/logger.h"
#include "base/platform.h"
#include "base/util.h"

//using namespace engine;
namespace ac
{
enum EPrefabChunkIds
{
	ePrefabChunk_Header = 1,
};

PrefabProcessor::PrefabProcessor()
{
}

PrefabProcessor::~PrefabProcessor()
{
}

const char* PrefabProcessor::supportedFileExtensions() const
{
	return ".prefab";
}

AssetProcessor::EType PrefabProcessor::type() const
{
	return eType_Compiler;
}

bool PrefabProcessor::process(const char* pSrcFilename, const char* pDestPath, const ArgsParser& rArgs)
{
	String filename;
	FileWriter file;

	extractFileName(pSrcFilename, filename);
	filename = mergePathFile(pDestPath, filename.c_str());

	file.openFile(filename.c_str());

	file.beginChunk(ePrefabChunk_Header, 1);
	file.writeString("PREFAB");
	file.endChunk();

	file.closeFile();

	return true;
}
}