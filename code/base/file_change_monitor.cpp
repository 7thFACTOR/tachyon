#include "base/file_change_monitor.h"
#include "base/thread.h"
#include "base/logger.h"
#include "base/util.h"
#include "base/platform.h"

#ifdef _WINDOWS
#include <windows.h>

namespace base
{
class WatchFolderThread : public Thread
{
public:
	void onRun() override
	{
		hDir = CreateFile(
			pathToWatch.c_str(),
			FILE_LIST_DIRECTORY,
			FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS, NULL);

		if (hDir == INVALID_HANDLE_VALUE)
		{
			B_LOG_ERROR("Could not open directory for watching changes " << pathToWatch);
			return;
		}

		char dataBuffer[1024] = { 0 };
		char* buffer = dataBuffer;
		DWORD BytesReturned = 0;

		while (ReadDirectoryChangesW(
			hDir, /* handle to directory */
			dataBuffer, /* read results buffer */
			sizeof(dataBuffer), /* length of buffer */
			TRUE, 
			FILE_NOTIFY_CHANGE_LAST_WRITE
			| FILE_NOTIFY_CHANGE_FILE_NAME
			| FILE_NOTIFY_CHANGE_DIR_NAME
			| FILE_NOTIFY_CHANGE_SIZE
			| FILE_NOTIFY_CHANGE_CREATION,
			&BytesReturned, /* bytes returned */
			NULL, /* overlapped buffer */
			NULL))
		{
			FILE_NOTIFY_INFORMATION* fni;
			buffer = dataBuffer;
			u64 nextEntryOffset = 0;

			if (BytesReturned)
			do {
				fni = (FILE_NOTIFY_INFORMATION*)buffer;
				char* bufferWString = new char[fni->FileNameLength + 2];
				bufferWString[fni->FileNameLength] = 0;
				bufferWString[fni->FileNameLength + 1] = 0;
				memcpy(bufferWString, fni->FileName, fni->FileNameLength);
				String filename = stringFromUtf16((wchar_t*)bufferWString);

				delete [] bufferWString;

				{
					FileChangeMonitor* fcm = (FileChangeMonitor*)threadUserData;
					AutoLock<Mutex> autoLock(fcm->queueLock);
					FileChangeNotification n;

					switch (fni->Action)
					{
					case FILE_ACTION_ADDED:
						n.changeType = FileChangeNotification::FileCreated;
						break;
					case FILE_ACTION_REMOVED:
						n.changeType = FileChangeNotification::FileDeleted;
						break;
					case FILE_ACTION_MODIFIED:
						n.changeType = FileChangeNotification::FileModified;
						break;
					case FILE_ACTION_RENAMED_OLD_NAME:
						n.changeType = FileChangeNotification::FileOldName;
						break;
					case FILE_ACTION_RENAMED_NEW_NAME:
						n.changeType = FileChangeNotification::FileNewName;
						break;
					}

					B_LOG_DEBUG("File " << filename << " " << (u32)fni->Action);

					n.filename = filename;
					fcm->notifyQueue.append(n);
				}

				nextEntryOffset = fni->NextEntryOffset;
				buffer += nextEntryOffset;
			} while (nextEntryOffset);
		}
	}

	HANDLE hDir;
	String pathToWatch;
	bool watchSubtree;
};

FileChangeMonitor::FileChangeMonitor()
{

}

FileChangeMonitor::~FileChangeMonitor()
{
	for (auto thread : threads)
	{
		thread->stop(true, 1);
		thread->close();
		CloseHandle(thread->hDir);
		delete thread;
	}
}

bool FileChangeMonitor::watchPath(const String& path, bool watchSubtree)
{
	WatchFolderThread* thread = new WatchFolderThread();

	thread->pathToWatch = path;
	threads.append(thread);
	thread->create(this, true, "FileChangeMonitor");

	return true;
}

void FileChangeMonitor::update()
{
	AutoLock<Mutex> autoLock(queueLock);

	for (auto& n : notifyQueue)
	{
		B_CALL_OBSERVERS_METHOD(onFileChangeNotification(n));
	}

	notifyQueue.clear();
}

}

#endif