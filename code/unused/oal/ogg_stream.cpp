#include "common.h"
#include "ogg_stream.h"

//---------------------------------------------------------------------------------
// Function : VorbisRead
// Purpose  : Callback for the Vorbis read function
// Info     :
//---------------------------------------------------------------------------------

size_t VorbisRead(void *ptr            /* ptr to the data that the vorbis files need*/,
				  size_t byteSize      /* how big a byte is*/,
				  size_t sizeToRead    /* How much we can read*/,
				  void *datasource     /* this is a pointer to the data we passed into ov_open_callbacks (our SOggFile struct*/)
{
	size_t              spaceToEOF;         // How much more we can read till we hit the EOF marker
	size_t              actualSizeToRead;   // How much data we are actually going to read from memory
	SOggFile*           vorbisData;         // Our vorbis data, for the typecast

	// Get the data in the right format
	vorbisData = (SOggFile*)datasource;

	// Calculate how much we need to read.  This can be sizeToRead*byteSize or less depending on how near the EOF marker we are
	spaceToEOF = vorbisData->dataSize - vorbisData->dataRead;

	if ((sizeToRead * byteSize) < spaceToEOF)
		actualSizeToRead = (sizeToRead * byteSize);
	else
		actualSizeToRead = spaceToEOF;

	// A simple copy of the data from memory to the datastruct that the vorbis libs will use
	if (actualSizeToRead)
	{
		// Copy the data from the start of the file PLUS how much we have already read in
		memcpy(ptr, (char*)vorbisData->dataPtr + vorbisData->dataRead, actualSizeToRead);
		// Increase by how much we have read by
		vorbisData->dataRead += (actualSizeToRead);
	}

	// Return how much we read (in the same way fread would)
	return actualSizeToRead;
}

//---------------------------------------------------------------------------------
// Function : VorbisSeek
// Purpose  : Callback for the Vorbis seek function
// Info     :
//---------------------------------------------------------------------------------
int VorbisSeek(void *datasource     /*this is a pointer to the data we passed into ov_open_callbacks (our SOggFile struct*/,
			   ogg_int64_t offset   /*offset from the point we wish to seek to*/,
			   int whence           /*where we want to seek to*/)
{
	size_t              spaceToEOF;     // How much more we can read till we hit the EOF marker
	ogg_int64_t         actualOffset;   // How much we can actually offset it by
	SOggFile*           vorbisData;     // The data we passed in (for the typecast)

	// Get the data in the right format
	vorbisData = (SOggFile*)datasource;

	// Goto where we wish to seek to
	switch (whence)
	{
	case SEEK_SET: // Seek to the start of the data file
		// Make sure we are not going to the end of the file
		if (vorbisData->dataSize >= offset)
			actualOffset = offset;
		else
			actualOffset = vorbisData->dataSize;
		// Set where we now are
		vorbisData->dataRead = (int)actualOffset;
		break;
	case SEEK_CUR: // Seek from where we are
		// Make sure we dont go past the end
		spaceToEOF = vorbisData->dataSize - vorbisData->dataRead;
		if (offset < spaceToEOF)
			actualOffset = (offset);
		else
			actualOffset = spaceToEOF;
		// Seek from our currrent location
		vorbisData->dataRead += actualOffset;
		break;
	case SEEK_END: // Seek from the end of the file
		vorbisData->dataRead = vorbisData->dataSize + 1;
		break;
	default:
		printf("*** ERROR *** Unknown seek command in VorbisSeek\n");
		break;
	};

	return 0;
}

//---------------------------------------------------------------------------------
// Function : VorbisClose
// Purpose  : Callback for the Vorbis close function
// Info     :
//---------------------------------------------------------------------------------
int VorbisClose(void *datasource /*this is a pointer to the data we passed into ov_open_callbacks (our SOggFile struct*/)
{
	// This file is called when we call ov_close.  If we wanted, we could free our memory here, but
	// in this case, we will free the memory in the main body of the program, so dont do anything
	return 1;
}

//---------------------------------------------------------------------------------
// Function : VorbisTell
// Purpose  : Classback for the Vorbis tell function
// Info     :
//---------------------------------------------------------------------------------
long VorbisTell(void *datasource /*this is a pointer to the data we passed into ov_open_callbacks (our SOggFile struct*/)
{
	SOggFile*   vorbisData;

	// Get the data in the right format
	vorbisData = (SOggFile*)datasource;

	// We just want to tell the vorbis libs how much we have read so far
	return vorbisData->dataRead;
}
/************************************************************************************************************************
    End of Vorbis callback functions
************************************************************************************************************************/

void ogg_stream::open(void*)
{
	/************************************************************************************************************************
	    Heres a total bodge, just to get the file into memory.  Normally, the file would have been loaded into memory
	    for a specific reason e.g. loading it from a pak file or similar.  I just want to get the file into memory for
	    the sake of the tutorial.
	************************************************************************************************************************/



	alGenBuffers(2, buffers);
	check();
	alGenSources(1, &source);
	check();

	alSource3f(source, AL_POSITION,        0.0, 0.0, 0.0);
	alSource3f(source, AL_VELOCITY,        0.0, 0.0, 0.0);
	alSource3f(source, AL_DIRECTION,       0.0, 0.0, 0.0);
	alSourcef(source, AL_ROLLOFF_FACTOR,  0.0);
	alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);
}

void ogg_stream::release()
{
	alSourceStop(source);
	empty();
	alDeleteSources(1, &source);
	check();
	alDeleteBuffers(1, buffers);
	check();

	ov_clear(&oggStream);

	delete[] oggMemoryFile.dataPtr;
	oggMemoryFile.dataPtr = NULL;
}

void ogg_stream::display()
{
	cout
			<< "version         " << vorbisInfo->version         << "\n"
			<< "channels        " << vorbisInfo->channels        << "\n"
			<< "rate (hz)       " << vorbisInfo->rate            << "\n"
			<< "bitrate upper   " << vorbisInfo->bitrate_upper   << "\n"
			<< "bitrate nominal " << vorbisInfo->bitrate_nominal << "\n"
			<< "bitrate lower   " << vorbisInfo->bitrate_lower   << "\n"
			<< "bitrate window  " << vorbisInfo->bitrate_window  << "\n"
			<< "\n"
			<< "vendor " << vorbisComment->vendor << "\n";

	for (int i = 0; i < vorbisComment->comments; i++)
		cout << "   " << vorbisComment->user_comments[i] << "\n";

	cout << endl;
}




bool ogg_stream::playback()
{
	if (playing())
		return true;

	if (!stream(buffers[0]))
		return false;

	if (!stream(buffers[1]))
		return false;

	alSourceQueueBuffers(source, 2, buffers);
	alSourcePlay(source);

	return true;
}




bool ogg_stream::playing()
{
	ALenum state;

	alGetSourcei(source, AL_SOURCE_STATE, &state);

	return (state == AL_PLAYING);
}




bool ogg_stream::update()
{
	int processed;
	bool active = true;

	alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);

	while (processed--)
	{
		ALuint buffer;

		alSourceUnqueueBuffers(source, 1, &buffer);
		check();

		active = stream(buffer);

		alSourceQueueBuffers(source, 1, &buffer);
		check();
	}

	return active;
}

bool ogg_stream::stream(ALuint buffer)
{
	char pcm[BUFFER_SIZE];
	int  size = 0;
	int  section;
	int  result;

	while (size < BUFFER_SIZE)
	{
		result = ov_read(&oggStream, pcm + size,
						 BUFFER_SIZE - size, 0, 2, 1, &section);

		if (result > 0)
			size += result;
		else if (result < 0)
			throw errorString(result);
		else
			break;
	}

	if (size == 0)
		return false;

	alBufferData(buffer, format, pcm, size, vorbisInfo->rate);
	check();

	return true;
}




void ogg_stream::empty()
{
	int queued;

	alGetSourcei(source, AL_BUFFERS_QUEUED, &queued);

	while (queued--)
	{
		ALuint buffer;

		alSourceUnqueueBuffers(source, 1, &buffer);
		check();
	}
}




void ogg_stream::check()
{
	int error = alGetError();

	if (error != AL_NO_ERROR)
		throw string("OpenAL error was raised.");
}



string ogg_stream::errorString(int code)
{
	switch (code)
	{
	case OV_EREAD:
		return string("Read from media.");
	case OV_ENOTVORBIS:
		return string("Not Vorbis data.");
	case OV_EVERSION:
		return string("Vorbis version mismatch.");
	case OV_EBADHEADER:
		return string("Invalid Vorbis header.");
	case OV_EFAULT:
		return string("Internal logic fault (bug or heap/stack corruption.");
	default:
		return string("Unknown Ogg error.");
	}
}
