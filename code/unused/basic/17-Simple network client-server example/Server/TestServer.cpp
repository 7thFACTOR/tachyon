#include "stdafx.h"

bool g_bExitServer = false;

NetServer* pServer;

class ClientInfo
{
public:

	Vector3D    m_position;
	Color       m_color;
	string      m_name;
};

map<int, ClientInfo> clients;

void mainLoop()
{
	Nytro::Engine::processFrame();
}

class MyNetListener: public NetworkListener
{
public:

	void onMessage(NetPacket* pMsg)
	{
		switch (pMsg->getMessageId())
		{
		case 101:
		{
			int id = pMsg->getOwnerClient()->getId();
			Vector3D move = pMsg->readVector3D();
			//debugWrite( "Client ID:%d to move with delta: %f %f %f", id, move.x, move.y, move.z );
			clients[id].m_position += move;

			// broadcast move
			NetPacket p;
			p.beginWrite(101);
			p.writeInt(id);
			p.writeVector3D(move);
			pServer->send(&p);

			break;
		}
		}
	}

	void onClientConnected(NetClientInfo* pClient)
	{
		// send clients to the new client
		for (uint32 i = 0; i < pServer->getClients().size(); i++)
		{
			NetPacket packet;

			packet.setPacketFlags(NetPacket::FLAG_RELIABLE);
			packet.beginWrite(NET_MSG_PEER_CONNECTED);
			packet.writeInt(pServer->getClients()[i]->getId());
			packet.writeVector3D(clients[pServer->getClients()[i]->getId()].m_position);
			pServer->send(&packet, pClient);

			// and send this client to others
			packet.beginWrite(NET_MSG_PEER_CONNECTED);
			packet.writeInt(pClient->getId());
			packet.writeVector3D(Vector3D());
			pServer->send(&packet, pServer->getClients()[i]);
		}
	}

	void onClientDisconnected(NetClientInfo* pClient)
	{
		debugWrite("Client UID: %d disconnected so now send this info to others.", pClient->getId());
		debugWrite("Sent.");
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	Nytro::Engine::EngineSetup setup("data/server.setup.xml");
	StdioLogger stdioLogger;
	getLogger()->linkChild(&stdioLogger);
	getLogger()->open("server.log");
	Nytro::Engine::initializeEngine(&setup);

	pServer = (NetServer*) getNetworkProvider()->newInstanceOf(NetworkProvider::INTERFACE_ID_SERVER);
	assert(pServer);

	pServer->setUdpClientDisconnectTimeout(350000);
	pServer->setListener(new MyNetListener);
	pServer->start(NET_DEFAULT_PORT, NET_PROTOCOL_UDP);

	while (!g_bExitServer)
	{
		mainLoop();
		sleep(1);
	}

	safe_delete(pServer);
	Nytro::Engine::releaseEngine();

	return 0;
}

