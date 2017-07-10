// this will include stdafx.h which includes nytro.h also
#include "stdafx.h"

// we'll be using some of the engine's namespaces
// the main namespace
using namespace Nytro;
// the engine system related namespace
using namespace System;
// and the engine core classes/functions
using namespace Engine;

// we define a scene(graph)
SceneGraph* pScene;
// a model made of submeshes with assigned shaders and textures per submesh
ModelNode* pModel;
ModelNode* pModel2;
ModelNode* pModel3;
ParticleSystemNode* pPSys;
ParticleSystemNode* pPSys2;
// a camera to view our model
CameraNode* pCamera;

UiManager   ui;
UiButton    button;
UiCheck     check;
UiSlider    slider;
UiEdit      edit;
UiRadio     radioGaga, radioGugu;
UiWidgetGroup  radioGroup;
UiFontData* pFont;
UiSkin      uiSkin;
UiConsole   console;
FPSCameraController fpsCam;

// camera movement speed (units per second)
const float cCameraMoveSpeed = 50;
bool bMoving = false;

void myUIEvents(UiWidget* pWidget, UiEvent& rEvent)
{
	if (pWidget == &button && rEvent.m_type == UiEvent::EVENT_MOUSE_CLICK)
	{
		messageBox(0, "X1!");
	}
}

void onConsoleCommand(UiConsole* pConsole, Command& rCmd)
{
	if (rCmd.getName() == "x2")
	{
		string str;
		rCmd.getArgumentList().getArgumentValue(0).getAsText(str);

		pConsole->writeColored(Color(1, 0, 0), "XXXX %s", str.c_str());
	}
}

void mainLoop()
{
	// prepare to render
	getGfxProvider()->beginRender();

	// prepare camera matrices and other camera variables
	pCamera->computeTransform();
	// use the camera (will set it as the current camera in the gfx provider)
	fpsCam.controlCamera(pCamera);
	pCamera->use();

	pModel->setTranslation(220, 240, 0);
	//pModel->setRotation( u+=getDeltaTime()*10, 1, 0, 0 );
	pModel->setScale(1, 1, 1);

	pModel2->setTranslation(0, 40, 0);
	pModel2->setScale(1, 1, 1);
	//pModel2->setRotation( -u*7, 0, 1, 0 );

	pModel3->setTranslation(11, 45, 11);
	pModel3->setScale(.3f, .3f, .3f);
	//pModel3->setRotation( u*15, 0, 1, 0 );

	//pAttractorCtrl->m_attractorPos.x += getDeltaTime()*30;

	// prepare scene, sorting render packets
	pScene->prepare();
	// render the scene using the loaded pipeline
	pScene->render();

	//pSkinnedNode->schematicRender();

	//ui.process();
	//ui.render();

	// end rendering
	getGfxProvider()->endRender();
	// flip the backbuffer, showing the goods
	getGfxProvider()->flip();
}

// our custom keyboard/mouse/joystick/etc. input listener/handler
class MyInputListener : public InputListener
{
public:

	// overwriting this virtual member will allow us to respond to input events
	void handleDeviceEvents()
	{
		fpsCam.prepare();

		// check what input device type events are we supposed to handle
		switch (m_pDevice->getType())
		{
			// handle keyboard
		case InputDevice::INPUT_DEVICE_KEYBOARD:
		{
			// cast the current device to keyboard device
			KeyboardInputDevice* pKeyb = (KeyboardInputDevice*) m_pDevice;

			bMoving = false;
			// check if escape pressed down
			if (pKeyb->isKeyDown(KeyboardInputDevice::KEY_ESC))
			{
				// close application, exiting from engine loop
				getApplication()->setStopped(true);
			}

			// pressed W or up arrow, move forward
			if (pKeyb->isKeyDown(KeyboardInputDevice::KEY_W) || pKeyb->isKeyDown(KeyboardInputDevice::KEY_ARROW_UP))
			{
				pCamera->move(Vector3D(0, 0, cCameraMoveSpeed * getDeltaTime()));
				bMoving = true;
			}

			// pressed S or down arrow, move backward
			if (pKeyb->isKeyDown(KeyboardInputDevice::KEY_S) || pKeyb->isKeyDown(KeyboardInputDevice::KEY_ARROW_DOWN))
			{
				pCamera->move(Vector3D(0, 0, -cCameraMoveSpeed * getDeltaTime()));
				bMoving = true;
			}

			// pressed Z, move up
			if (pKeyb->isKeyDown(KeyboardInputDevice::KEY_Z))
			{
				pCamera->move(Vector3D(0, cCameraMoveSpeed * getDeltaTime(), 0));
				bMoving = true;
			}

			// pressed X, move down
			if (pKeyb->isKeyDown(KeyboardInputDevice::KEY_X))
			{
				pCamera->move(Vector3D(0, -cCameraMoveSpeed * getDeltaTime(), 0));
				bMoving = true;
			}

			// pressed A or left arrow, pan left
			if (pKeyb->isKeyDown(KeyboardInputDevice::KEY_A) || pKeyb->isKeyDown(KeyboardInputDevice::KEY_ARROW_LEFT))
			{
				fpsCam.setStrafeDirection(FPSCameraController::LEFT_STRAFE);
				pCamera->move(Vector3D(-cCameraMoveSpeed * getDeltaTime(), 0, 0));
				bMoving = true;
			}

			// pressed D or right arrow, pan right
			if (pKeyb->isKeyDown(KeyboardInputDevice::KEY_D) || pKeyb->isKeyDown(KeyboardInputDevice::KEY_ARROW_RIGHT))
			{
				pCamera->move(Vector3D(cCameraMoveSpeed * getDeltaTime(), 0, 0));
				fpsCam.setStrafeDirection(FPSCameraController::RIGHT_STRAFE);
				bMoving = true;
			}

			// console
			if (pKeyb->isKeyDown(KeyboardInputDevice::KEY_GRAVE))
			{
				console.toggleVisible();
			}

			fpsCam.setPlayerIsMoving(bMoving);
		}

		// input from mouse
		case InputDevice::INPUT_DEVICE_MOUSE:
		{
			// cast, to get our mouse device
			MouseInputDevice* pMouse = (MouseInputDevice*) m_pDevice;

			// rotate camera view direction by yaw
			pCamera->yaw(-pMouse->getDelta().x / 5);
			// rotate camera view direction by pitch
			pCamera->pitch(-pMouse->getDelta().y / 5);
		}
		}
	}
};

// our actual input listener instance, we'll create the pointed object below, on initialize
MyInputListener* pInputListener;

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	// load engine settings xml
	EngineSetup settings("data/engine.config.xml");

	// initialize the engine with the settings
	initializeEngine(&settings);

	// register a file location, to use when loading game data resource files
	// this one has no name, first param, and it is a hard disk folder direct location, named data
	getFileProvider()->registerFileLocation("", FileLocation::FILE_LOCATION_DIRECT, "data");

	// set window title
	getGfxProvider()->getRenderWindow()->setTitle("01 - Basics");

	// create and set the input listener
	pInputListener = new MyInputListener;
	getInputProvider()->setListener(pInputListener);


	// create a scene
	pScene = new SceneGraph;
	pScene->create();
	// load a render pipeline for this scene to be rendered with
	pScene->loadRenderPipeline("default.pipeline.xml");

	// create model
	pModel = new ModelNode;
	pModel->create();
	// load model from resource file, through resource manager
	pModel->setFromResource(getResourceManager()->loadResource("models/teapot.model.xml"));
	// add this model node to the scene's root node
	pScene->getRoot().addChild(pModel);

	// create model
	pModel2 = new ModelNode;
	pModel2->create();
	// load model from resource file, through resource manager
	pModel2->setFromResource(getResourceManager()->loadResource("models/teapot.model.xml"));
	// add this model node to the scene's root node
	pModel->addChild(pModel2);

	// create model
	pModel3 = new ModelNode;
	pModel3->create();
	// load model from resource file, through resource manager
	pModel3->setFromResource(getResourceManager()->loadResource("models/teapot.model.xml"));
	// add this model node to the scene's root node
	pModel2->addChild(pModel3);

	// create camera
	pCamera = new CameraNode;
	pCamera->create();
	// move camera center to some location
	pCamera->setTranslation(0, 0, 200);

	pPSys = new ParticleSystemNode;
	pPSys->create();
	pPSys->setFromResource(getResourceManager()->loadResource("particles/fire.particles.xml"));
	pPSys->setTranslation(0, 70, 0);
	pScene->getRoot().addChild(pPSys);

	pPSys2 = new ParticleSystemNode;
	pPSys2->create();
	pPSys2->setFromResource(getResourceManager()->loadResource("particles/snow.particles.xml"));
	pPSys2->setTranslation(0, 270, 0);

	pScene->getRoot().addChild(pPSys2);

	ui.create();
	pFont = (UiFontData*)getResourceManager()->loadResource("fonts/button.font.xml");

	button.create();
	button.setText("Sample");
	button.setFont(pFont);
	button.setRectangle(UiRectangle(100, 100, 250, 60));

	check.create();
	check.setText("Check here to unleash hell.");
	check.setFont(pFont);
	check.setRectangle(UiRectangle(100, 180, 250, 60));

	slider.create();
	slider.setRectangle(UiRectangle(100, 220, 250, 30));

	edit.create();
	edit.setText("Some text here");
	edit.setFont(pFont);
	edit.setRectangle(UiRectangle(100, 280, 250, 40));

	radioGaga.create();
	radioGaga.setText("Gaga");
	radioGaga.setFont(pFont);
	radioGaga.setRectangle(UiRectangle(100, 320, 250, 40));

	radioGugu.create();
	radioGugu.setText("Gugu");
	radioGugu.setFont(pFont);
	radioGugu.setRectangle(UiRectangle(100, 360, 250, 40));

	radioGroup.addWidget(&radioGaga);
	radioGroup.addWidget(&radioGugu);

	console.setManager(&ui);
	console.create();
	console.setFont(pFont);
	console.setCommandCallback(onConsoleCommand);
	console.setBackTexture((Texture*)getResourceManager()->loadResource("textures/gui/console.png"));
	console.setBackTile(10, 5);

	ui.addWidget(&button);
	ui.addWidget(&check);
	ui.addWidget(&slider);
	ui.addWidget(&edit);
	ui.addWidget(&radioGaga);
	ui.addWidget(&radioGugu);
	ui.addWidget(&console);

	ui.getMousePointer()->setTexture((Texture*) getResourceManager()->loadResource("textures/gui/cursor.png"));
	ui.setUiEventCallback(myUIEvents);

	// set the main user callback loop function, declared above
	getApplication()->setMainLoopCallback(mainLoop);

	debugWrite("Running sample.");
	// run the engine's main loop
	getApplication()->run();

	// on exit, free the objects
	pCamera->free();
	pModel->free();
	pScene->free();
	ui.free();

	// unset the input listener, because we'll delete it after this
	// and we dont want any input events anymore
	getInputProvider()->setListener(NULL);

	// delete objects
	safe_delete(pCamera);
	safe_delete(pModel);
	safe_delete(pScene);
	safe_delete(pInputListener);

	// release the engine. Done!
	releaseEngine();

	return 0;
}