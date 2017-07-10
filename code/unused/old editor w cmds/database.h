#pragma once
#include "base/types.h"
#include "base/defines.h"
#include "base/string.h"

namespace engine
{
using namespace base;

struct UserRecord
{
		u32 id_user;
		u32 id_user_group;
		String username, password, fullname, email;
};

struct UserGroupRecord
{
		u32 id_user_group;
		String name;
};

struct ProjectRecord
{
		u32 id_project;
		String name;
		String gameClassName;
		String dataRootPath;
};

struct SessionRecord
{
		u32 id_session;
		u32 id_user;
};

struct ActionRecord
{
		u32 id_action;
		u32 id_session;
		u32 id_user;
		bool is_reverted;
		String action_data;
};

struct AssetRecord
{
		u32 id_asset;
		u32 id_project;
		String filename;
};

struct SettingsRecord
{
		u32 id_settings;
		u32 id_user;
		String settings_data;
};

struct LayerRecord
{
	u32 id_layer;
};

struct SectionRecord
{
	u32 id_section;
};

struct IEditorDatabase
{
	typedef bool (*TPfnEnumUsers)(const UserRecord& rUser);
	typedef bool (*TPfnEnumUserGroups)(const UserGroupRecord& rUserGroup);
	typedef bool (*TPfnEnumProjects)(const ProjectRecord& rProject);
	typedef bool (*TPfnEnumSessions)(const SessionRecord& rSession);
	typedef bool (*TPfnEnumActions)(const ActionRecord& rAction);
	typedef bool (*TPfnEnumAssets)(const AssetRecord& rAsset);
	typedef bool (*TPfnEnumLayers)(const LayerRecord& rRec);
	typedef bool (*TPfnEnumSections)(const SectionRecord& rRec);

		virtual ~IEditorDatabase(){}

	virtual bool initialize() = 0;
	virtual bool shutdown() = 0;

	// users
	virtual bool addUser(const UserRecord& rUser) = 0;
	virtual bool updateUser(const UserRecord& rUser) = 0;
	virtual bool deleteUser(u32 aUserId) = 0;
	virtual bool getUser(UserRecord& rUser) = 0;
	virtual bool enumerateUsers(TPfnEnumUsers pCallback) = 0;
	
	virtual bool addUserGroup(const UserGroupRecord& rGroup) = 0;
	virtual bool updateUserGroup(const UserGroupRecord& rGroup) = 0;
	virtual bool deleteUserGroup(u32 aGroupId) = 0;
	virtual bool getUserGroup(UserGroupRecord& rGroup) = 0;
	virtual bool enumerateUserGroups(TPfnEnumUserGroups pCallback) = 0;

	// projects
	virtual bool addProject(const ProjectRecord& rProject) = 0;
	virtual bool updateProject(const ProjectRecord& rProject) = 0;
	virtual bool deleteProject(u32 aProjectId) = 0;
	virtual bool getProject(ProjectRecord& rProject) = 0;
	virtual bool enumerateProjects(TPfnEnumProjects pCallback) = 0;

	// sessions
	virtual bool login(const char* pUsername, const char* pPassword, u32& aOutSessionId) = 0;
	virtual bool forgotPassword(const char* pUsername) = 0;
	virtual bool logout(u32 aSessionId) = 0;
	virtual u32 lastSessionId(u32 aUserId) = 0;
	virtual bool updateSession(const SessionRecord& rSession) = 0;
	virtual bool deleteSession(u32 aSessionId) = 0;
	virtual bool deleteEmptySessions(u32 aUserId) = 0;
	virtual bool deleteAllEmptySessions() = 0;
	virtual bool getSession(SessionRecord& rSession) = 0;
	virtual bool enumerateSessions(TPfnEnumSessions pCallback) = 0;

	// settings
	virtual bool updateSettings(u32 aSessionId, const char* pSettingsText) = 0;
		virtual bool getSettings(u32 aSessionId, String& rOutSettingsText) = 0;

	// actions
	virtual bool addAction(u32 aSessionId, const ActionRecord& rAction) = 0;
	virtual bool deleteAction(u32 aActionId) = 0;
	virtual bool undo(const ActionRecord& rUndoAction) = 0;
	virtual bool redo(const ActionRecord& rRedoAction) = 0;
	virtual bool deleteSessionActions(u32 aSessionId, u32 aCountFromLastOne) = 0;
	virtual u32 lastActionId(u32 aSessionId) = 0;
	virtual bool enumerateActions(u32 aSessionId, TPfnEnumActions pCallback) = 0;

	// collaborative
	//! locks an entire section for editing
	//! \return false if the section cannot be locked
	virtual bool setSectionLock(u32 aSessionId, u32 aSectionId, bool bLock) = 0;
	//! locks an entire layer for editing
	//! \return false if the layer cannot be locked
	virtual bool setLayerLock(u32 aSessionId, u32 aLayerId, bool bLock) = 0;
	virtual bool isSectionLocked(u32 aSectionId) = 0;
	virtual bool isLayerLocked(u32 aLayerId) = 0;

	// sections
	virtual bool addSection(u32 aSessionId, const SectionRecord& rSection) = 0;
	virtual bool updateSection(u32 aSessionId, const SectionRecord& rSection) = 0;
	virtual bool deleteSection(u32 aSessionId, u32 aSectionId) = 0;
	virtual bool getSection(SectionRecord& rSection) = 0;
	virtual bool enumerateSections(u32 aProjectId, TPfnEnumSections pCallback) = 0;

	// layers
	virtual bool addLayer(u32 aSessionId, const LayerRecord& rLayer) = 0;
	virtual bool updateLayer(u32 aSessionId, const LayerRecord& rLayer) = 0;
	virtual bool deleteLayer(u32 aSessionId, u32 aLayerId) = 0;
	virtual bool getLayer(LayerRecord& rLayer) = 0;
	virtual bool enumerateLayers(u32 aSectionId, TPfnEnumSections pCallback) = 0;

	// assets
	virtual bool addAsset(u32 aSessionId, const AssetRecord& rAsset) = 0;
	virtual bool deleteAsset(u32 aAssetId) = 0;
	virtual bool updateAsset(const AssetRecord& rAsset) = 0;
	virtual bool getAsset(AssetRecord& rAsset) = 0;
	virtual bool findAsset(const char* pFilename, AssetRecord& rOutAsset) = 0;
	virtual bool enumerateAssets(u32 aProjectId, TPfnEnumAssets pCallback) = 0;
};

}