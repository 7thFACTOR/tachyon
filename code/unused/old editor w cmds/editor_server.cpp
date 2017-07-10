#include "editor/editor_server.h"
#include "base/assert.h"
#include "base/logger.h"
#include "base/string.h"
#include "base/array.h"
#include "base/util.h"
#include "core/core.h"
#include "editor/editor.h"
#include "base/json.h"
#include <enet/enet.h>

namespace engine
{
EditorServer::EditorServer()
{
	if (enet_initialize () != 0)
	{
		B_LOG_ERROR("An error occurred while initializing network");
		return;
	}

	B_LOG_INFO("Editor server was created and network initialized");
}

EditorServer::~EditorServer()
{
	enet_deinitialize();
	B_LOG_INFO("Editor server was deleted and network system deinitialized");
}

bool EditorServer::startServer()
{
	m_address.host = ENET_HOST_ANY;
	m_address.port = 7007;
	m_server = enet_host_create(
		&m_address, 
		32 /* allow up to 32 clients and/or outgoing connections */,
		2 /* allow up to 2 channels to be used, 0 and 1 */,
		0 /* assume any amount of incoming bandwidth */,
		0 /* assume any amount of outgoing bandwidth */);
	
	if (!m_server)
	{
		B_LOG_ERROR("An error occurred while trying to create an ENet server host");
		return false;
	}

	B_LOG_INFO("Editor server was started");

	return true;
}

bool EditorServer::stopServer()
{
	enet_host_destroy(m_server);
	B_LOG_INFO("Editor server was stopped");

	return true;
}

void EditorServer::update()
{
	if (!m_server)
	{
		return;
	}

	ENetEvent event;
	int res = enet_host_service(m_server, &event, 0);
	
	if (1)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
		{
			B_LOG_INFO("A new client connected from %x:%u.\n", 
				event.peer->address.host,
				event.peer->address.port);

			/* Store any relevant client information here. */
			event.peer->data = "Client information";
			break;
		}

		case ENET_EVENT_TYPE_RECEIVE:
		{
			String cmdJson = (char*)event.packet->data;
			JsonDocument jsonDoc;

			B_LOG_DEBUG("Editor cmd: %s", cmdJson.c_str());

			jsonDoc.setJsonText(cmdJson);
			
			if (jsonDoc.parse())
			{
				CmdArgs args;
				CmdArgs returnValues;
				auto jsonArgs = jsonDoc.getArray("args");

				if (jsonArgs)
				{
					for (size_t i = 0; i < jsonArgs->size(); ++i)
					{
						args.values.append(jsonArgs->at(i)->asString());
					}
				}

				bool bSuccess;

				if (!editor().call(jsonDoc.getString("cmd"), &args, &returnValues))
				{
					B_LOG_ERROR("Cannot find editor command: '%s'", jsonDoc.getString("cmd").c_str());
					bSuccess = false;
				}
				else
				{
					bSuccess = true;
				}

				JsonDocument jsonDocReturn;

				jsonDocReturn.beginDocument();
				jsonDocReturn.addValue("cmdId", jsonDoc.getInt("cmdId"));
				jsonDocReturn.beginArray("returnArgs");

				for (size_t i = 0; i < returnValues.values.size(); ++i)
				{
					jsonDocReturn.addValue(returnValues.values[i]);
				}

				jsonDocReturn.endArray();
				jsonDocReturn.endDocument();
				
				String jsonReturn;
				
				jsonDocReturn.saveToText(jsonReturn);

				ENetPacket* packet = enet_packet_create(jsonReturn.c_str(), jsonReturn.length() + 1, ENET_PACKET_FLAG_RELIABLE);

				if (0 > enet_peer_send(event.peer, 0, packet))
				{
					B_LOG_ERROR("Cannot send Enet packet for return values");
				}
			}
			
			// Clean up the packet now that we're done using it
			enet_packet_destroy(event.packet);
			break;
		}

		case ENET_EVENT_TYPE_DISCONNECT:
		{	
			B_ASSERT(event.peer);
			B_ASSERT(event.peer->data);
			B_LOG_INFO("%s disconected.\n", event.peer->data);
			event.peer->data = nullptr;
		}
		}
	}
}

}