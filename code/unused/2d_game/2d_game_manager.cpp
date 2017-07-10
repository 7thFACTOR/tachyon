#include "2d_game_manager.h"
#include "system/core.h"
#include "system/resource_manager.h"
#include "system/profiler.h"
#include "system/plugin.h"
#include "system/util.h"
#include "system/application.h"
#include "math/conversion.h"
#include "game/components/camera.h"
#include "game/types.h"
#include "audio/audio_manager.h"
#include "resources/texture.h"
#include "resources/gpu_program.h"
#include "physics/character.h"
#include "input/input_manager.h"
#include "input/mouse.h"
#include "system/logger.h"
#include "editor/editor.h"

using namespace nytro;

N_REGISTER_PLUGIN(Game2DPlugin, "A sample 2D game");
N_REGISTER_DERIVED_CLASS(Game2D, GameManager);

int numInstancesPerSide = 10;
Vec3* velos;

void Game2D::onInitialize()
{
	files().addDataFolder("../data/crazystar");
	files().addDataFolder("../data/engine");

	if (engineHost() == eEngineHost_GameLauncher)
	{
		loadSection("worlds/test", false);
	}

	m_pFont = nullptr;
	//m_cameraEntityId = StandardEntitiesFactory::composeCamera(this->createEmptyEntity());
	m_controls.load("../config/2d_game.input");

	if (engineHost() == eEngineHost_Editor)
	{
		//m_pCameraEntity = findEntityByName("EditorCamera");
	}
	else
	{
		//m_pCameraEntity = createCameraEntity();
		//m_pCameraEntity->component<Transform>()->setTranslation(100, 100, 100);
	}

	m_pFont = resources().load<FontData>("fonts/default");
	m_pSmallFont = resources().load<FontData>("fonts/small");
	m_pDefaultGpuProgram = resources().load<GpuProgram>("gpu_programs/default");
	m_pMeshData = resources().load<MeshResource>("meshes/teapot001");

	m_pRT = render().newTexture();
	m_pRT->setAsRenderTarget(Texture::eRenderTargetFormat_RGBA8, 512, 512, true, true, true);
	m_pRT->upload();

	m_pInstances = render().newGeometryInstanceBuffer();
	int numInstances = powl(numInstancesPerSide, 3);
	m_pInstances->create(
		GeometryInstanceBuffer::eAttribute_TransformMatrix1
		| GeometryInstanceBuffer::eAttribute_Color, numInstances);

	u8* pBuf = m_pInstances->instanceDataBuffer();
	Matrix m;
	Color c;

	velos = new Vec3[numInstances];
	int iii = 0;

	for (int i = 0; i < numInstancesPerSide; ++i)
	{
		for (int j = 0; j < numInstancesPerSide; ++j)
		{
			for (int k = 0; k < numInstancesPerSide; ++k)
			{
				m.setIdentity();
				m.setTranslation(i*150, j*150, k*150);
				c.set(randomFloat(0, 1), randomFloat(0, 1), randomFloat(0, 1), 1);
				*((Color*)(pBuf)) = c;
				*((Matrix*)(pBuf + sizeof(Color))) = m;
				pBuf += sizeof(Matrix) + sizeof(Color);
				velos[iii++].set(randomFloat(-1,1), randomFloat(-1,1), randomFloat(-1,1));
			}
		}
	}

	m_pInstances->uploadInstanceDataBuffer();
	setGameState(eGameState_InGame);
}

void Game2D::onShutdown()
{
	if (engineHost() == eEngineHost_GameLauncher)
	{
	//	m_pCameraEntity->destroyAllComponents();
	//	N_SAFE_DELETE(m_pCameraEntity);
	}

	m_controls.free();
}

void Game2D::onEnterGameState()
{
	if (engineHost() == eEngineHost_Editor)
	{
//		m_pCameraEntity = findEntityByName("EditorCamera");
	}
}

void Game2D::onExitGameState()
{
}

void Game2D::onUpdate()
{
	if (&editor())
	{
		editor().update();
	}

//	m_pCameraEntity->update();
	audio().update();
}

void Game2D::onRender()
{
	String str;

	render().mainContext().beginRender();
		
	u8* pBuf = m_pInstances->instanceDataBuffer();
	Matrix m;
	int iii = 0;
	static f32 u = 0;

	for (int i = 0; i < numInstancesPerSide; ++i)
	{
		for (int j = 0; j < numInstancesPerSide; ++j)
		{
			for (int k = 0; k < numInstancesPerSide; ++k)
			{
				m.setIdentity();
				Matrix oldm = *((Matrix*)(pBuf + sizeof(Color)));
				//velos[iii] += gameDeltaTime()*40;
				m.setTranslation(velos[iii]*gameDeltaTime()*40 + oldm.translation());
				Quat q;
				q = nytro::toQuat((u+=1.0f*gameDeltaTime()), Vec3(1,0,0));
				Matrix m2 = toMatrix(q);
				Matrix m3;
				m3.setIdentity();
				m3.setScale(1,3,1);
				m = m2*m;
				*((Matrix*)(pBuf + sizeof(Color))) = m;
				pBuf += sizeof(Color) + sizeof(Matrix);
				++iii;
			}
		}
	}
m.setIdentity();
	m_pInstances->uploadInstanceDataBuffer();
	
	render().mainContext().setRenderTarget(m_pRT);
	render().mainContext().clear(
		eClearBufferFlag_Color|eClearBufferFlag_Depth,
		Color(142.0f/255.0f, 0.0f/255.0f, 0.0f/255.0f, 0));

	Texture* ptex = resources().load<Texture>("textures/missing_material");

	render().mainContext().depthStencilState().bDepthTest = true;
	render().mainContext().depthStencilState().bDepthWrite = true;
	render().mainContext().depthStencilState().depthFunc = eCmpFunc_LessEqual;
	render().mainContext().rasterizerState().cullMode = eCullMode_None;
	render().mainContext().rasterizerState().cullMode = eCullMode_CCW;
	render().mainContext().blendState().renderTarget[0].bEnable = false;
	render().mainContext().blendState().renderTarget[0].renderTargetWriteMask = 0xFF;
	render().mainContext().blendState().renderTarget[0].blendOp = eBlendOp_Add;
	render().mainContext().blendState().renderTarget[0].srcBlend = eBlend_One;
	render().mainContext().blendState().renderTarget[0].destBlend = eBlend_One;

	m.setTranslation(0, 150, 0);
	render().mainContext().fitViewportToWindow();
	render().mainContext().setWorldMatrix(m);
//	render().mainContext().setProjectionMatrix(m_pCameraEntity->component<Camera>()->projectionMatrix());
//	render().mainContext().setViewMatrix(m_pCameraEntity->component<Camera>()->viewMatrix());

	render().mainContext().bindGpuProgram(m_pDefaultGpuProgram);
	render().mainContext().bindTexture(ptex, N_DEFAULT_TEXTURE_SAMPLER_NAME);
	render().mainContext().setGpuProgramRenderMethod(N_DEFAULT_GPU_PROGRAM_RENDER_METHOD);
	render().mainContext().commitGpuProgramConstants();
	for (int i = 0; i < m_pMeshData->geometryBuffers().count(); ++i)
	{
		render().mainContext().renderGeometryBuffer(m_pMeshData->geometryBuffers()[i], 0);
	}
	render().mainContext().unbindGpuProgram();
	


	render().mainContext().setRenderTarget(0);
	render().mainContext().clear(
		eClearBufferFlag_Color|eClearBufferFlag_Depth,
		Color(242.0f/255.0f, 220.0f/255.0f, 250.0f/255.0f, 0));

	f32 c[4] = {1,0,0,1};
	f32 c2[4] = {1,1,1,1};
	m.setTranslation(0, 0, 0);
	render().mainContext().dbgRenderBegin();
	render().mainContext().dbgSetColor(1,1,1,1);
	render().mainContext().setWorldMatrix(m);
	render().mainContext().dbgSetInstanceBuffer(m_pInstances);
	render().mainContext().dbgSetBlendMode(eBlendMode_Opaque);
	render().mainContext().dbgDisableTexturing();
	render().mainContext().setGpuProgramRenderMethod(N_DEBUG_GPU_PROGRAM_RENDER_METHOD_SOLID_INSTANCED);
	render().mainContext().dbgDrawSolidEllipsoid(24, 24, 24, 8, 8);
	render().mainContext().dbgSetInstanceBuffer(nullptr);
	render().mainContext().setGpuProgramRenderMethod(N_DEBUG_GPU_PROGRAM_RENDER_METHOD_TEXTURED);
	render().mainContext().dbgDrawGrid(1024, 32, 64, Color(0,0,1,1),  Color(1,0,0,1),  Color(1,1,0,1));
	render().mainContext().setGpuProgramConstantArray("diffuseColor", c2, 4);
	render().mainContext().dbgSetColor(0,0,1,1);
	render().mainContext().dbgDrawSolidEllipsoid(20, 20, 20, 40, 40);
	m.setTranslation(0,1220,0);
	render().mainContext().setWorldMatrix(m);
	render().mainContext().dbgSetColor(0,1,0,1);
	render().mainContext().dbgDraw3dCross(150);
	m.setTranslation(0,0,0);
	m.setRotationZ(0);
	render().mainContext().dbgSetProjectionForUI();
	render().mainContext().dbgSetBlendMode(eBlendMode_Opaque);
	render().mainContext().dbgTextSettings().pFont = m_pSmallFont;
	profiler().renderGraph(&render().mainContext(), profiler().section("ExecuteRenderCommands"), 10, render().mainContext().viewHeight() - 90, render().mainContext().viewWidth() - 40, 60);
	render().mainContext().dbgDraw2dTexturedQuad(m_pRT, 0, render().mainContext().viewHeight() - 26, 26, 26);
	render().mainContext().dbgDraw2dTexturedQuad(m_pRT, render().mainContext().viewWidth() - 100,render().mainContext().viewHeight() - 100, 100, 100);
	render().mainContext().dbgDraw2dTexturedQuad(m_pRT, 0, 0, 100, 100);
	render().mainContext().dbgDraw2dTexturedQuad(m_pRT, render().mainContext().viewWidth() - 100,0,100,100);

	//Transform* pCamXform = m_pCameraEntity->component<Transform>();
	int allocs = mainAllocator()->allocations();
	int totalMem = mainAllocator()->totalAllocatedSize();
	formatString(str,
		"2D Game, RenderManager: %s\n"
		"Camera(%f, %f, %f)\n"
		"Mem(allocs: %d, size: %d Mb)\n"
		"FPS: %0.2f Speed: %0.2f kmph\n"
		"DBGVertexLoad %d pct (%d/%d)\n",
		render().runtimeClass()->className(),
		0.0f, 0.0f, 0.0f,
		allocs, (int)(totalMem / 1024 / 1024),
		framesPerSecond(),
		m_crtSpeed / 1000.0f * 3600.0f,
		(int)(100 * (f32)render().mainContext().dbgUsedVertexCount() / render().mainContext().dbgMaxVertexCount()),
		render().mainContext().dbgUsedVertexCount(),
		render().mainContext().dbgMaxVertexCount());
	render().mainContext().dbgEnableTextureFiltering(false);
	render().mainContext().dbgTextSettings().textColor = Color::kYellow;
	render().mainContext().dbgDrawTextAt(0, 10, str.c_str());
	render().mainContext().dbgEnableTextureFiltering(true);
	render().mainContext().dbgRenderEnd();
	
	if (&editor())
	{
		editor().onRenderGizmos(render().mainContext());
	}
	
	render().mainContext().endRender();

	N_BEGIN_PROFILE_SECTION(ExecuteRenderCommands);
	render().sortRenderCommands();
	render().executeRenderCommands();
	N_END_PROFILE_SECTION(ExecuteRenderCommands);
	profiler().computeSectionsPercentTime();
	render().present();
}

void Game2D::onInput()
{
	m_controls.prepare();

	// camera movement speed in units (meters) per second
	const f32 cCameraMoveSpeed = 114.0f * kMeterUnit;
	const f32 cCameraRunSpeedMul = 3;
	bool bIsMoving = false;
	Vec3 actorMoveDelta;
	f32 runningMul = 1;

	// if the 'exit' action was triggered, then halt engine loop
	if (m_controls.isAction("exit"))
	{
		application().setStopped(true);
	}

	if (m_controls.isAction("run"))
	{
		runningMul = cCameraRunSpeedMul;
	}

	m_crtSpeed = runningMul * cCameraMoveSpeed;

	if (m_controls.isAction("moveForward"))
	{
		actorMoveDelta += Vec3(0, 0, 1);
		bIsMoving = true;
	}

	if (m_controls.isAction("moveBackward"))
	{
		bIsMoving = true;
		actorMoveDelta += Vec3(0, 0, -1);
	}

	if (m_controls.isAction("moveLeft"))
	{
		bIsMoving = true;
		actorMoveDelta += Vec3(-1, 0, 0);
	}

	if (m_controls.isAction("moveRight"))
	{
		bIsMoving = true;
		actorMoveDelta += Vec3(1, 0, 0);
	}
	
	if (m_controls.isAction("saveGame"))
	{
		//saveGame("game00", "../data/save", 0, 0);
	}

	if (m_controls.isAction("loadGame"))
	{
		//loadGame("game00", "../data/save", 0, 0);
	}

	//resources().load<Material>("materials/default");

	//Camera* pCam = m_pCameraEntity->component<Camera>();
	//Transform* pTx = m_pCameraEntity->component<Transform>();

	//pCam->addYawAngle(-input().mouse()->delta().x / 10.0f);
	//pCam->addPitchAngle(-input().mouse()->delta().y / 10.0f);
	//pCam->onUpdate();
	
	//Matrix m = toMatrix(pTx->rotation());
	//
	//m.invert();
	//m.transform(actorMoveDelta, actorMoveDelta);
	//pTx->addTranslation(actorMoveDelta * m_crtSpeed * gameDeltaTime());
}