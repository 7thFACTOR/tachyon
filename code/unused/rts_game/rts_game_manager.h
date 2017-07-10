#pragma once
#include "math/util.h"
#include "resources/font.h"
#include "resources/mesh.h"
#include "game/game_manager.h"
#include "input/input_map.h"

using namespace nytro;
class EditorProxy;

class RtsGame: public GameManager
{
public:
	N_RUNTIME_CLASS;
	
	void onInitialize();
	void onShutdown();
	void onUpdate();
	void onRender();
	void onInput();
	void onEnterGameState();
	void onExitGameState();

	InputMap m_controls;
	EntityId m_cameraEntityId;
	FontData* m_pFont;
	FontData* m_pSmallFont;
	GpuProgram* m_pDefaultGpuProgram;
	MeshResource* m_pMeshData;
	Texture* m_pRT;
	GeometryInstanceBuffer* m_pInstances;
	f32 m_crtSpeed;
};