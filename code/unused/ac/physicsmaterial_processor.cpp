#include "common.h"
#include "physicsmaterial_processor.h"
#include "base/logger.h"
#include "base/platform.h"
#include "base/util.h"

//using namespace engine;
namespace ac
{
enum EPhysicsMaterialChunkIds
{
	ePhysicsMaterialChunk_Header = 1,
};

PhysicsMaterialProcessor::PhysicsMaterialProcessor()
{
}

PhysicsMaterialProcessor::~PhysicsMaterialProcessor()
{
}

const char* PhysicsMaterialProcessor::supportedFileExtensions() const
{
	return ".physmtl";
}

AssetProcessor::EType PhysicsMaterialProcessor::type() const
{
	return eType_Compiler;
}

bool PhysicsMaterialProcessor::process(const char* pSrcFilename, const char* pDestPath, const ArgsParser& rArgs)
{
	String filename;
	FileWriter file;

	extractFileName(pSrcFilename, filename);
	filename = mergePathFile(pDestPath, filename.c_str());

	file.openFile(filename.c_str());

	file.beginChunk(ePhysicsMaterialChunk_Header, 1);
	file.writeString("PHYSMTL");
	file.endChunk();

	file.closeFile();

	return true;
}
}