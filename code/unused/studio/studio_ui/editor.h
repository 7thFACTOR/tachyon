#pragma once
#include "../api/plugin.h"
#include <string>
#include <vector>
#include <map>
#include <QThread>
#include <QMutex>
#define NOMINMAX
#include <enet/enet.h>

namespace editor
{
struct PluginGuid;
struct PluginGuidArray;

typedef unsigned int CommandUid;

/// A version struct, holding version information for plugin or editor
struct Version
{
	Version()
		: major(0)
		, minor(0)
		, build(0)
	{
	}

	Version(int aMajor, int aMinor, int aBuild)
		: major(aMajor)
		, minor(aMinor)
		, build(aBuild)
	{
	}

	bool operator <= (const Version& rOther) const
	{
		int ver = major*100 + minor*10 + build;
		int otherVer = rOther.major*100 + rOther.minor*10 + rOther.build;

		return (ver <= otherVer);
	}

	bool operator >= (const Version& rOther) const
	{
		int ver = major*100 + minor*10 + build;
		int otherVer = rOther.major*100 + rOther.minor*10 + rOther.build;

		return (ver >= otherVer);
	}

	Version& operator = (const char* pVerStr)
	{
		if (pVerStr)
		{
			sscanf_s(pVerStr, "%d.%d.%d", &major, &minor, &build);
		}

		return *this;
	}

	int major, minor, build;
};

/// A plugin unique ID, in a GUID form (see more about Microsoft GUID) and online/offline GUID generators
struct PluginGuid
{
	PluginGuid()
		: data1(0)
		, data2(0)
		, data3(0)
	{
		memset(data4, 0, 8);
	}

	/// construct the guid from several elements/parts
	/// example:
	/// as text: {31D9B906-6125-4784-81FF-119C15267FCA}
	/// as C++: 0x31d9b906, 0x6125, 0x4784, 0x81, 0xff, 0x11, 0x9c, 0x15, 0x26, 0x7f, 0xca
	PluginGuid(
		unsigned int a, unsigned short b, unsigned short c,
		unsigned char d, unsigned char e, unsigned char f, unsigned char g,
		unsigned char h, unsigned char i, unsigned char j, unsigned char k)
		: data1(a)
		, data2(b)
		, data3(c)
	{
		data4[0] = d;
		data4[1] = e;
		data4[2] = f;
		data4[3] = g;
		data4[4] = h;
		data4[5] = i;
		data4[6] = j;
		data4[7] = k;
	}

	bool operator == (const PluginGuid& rOther) const
	{
		return (data1 == rOther.data1
				&& data2 == rOther.data2
				&& data3 == rOther.data3 
				&& !memcmp(data4, rOther.data4, 8));
	}

	unsigned int data1;
	unsigned short data2;
	unsigned short data3;
	unsigned char data4[8];
};

/// This array is used to specify plug-in dependencies, must-have and optional,
/// filled with GUIDs of plug-ins that the user must have and optional
struct PluginGuidArray
{
	PluginGuidArray()
		: pMustHaveGuids(NULL)
		, pOptionalGuids(NULL)
		, mustHaveGuidCount(0)
		, optionalGuidCount(0)
	{
	}
	
	PluginGuidArray( 
		const PluginGuid* apMustHaveGuids,
		const PluginGuid* apOptionalGuids,
		size_t aMustHaveGuidCount,
		size_t aOptionalGuidCount)
		: pMustHaveGuids(apMustHaveGuids)
		, pOptionalGuids(apOptionalGuids)
		, mustHaveGuidCount(aMustHaveGuidCount)
		, optionalGuidCount(aOptionalGuidCount)
	{
	}
	
	const PluginGuid* pMustHaveGuids;
	const PluginGuid* pOptionalGuids;
	size_t mustHaveGuidCount;
	size_t optionalGuidCount;
};

struct PluginInfo
{
	PluginInfo()
		: hLibHandle(nullptr)
		, pInstance(nullptr)
		, bInvalidVersion(true)
	{}
	
	void* hLibHandle;
	IEditorPlugin* pInstance;
	bool bInvalidVersion;
	PluginGuid guid;
	std::string name, description, author, url, guidStr, iconFilename;
	bool bUnloadable;
	Version version, minEditorVersion, maxEditorVersion;
	PluginGuidArray dependencies;
};

class EditorClientThread: public QThread
{
	Q_OBJECT
public:
	EditorClientThread(QObject* pParent = nullptr)
	: QThread(pParent)
	{
		m_pClient = nullptr;
		m_pPeer = nullptr;
		m_bStopped = false;
		moveToThread(this);
	}
	void run();
	CommandUid queueCommandCall(const char* pCmd, CmdArgs& rArgs);
	bool waitForCommandReturnValues(CommandUid aCmdId, CmdArgs& rReturnValues, int waitMsec = 0);

	ENetHost* m_pClient;
	ENetPeer* m_pPeer;
	bool m_bStopped;

protected:
	bool m_bConnected;
	QMutex m_lock;
	std::vector<std::string> m_commandsToSend;
	std::map<CommandUid, std::string> m_receivedReturnValues;
};

class Editor: public IEditor
{
public:
	typedef std::map<std::string, PluginInfo*> TPluginMap;

	Editor();
	~Editor();

	QMainWindow* mainWindow();
	bool call(const char* pCmdName, CmdArgs& rArgs = CmdArgs(), CmdArgs& rReturnValues = CmdArgs());
	void loadPluginsFromFolder(const char* pFolder);
	void loadPluginsFromFolderRecursive(const char* pFolder);
	bool loadPlugin(const char* pFilename);
	void unloadPlugin(const char* pFilename);
	void unloadAllPlugins();
	PluginInfo* findPluginByGuid(const PluginGuid& rGuid);
	void enablePlugin(const PluginGuid& rGuid, bool bEnable);
	QString readAllTextFromFile(const char* pFilename);
	bool connectToEngine();
	bool disconnectFromEngine();

	TPluginMap m_plugins;
	QMainWindow* m_pMainWnd;
	Version m_version;
	ENetHost* m_pClient;
	EditorClientThread m_clientThread;
};

extern Editor ed;

#define CALLCMD(...) editor::ed.call(__VA_ARGS__);
}