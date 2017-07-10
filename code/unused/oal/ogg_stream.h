#ifndef __ogg_h__
#define __ogg_h__

#include <string>
#include <iostream>
using namespace std;

#include <al\al.h>
#include "ogg.h"
#include <vorbis/vorbisfile.h>

/************************************************************************************************************************
    Here is our data struct, that contains the information we need about the file in memory
************************************************************************************************************************/
struct SOggFile
{
	char*       dataPtr;            // Pointer to the data in memoru
	int         dataSize;           // Size of the data
	int         dataRead;           // How much data we have read so far
};

#define BUFFER_SIZE (4096 * 4)

class ogg_stream
{
public:

	void open(string path);
	void release();
	void display();
	bool playback();
	bool playing();
	bool update();

protected:

	bool stream(ALuint buffer);
	void empty();
	void check();
	string errorString(int code);

private:

	SOggFile        oggMemoryFile;      // Data on the ogg file in memory
	ov_callbacks    vorbisCallbacks;    // callbacks used to read the file from mem

	OggVorbis_File  oggStream;
	vorbis_info*    vorbisInfo;
	vorbis_comment* vorbisComment;

	ALuint buffers[2];
	ALuint source;
	ALenum format;
};

#endif // __ogg_h__
