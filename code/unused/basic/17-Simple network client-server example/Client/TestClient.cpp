#include "stdafx.h"

NetClient* pClient;
Canvas* pCanvas;
CameraNode* pCamera;

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
	getGfxProvider()->setClearColor(0.4, 0.4, 0.4, 1);
	getGfxProvider()->clear();
	getGfxProvider()->beginRender();

	getGfxProvider()->getCamera()->computeTransform();
	getGfxProvider()->getCamera()->use();
	getGfxProvider()->disableShaders();
	getGfxProvider()->disableTexturing();
	getGfxProvider()->setMatrixMode(GfxProvider::MATRIX_WORLD);
	getGfxProvider()->loadIdentity();
	pCanvas->drawGrid(5000, 10, 50, Color(0, 0, 0), Color(0.2, 0.2, 0.2), Color(1, 0, 0));

	map<int, ClientInfo>::iterator iter = clients.begin();

	while (iter != clients.end())
	{
		getGfxProvider()->setMatrixMode(GfxProvider::MATRIX_WORLD);
		getGfxProvider()->loadIdentity();
		getGfxProvider()->translate(iter->second.m_position.x, iter->second.m_position.y, iter->second.m_position.z);
		pCanvas->begin(VertexBuffer::TRIANGLE_LIST);
		pCanvas->color(&iter->second.m_color);
		pCanvas->drawSolidBox(10, 20, 10);
		pCanvas->end();

		iter++;
	}

	getGfxProvider()->endRender();
	getGfxProvider()->flip();

	sleep(1);
}

class MyInputListener : public InputListener
{
public:

	void handleDeviceEvents()
	{
		if (m_pDevice->getType() == InputDevice::INPUT_DEVICE_KEYBOARD)
		{
			KeyboardInputDevice* pDev  = (KeyboardInputDevice*) m_pDevice;

			bool bSend = false;
			Vector3D move;
			const int speed  = 80;

			if (pDev->isKeyDown(KeyboardInputDevice::KEY_A))
			{
				bSend = true;
				move.x = getDeltaTime() * speed;
			}
			if (pDev->isKeyDown(KeyboardInputDevice::KEY_D))
			{
				bSend = true;
				move.x = -getDeltaTime() * speed;
			}
			if (pDev->isKeyDown(KeyboardInputDevice::KEY_W))
			{
				bSend = true;
				move.z = getDeltaTime() * speed;
			}
			if (pDev->isKeyDown(KeyboardInputDevice::KEY_S))
			{
				bSend = true;
				move.z = -getDeltaTime() * speed;
			}
			if (pDev->isKeyDown(KeyboardInputDevice::KEY_ESC))
			{
				getApplication()->setStopped(true);
			}

			if (bSend)
			{
				NetPacket p;

				p.beginWrite(101);
				p.writeVector3D(move);
				pClient->send(&p);
				//debugWrite( "Send move..." );
			}
		}

//            sleep(50);
	}
};

class MyNetListener: public NetworkListener
{
public:

	void onMessage(NetPacket* pMsg)
	{
		//debugWrite( "MSGID:%d", pMsg->getMessageId() );
		switch (pMsg->getMessageId())
		{
		case 101:
		{
			int clientId = pMsg->readInt();
			Vector3D move = pMsg->readVector3D();
			clients[clientId].m_position += move;
			//debugWrite( "Move ID:%d %f %f %f", clientId, move.x, move.y, move.z );
			break;
		}

		case NET_MSG_PEER_CONNECTED:
		{
			int clientId = pMsg->readInt();
			clients[clientId].m_position = pMsg->readVector3D();
			clients[clientId].m_color.r = getRandomFloat(0, 1);
			clients[clientId].m_color.g = getRandomFloat(0, 1);
			clients[clientId].m_color.b = getRandomFloat(0, 1);
			debugWrite("A new client peer connected");
			break;
		}

		case NET_MSG_PEER_DISCONNECTED:
		{
			int clientId = pMsg->readInt();
			debugWrite("Peer %d left, so saaaaad...", clientId);

			map<int, ClientInfo>::iterator iter = clients.find(clientId);

			if (iter != clients.end())
			{
				clients.erase(iter);
			}
			break;
		}

		case NET_MSG_DISCONNECTED:
		{
			debugWrite("Server is no longer.");
			break;
		}
		}
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	Nytro::Engine::EngineSetup setup("data/client.setup.xml");
	StdioLogger stdioLogger;
	string str;
	formatString(str, "client_%d.log", getTickCount());
	getLogger()->open(str.c_str());
	getLogger()->linkChild(&stdioLogger);
	Nytro::Engine::initializeEngine(&setup);

	pClient = (NetClient*) getNetworkProvider()->newInstanceOf(NetworkProvider::INTERFACE_ID_CLIENT);
	assert(pClient);

	pClient->connect("localhost", NET_DEFAULT_PORT, NET_PROTOCOL_UDP);
	pClient->setListener(new MyNetListener);

	NetPacket p;
	p.beginWrite(NET_MSG_HELLO);
	pClient->send(&p);

	assert(getInputProvider());
	getInputProvider()->setListener(new MyInputListener);

	pCanvas = (Canvas*) getGfxProvider()->newInstanceOf(GfxProvider::INTERFACE_ID_CANVAS);
	pCamera = new CameraNode;
	pCamera->create();
	getGfxProvider()->setCamera(pCamera);
	pCamera->setTranslation(500, 100, -500);
	pCamera->setRotation(45, 0, 1, 0);

	getApplication()->setMainLoopCallback(mainLoop);
	getApplication()->run();

	p.setPacketFlags(NetPacket::FLAG_RELIABLE);
	p.beginWrite(NET_MSG_BYE);
	pClient->send(&p);
	debugWrite("Sent BYE.");

	while (pClient->getSendPacketQueue().size())
	{
		debugWrite("Waiting for %d packets to be sent...", pClient->getSendPacketQueue().size());
		sleep(1000);
	}

	delete getInputProvider()->getListener();
	delete pClient->getListener();
	safe_delete(pCamera);
	safe_delete(pCanvas);
	safe_delete(pClient);

	Nytro::Engine::releaseEngine();

	return 0;
}

