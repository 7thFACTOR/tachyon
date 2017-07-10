#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/observer.h"
#include "base/mutex.h"

namespace base
{
struct FileChangeNotification
{
	enum ChangeType
	{
		FileCreated,
		FileDeleted,
		FileModified,
		FileOldName,
		FileNewName,
		FileLastWrite,
		FileAttributes,
		FileDirName,
		FileSize,
		FileLastAccess,
		FileSecurity
	};

	ChangeType changeType;
	String filename;
};
	
struct FileChangeObserver
{
	virtual void onFileChangeNotification(const FileChangeNotification& n) {}
};

class B_API FileChangeMonitor : public Observable<FileChangeObserver>
{
public:
	friend class WatchFolderThread;
	FileChangeMonitor();
	~FileChangeMonitor();

	bool watchPath(const String& path, bool recursive);
	void update();

protected:
	Array<class WatchFolderThread*> threads;
	Mutex queueLock;
	Array<FileChangeNotification> notifyQueue;
};

}