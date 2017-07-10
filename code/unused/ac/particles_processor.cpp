#include "common.h"
#include "particles_processor.h"
#include "base/logger.h"
#include "base/platform.h"
#include "base/util.h"

namespace ac
{
enum EParticlesChunkIds
{
	eParticlesChunk_Header = 1,
};

ParticlesProcessor::ParticlesProcessor()
{
}

ParticlesProcessor::~ParticlesProcessor()
{
}

const char* ParticlesProcessor::supportedFileExtensions() const
{
	return ".particles";
}

AssetProcessor::EType ParticlesProcessor::type() const
{
	return eType_Compiler;
}

bool ParticlesProcessor::process(const char* pSrcFilename, const char* pDestPath, const ArgsParser& rArgs)
{
	String filename;
	FileWriter file;

	extractFileName(pSrcFilename, filename);
	filename = mergePathFile(pDestPath, filename.c_str());

	file.openFile(filename.c_str());

	file.beginChunk(eParticlesChunk_Header, 1);
	file.writeString("PARTICLES");
	file.endChunk();

	file.closeFile();

	return true;
}
}