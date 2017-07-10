#include "common.h"
#include "sprite_processor.h"
#include "base/logger.h"
#include "base/platform.h"
#include "base/util.h"

//using namespace engine;
namespace ac
{
enum ESpriteChunkIds
{
	eSpriteChunk_Header = 1,
};

SpriteProcessor::SpriteProcessor()
{
}

SpriteProcessor::~SpriteProcessor()
{
}

const char* SpriteProcessor::supportedFileExtensions() const
{
	return ".sprite";
}

AssetProcessor::EType SpriteProcessor::type() const
{
	return eType_Compiler;
}

bool SpriteProcessor::process(const char* pSrcFilename, const char* pDestPath, const ArgsParser& rArgs)
{
	String filename;
	FileWriter file;

	extractFileName(pSrcFilename, filename);
	filename = mergePathFile(pDestPath, filename.c_str());

	file.openFile(filename.c_str());

	file.beginChunk(eSpriteChunk_Header, 1);
	file.writeString("SPRITE");
	file.endChunk();

	file.closeFile();

	return true;
}
}