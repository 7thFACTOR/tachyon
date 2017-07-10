#include "common.h"
#include "section_processor.h"
#include "base/logger.h"
#include "base/platform.h"
#include "base/util.h"

//using namespace engine;
namespace ac
{
enum ESectionChunkIds
{
	eSectionChunk_Header = 1,
};

SectionProcessor::SectionProcessor()
{
}

SectionProcessor::~SectionProcessor()
{
}

const char* SectionProcessor::supportedFileExtensions() const
{
	return ".section";
}

AssetProcessor::EType SectionProcessor::type() const
{
	return eType_Compiler;
}

bool SectionProcessor::process(const char* pSrcFilename, const char* pDestPath, const ArgsParser& rArgs)
{
	String filename;
	FileWriter file;

	extractFileName(pSrcFilename, filename);
	filename = mergePathFile(pDestPath, filename.c_str());

	file.openFile(filename.c_str());

	file.beginChunk(eSectionChunk_Header, 1);
	file.writeString("SECTION");
	file.endChunk();

	file.closeFile();

	return true;
}
}