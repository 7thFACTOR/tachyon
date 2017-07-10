#include "stdafx.h"

// fancy HTML log
HTMLLogger g_htmlLogger("nytro.log.html");

void onWidgetEvent(UiWidget* pWidget, UiEvent& rEvent)
{
	if (pWidget->getName() == "btn1" && rEvent.m_type == UiEvent::EVENT_MOUSE_CLICK)
	{
		pWidget->getManager()->showWidgetsByGroupName("grp1", false);
	}
}

MyInGameStateHandler::MyInGameStateHandler()
{
	// must set the type of this state handler, this one will be for the actual in-game state
	setType(GAME_STATE_GAME_PLAY);
	m_pModel = NULL;
}

// called when this game state must be initialized
void MyInGameStateHandler::onInitialize()
{
	debugWrite("Init game play state");
	// load input action controls
	m_controls.load("data/default.actions.xml");

	ui.create();
	ui.setUiEventCallback(onWidgetEvent);
	skin.load("ui/default.uiskin.xml");
	m_pUiLayout = (UiLayoutData*) getResourceManager()->loadResource("ui/sample.ui.xml");
	ui.addWidgets(m_pUiLayout->getWidgets());
	skin.apply(&ui);
	ui.getMousePointer()->setTexture((Texture*)getResourceManager()->loadResource("textures/gui/cursor.png"));

	m_pCamera = new CameraNode();
	m_pCamera->create();
	m_pCanvas = (Canvas*) getGfxProvider()->newInstanceOf(GfxProvider::INTERFACE_ID_CANVAS);
	m_pCanvas->create();

	m_pModel = new ModelNode;
	m_pModel->create();
	m_pModel->setFromResource(getResourceManager()->loadResource("models/limpy.model.xml"));
	getGameProvider()->getScene()->getRoot().addChild(m_pModel);

	m_pAnimSet = (SkeletonAnimationSet*) getResourceManager()->loadResource("anims/hunter.animset.xml");
	m_pModel->setAnimationSet(m_pAnimSet);
}

// called when this game state must be released
void MyInGameStateHandler::onRelease()
{
	safe_delete(m_pModel);
	safe_delete(m_pCamera);
}

// called when the user enters this game state (from another state)
void MyInGameStateHandler::onEnter()
{
}

// called when the user exits this game state (but the game state is not destroyed, just paused)
void MyInGameStateHandler::onExit()
{
}

// called every frame when this game state is the current one
void MyInGameStateHandler::onFrame()
{
	// begin rendering
	getGfxProvider()->clear();
	getGfxProvider()->beginRender();

	// prepare the camera and use it
	// prepare camera matrices and other camera variables
	m_pCamera->setTranslation(m_pCamera->getTranslation().x, 50, m_pCamera->getTranslation().z);
	m_pCamera->computeTransform();
	m_fpsCameraController.controlCamera(m_pCamera);
	m_pCamera->use();
	getGameProvider()->getScene()->prepare();
	getGameProvider()->getScene()->render();
	m_pModel->schematicRender();
	m_pCanvas->setupForUi();
	m_pCanvas->setFont((UiFontData*)getResourceManager()->loadResource("fonts/viewport.font.xml"));

	ui.process();
	ui.render();

	// end rendering
	getGfxProvider()->endRender();
	// show back-buffer image that we have obtained/rendered
	getGfxProvider()->flip();
}

// called when the engine has input data to be processed by this current game state
void MyInGameStateHandler::onInput(InputDevice* pDevice)
{
	// input from mouse
	if (pDevice->getType() == InputDevice::INPUT_DEVICE_MOUSE)
	{
		// cast, to get our mouse device
		MouseInputDevice* pMouse = (MouseInputDevice*) pDevice;

		// rotate camera view direction by pitch
		m_pCamera->pitch(-pMouse->getDelta().y / 5);
		// rotate camera view direction by yaw
		m_pCamera->yaw(-pMouse->getDelta().x / 5);
	}

	// prepare action controls to be queried
	m_controls.prepare();

	// camera movement speed in units (meters) per second
	const float cCameraMoveSpeed = 45;
	const float cCameraRunSpeedMul = 3;

	// if the 'exit' action was triggered, then halt engine loop
	if (m_controls.isAction("exit"))
	{
		getApplication()->setStopped(true);
	}

	m_fpsCameraController.prepare();
	float runningMul = 1;

	if (m_controls.isAction("run"))
	{
		runningMul = cCameraRunSpeedMul;
		m_fpsCameraController.setPlayerIsRunning(true);
	}

	if (m_controls.isAction("move_forward"))
	{
		m_fpsCameraController.setPlayerIsMoving(true);
		m_pCamera->move(Vector3D(0, 0, runningMul * cCameraMoveSpeed * getDeltaTime()));
	}

	if (m_controls.isAction("move_backward"))
	{
		m_fpsCameraController.setPlayerIsMoving(true);
		m_pCamera->move(Vector3D(0, 0, -cCameraMoveSpeed * runningMul * getDeltaTime()));
	}

	if (m_controls.isAction("move_left"))
	{
		m_fpsCameraController.setPlayerIsMoving(true);
		m_fpsCameraController.setStrafeDirection(FPSCameraController::LEFT_STRAFE);
		m_pCamera->move(Vector3D(-cCameraMoveSpeed * runningMul * getDeltaTime(), 0, 0));
	}

	if (m_controls.isAction("move_right"))
	{
		m_fpsCameraController.setPlayerIsMoving(true);
		m_fpsCameraController.setStrafeDirection(FPSCameraController::RIGHT_STRAFE);
		m_pCamera->move(Vector3D(cCameraMoveSpeed * runningMul * getDeltaTime(), 0, 0));
	}

	if (m_controls.isAction("fire_left"))
	{
	}
}

//---

// called by the engine when the game must be initialized
Result MyGame::initialize()
{
	// first we must call the base method
	GameProvider::initialize();

	// FIRST OF ALL!!! register a file location, to use when loading game data resource files
	// this one has no name, first parameter, and its a hard disk folder direct location, named 'data',
	// same folder as the executable, both in the \binaries folder
	getFileProvider()->registerFileLocation("", FileLocation::FILE_LOCATION_DIRECT, "data");

	// setup editor's file locations
	getFileProvider()->registerFileLocation("editor", FileLocation::FILE_LOCATION_DIRECT, "editor");
	getFileProvider()->getFileLocationByName("editor")->setDisplayName("Editor");

	// load a rendering pipeline into the scene
	getScene()->loadRenderPipeline("default.pipeline.xml");
	getMaterialLibrary()->load("materials/default.materiallib.xml");
	getWorld()->load("worlds/1.world.xml");
	GameConfiguration cfg;


	// set the game title, that will appear in the window title
	setGameTitle("02 - Simple Game Provider");

	// lets add a new instance of the in-game state handler
	addGameStateHandler(new MyInGameStateHandler);

	// and lets set the current game state
	setCurrentGameStateHandler(GameStateHandler::GAME_STATE_GAME_PLAY);

	getCurrentGameStateHandler()->onInitialize();

	return ALL_OK;
}

// called by the engine when the game must be released
Result MyGame::release()
{
	getCurrentGameStateHandler()->onRelease();

	// first we must call the base method
	GameProvider::release();

	// delete the instance of the in-game state handler
	delete getGameStateHandlerByType(GameStateHandler::GAME_STATE_GAME_PLAY);

	return ALL_OK;
}

//---

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	// load engine setup
	EngineSetup setup("data/engine.setup.xml");

	setLogger(&g_htmlLogger);
	// tweak settings, manually specifying the game provider
	setup.setUserProvider(SUPER_ID_GAME_PROVIDER, new MyGame);

	// initialize the engine with the setup
	initializeEngine(&setup);

	// run the engine's main loop, which will run the game provider's onFrame() in a loop
	getApplication()->run();

	// release the engine. Done!
	releaseEngine();

	return 0;
}

