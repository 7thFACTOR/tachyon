#include "stdafx.h"
#include "editor.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QVariant>
#include <QElapsedTimer>
#include <QStringList>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

namespace editor
{
Editor ed;
const char* kPluginInfoExtension = ".plugin";

void EditorClientThread::run()
{
	ENetEvent event;
	m_bConnected = false;
	
	while (!m_bStopped)
	{
		int res = enet_host_service(m_pClient, &event, 0);

		if (res > 0 &&
			event.type == ENET_EVENT_TYPE_CONNECT)
		{
			m_bConnected = true;
		}

		if (event.type == ENET_EVENT_TYPE_RECEIVE)
		{
			m_lock.lock();
			std::string cmdJson = (char*)event.packet->data;
			QJsonDocument doc = QJsonDocument::fromJson(cmdJson.c_str());
			QVariantMap result = doc.object().toVariantMap();
			
			m_receivedReturnValues[result["cmdId"].toInt()] = cmdJson;
			m_lock.unlock();
		}

		if (m_bConnected)
		{
			m_lock.lock();
			std::string cmd;

			if (!m_commandsToSend.empty())
			{
				cmd = m_commandsToSend[0];
				m_commandsToSend.erase(m_commandsToSend.begin());
			}

			m_lock.unlock();

			if (!cmd.empty())
			{
				ENetPacket* packet = enet_packet_create(
					cmd.c_str(),
					cmd.size() + 1,
					ENET_PACKET_FLAG_RELIABLE);
				enet_peer_send(m_pPeer, 0, packet);
			}
		}

		msleep(1);
	}

	enet_peer_reset(m_pPeer);
}

CommandUid EditorClientThread::queueCommandCall(const char* pCmd, CmdArgs& rArgs)
{
	static CommandUid s_cmdId = 0;
	QVariantMap jsonCmd;
	QVariantList jsonArgs;
	
	jsonCmd["cmdId"] = s_cmdId++;
	jsonCmd["cmd"] = pCmd;
	
	for (size_t i = 0; i < rArgs.values.size(); ++i)
	{
		jsonArgs << rArgs.values[i].c_str();
	}
		
	jsonCmd["args"] = jsonArgs;

	QJsonObject obj = QJsonObject::fromVariantMap(jsonCmd);
	QJsonDocument doc = QJsonDocument(obj);
	std::string json = doc.toJson().data();

	m_lock.lock();
	m_commandsToSend.push_back(json);
	m_lock.unlock();

	return s_cmdId;
}

bool EditorClientThread::waitForCommandReturnValues(CommandUid aCmdId, CmdArgs& rReturnValues, int waitMsec)
{
	QString strJson;
	QVariantMap returnVals;
	QElapsedTimer timer;
	timer.start();

	while (false)
	{
		if (m_lock.tryLock())
		{
			auto iter = m_receivedReturnValues.find(aCmdId);
		
			if (iter != m_receivedReturnValues.end())
			{
				strJson = iter->second.c_str();
				m_receivedReturnValues.erase(iter);
				m_lock.unlock();
				break;
			}
			m_lock.unlock();
		}

		if (waitMsec && timer.hasExpired(waitMsec))
		{
			return false;
		}

		msleep(1);
	}

	// lets put the return values in the command args
	QJsonDocument doc = QJsonDocument::fromJson(strJson.toLatin1());
	returnVals = doc.object().toVariantMap();

	rReturnValues.values.clear();

	for (int i = 0; i < returnVals["values"].toStringList().size(); ++i)
	{
		rReturnValues.values.push_back((const char*)returnVals["values"].toStringList()[i].toLatin1());
	}

	return true;
}

Editor::Editor()
{
	m_version.major = 0;
	m_version.minor = 1;
	m_version.build = 1;
	m_pClient = nullptr;
}

Editor::~Editor()
{
}

QMainWindow* Editor::mainWindow()
{
	return m_pMainWnd;
}

bool Editor::call(const char* pCmdName, CmdArgs& rArgs, CmdArgs& rReturnValues)
{
	CmdArgs returnValues;
	CommandUid cmdUid = m_clientThread.queueCommandCall(pCmdName, rArgs);

	if (m_clientThread.waitForCommandReturnValues(cmdUid, returnValues))
	{
		return true;
	}
	
	return false;
}

void Editor::loadPluginsFromFolder(const char* pFolder)
{
	loadPluginsFromFolderRecursive(pFolder);

	// initialize plug-ins
	TPluginMap::iterator iter = m_plugins.begin();

	while(iter != m_plugins.end())
	{
		IEditorPlugin* pPlugin = iter->second->pInstance;
		
		if (pPlugin)
		{
			// check dependencies
			//TODO

			// initialize
			pPlugin->onLoad();
		}

		++iter;
	}
}

void Editor::loadPluginsFromFolderRecursive(const char* pFolder)
{
#if defined(_WINDOWS)
	char strMask[MAX_PATH];
	char strPath[MAX_PATH];
	WIN32_FIND_DATAA fd;
	HANDLE hFind;

	sprintf_s(strMask, MAX_PATH, "%s\\*", pFolder);
	hFind = FindFirstFileA(strMask, &fd);

	if(INVALID_HANDLE_VALUE == hFind)
	{
		return;
	}

	do 
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && !strstr(fd.cFileName, "."))
		{
			sprintf_s(strPath, MAX_PATH, "%s\\%s", pFolder, fd.cFileName);
			loadPluginsFromFolderRecursive(strPath);
		}
		else if (strstr(fd.cFileName, kPluginInfoExtension))
		{
			sprintf_s(strPath, MAX_PATH, "%s\\%s", pFolder, fd.cFileName);
			loadPlugin(strPath);
		}

	} while(FindNextFileA(hFind, &fd));

	FindClose(hFind);
#endif
}

bool Editor::loadPlugin(const char* pFilename)
{
	QString json;

	// load the plugin manifest
	json = readAllTextFromFile(pFilename);
	QVariantMap result;
	QJsonDocument doc = QJsonDocument::fromJson(json.toLatin1());
	result = doc.object().toVariantMap();

	if (result.isEmpty())
	{
		return false;
	}

	PluginInfo* plugin = new PluginInfo();

	plugin->name = result["name"].toString().toLatin1();
	plugin->description = result["description"].toString().toLatin1();
	plugin->author = result["author"].toString().toLatin1();
	plugin->url = result["url"].toString().toLatin1();
	plugin->guidStr = result["guid"].toString().toLatin1();
	plugin->version = result["version"].toString().toLatin1();
	plugin->minEditorVersion = result["minEditorVersion"].toString().toLatin1();
	plugin->maxEditorVersion = result["maxEditorVersion"].toString().toLatin1();
	plugin->bUnloadable = result["unloadable"].toString() == "true";

	//TODO: dependencies

	QString dllFilename = pFilename;

#ifdef _DEBUG
	#define DBG_POSTFIX "_d"
#else
	#define DBG_POSTFIX ""
#endif

#ifdef _WINDOWS_
	dllFilename.replace(kPluginInfoExtension, DBG_POSTFIX".dll");
	HMODULE hModule = LoadLibraryA(dllFilename.toLatin1());
	TPfnCreatePluginInstance pfnCreatePluginInstance = (TPfnCreatePluginInstance)GetProcAddress(hModule, "createPluginInstance");
#else
	dllFilename.replace(kPluginInfoExtension, DBG_POSTFIX".so");
	HMODULE hModule = dlopen(dllFilename.toLatin1());
	TPfnCreatePluginInstance pfnCreatePluginInstance = (TPfnCreatePluginInstance)dlsym(hModule, "createPluginInstance");
#endif

	if(pfnCreatePluginInstance)
	{
		// check version
		bool bValidVersion = 
			(plugin->minEditorVersion <= m_version && plugin->maxEditorVersion >= m_version);

		if (bValidVersion)
		{
			IEditorPlugin* pPlugin = pfnCreatePluginInstance(this);

			plugin->hLibHandle = hModule;
			plugin->pInstance = pPlugin;
			plugin->bInvalidVersion = false;
			m_plugins[pFilename] = plugin;
			return true;
		}
		else
		{
			plugin->hLibHandle = 0;
			plugin->pInstance = nullptr;
			plugin->bInvalidVersion = true;
			m_plugins[pFilename] = plugin;
		}
	}
	else
	{
		QMessageBox::critical(0, "Error", QString("Cannot find 'createPluginInstance' function in plugin: ") + dllFilename);
	}

	return false;
}

void Editor::unloadPlugin(const char* pFilename)
{
	auto iter = m_plugins.find(pFilename);
	
	if(iter != m_plugins.end())
	{
		// if we have a plugin
		if(iter->second->pInstance)
		{
			// stop plugin
			iter->second->pInstance->onUnload();
			// delete plugin
			delete iter->second->pInstance;
			iter->second->pInstance = nullptr;
		}
#ifdef _WINDOWS_
		FreeLibrary((HMODULE)iter->second->hLibHandle);
#endif
		// take out plugin
		m_plugins.erase(iter);
	}
}

PluginInfo* Editor::findPluginByGuid(const PluginGuid& rGuid)
{
	for(auto iter = m_plugins.begin(); iter != m_plugins.end(); ++iter)
	{
		if(iter->second->guid == rGuid)
		{
			return iter->second;
		}
	}

	return nullptr;
}

void Editor::enablePlugin(const PluginGuid& rGuid, bool bEnable)
{
	//TODO
}

void Editor::unloadAllPlugins()
{
	auto iter = m_plugins.begin();
	std::vector<std::string> filenames;

	while (iter != m_plugins.end())
	{
		filenames.push_back( iter->first );
		++iter;
	}

	for (size_t i = 0; i < filenames.size(); ++i)
	{
		unloadPlugin(filenames[i].c_str());
	}

	m_plugins.clear();
}

QString Editor::readAllTextFromFile(const char* pFilename)
{
	QFile file(pFilename);

	if (!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::information(0, "Error", file.errorString());
		return "";
	}

	QTextStream inStream(&file);
	QString str;

	while (!inStream.atEnd())
	{
		QString line = inStream.readLine();
		str += line;
	}

	file.close();

	return str;
}

bool Editor::connectToEngine()
{
	if (enet_initialize())
	{
		QMessageBox::information(0, "Error", "Could not init enet");
		return false;
	}

	m_pClient = enet_host_create(
		0 /* create a client host */,
		1 /* only allow 1 outgoing connection */,
		2 /* allow up 2 channels to be used, 0 and 1 */,
		57600 / 8 /* 56K modem with 56 Kbps downstream bandwidth */,
		14400 / 8 /* 56K modem with 14 Kbps upstream bandwidth */);

	if (!m_pClient)
	{
		QMessageBox::information(0, "Error", "Could not connect to editor server");
		return false;
	}

	ENetAddress address;
	ENetEvent event;
	ENetPeer* peer;

	enet_address_set_host(&address, "localhost");
	address.port = 7007;

	/* Initiate the connection, allocating the two channels 0 and 1. */
	peer = enet_host_connect(m_pClient, &address, 2, 0);

	if (!peer)
	{
		QMessageBox::information(0, "Error", "No available peers for initiating an ENet connection");

		return false;
	}

	m_clientThread.m_pPeer = peer;
	m_clientThread.m_pClient = m_pClient;
	m_clientThread.m_bStopped = false;
	m_clientThread.start();
	return true;
}

bool Editor::disconnectFromEngine()
{
	m_clientThread.m_bStopped = true;
	m_clientThread.wait();

	if (m_clientThread.m_pPeer)
	{
		enet_peer_disconnect_now(m_clientThread.m_pPeer, 0);
		m_clientThread.m_pPeer = nullptr;
	}

	if (m_pClient)
	{
		enet_host_destroy(m_pClient);
		m_pClient = nullptr;
	}

	enet_deinitialize();
	return true;
}
}