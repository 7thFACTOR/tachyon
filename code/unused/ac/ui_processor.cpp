#include "common.h"
#include "ui_processor.h"
#include "base/logger.h"
#include "base/platform.h"
#include "base/util.h"

namespace ac
{
enum EUiChunkIds
{
	eUiChunk_Header = 1,
};

UiProcessor::UiProcessor()
{
}

UiProcessor::~UiProcessor()
{
}

const char* UiProcessor::supportedFileExtensions() const
{
	return ".ui";
}

bool UiProcessor::process(const char* pSrcFilename, Stream* pStream, const Project& project)
{
	String filename;
	//FileWriter file;

	//extractFileName(pSrcFilename, filename);
	//filename = mergePathFile(pDestPath, filename.c_str());

	//file.openFile(filename.c_str());

	//file.beginChunk(eUiChunk_Header, 1);
	//file.writeString("UI");
	//file.endChunk();

	//file.closeFile();

	return true;
}
}