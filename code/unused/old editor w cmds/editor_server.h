#pragma once
#include "base/types.h"
#include "base/defines.h"
#include "core/defines.h"
#include <enet/enet.h>

namespace engine
{
using namespace base;

class Editor;

class E_API EditorServer
{
public:
	EditorServer();
	virtual ~EditorServer();

	bool startServer();
	bool stopServer();
	void update();

protected:
	ENetAddress m_address;
	ENetHost* m_server;
};

}