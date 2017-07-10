#include "common.h"
#include "sound_processor.h"
#include "base/logger.h"
#include "base/platform.h"
#include "base/util.h"
//#include "resources/sound_buffer.h"

//using namespace engine;
namespace ac
{
SoundProcessor::SoundProcessor()
{
}

SoundProcessor::~SoundProcessor()
{
}

const char* SoundProcessor::supportedFileExtensions() const
{
	return ".mp3 .ogg .wav .wma";
}

AssetProcessor::EType SoundProcessor::type() const
{
	return eType_Compiler;
}

bool SoundProcessor::process(const char* pSrcFilename, const char* pDestPath, const ArgsParser& rArgs)
{
	String filename, ext;
	FileWriter file;

	extractFileNameNoExtension(pSrcFilename, filename);
	extractFileExtension(pSrcFilename, ext);
	filename = mergePathFile(pDestPath, (filename + ".sound").c_str());

	if (isFilesLastTimeSame(pSrcFilename, filename.c_str()))
	{
		B_LOG_DEBUG("Skipping %s", pSrcFilename);
		return true;
	}

	if (!file.openFile(filename.c_str()))
	{
		return false;
	}

	//file.beginChunk(eSoundChunk_Header, 1);
	file.writeString("SOUND");
	file.endChunk();

	FILE* pFile = fopen(pSrcFilename, "rb");

	if (!pFile)
	{
		B_LOG_ERROR("Could not open sound file: %s", pSrcFilename);
		return false;
	}

	fseek(pFile, 0, SEEK_END);
	u64 size = ftell(pFile);
	fseek(pFile, SEEK_SET, 0);

	if (!size)
	{
		B_LOG_ERROR("Empty sound file: %s", pSrcFilename);
		fclose(pFile);
		return false;
	}

	u8* pData = new u8[size];
	
	fread(pData, size, 1, pFile);
	fclose(pFile);

	//file.beginChunk(eSoundChunk_Data, 1);
	file.writeString(ext.c_str());
	file.writeInt64(size);
	E_LOG_VERBOSE("Writing sound file data: %s %d bytes", pSrcFilename, size);
	file.write(pData, size);
	delete [] pData;
	file.endChunk();
	file.closeFile();
	setSameFileTime(pSrcFilename, filename.c_str());

	return true;
}
}