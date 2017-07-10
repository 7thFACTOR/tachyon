#pragma once

#include "stdafx.h"

using namespace Nytro;
using namespace Engine;

// a game state handler, this one is the in-game state
class MyInGameStateHandler : public GameStateHandler
{
public:

	MyInGameStateHandler();
	// called when this game state must be initialized
	void onInitialize();
	// called when this game state must be released
	void onRelease();
	// called when the user enters in this game state
	void onEnter();
	// called when the user exits this game state
	void onExit();
	// called every frame when this game state is the current one
	void onFrame();
	// called when the engine has input data to be processed by this current game state
	void onInput(InputDevice* pDevice);

private:

	// an input map action controls, loaded from an xml file
	InputMap m_controls;
	// a 3D model
	ModelNode* m_pModel;
	// a camera used for viewing the model
	CameraNode* m_pCamera;
	// a First person shooter camera controller used for effects like walking bounce, and others
	FPSCameraController m_fpsCameraController;
	SkeletonAnimationSet* m_pAnimSet;
	SkeletonAnimation m_anim;

	Canvas* m_pCanvas;
	vector<ModelNode*>      m_pieces;
	vector<PhysicsBody*>    m_bodies;
	PhysicsSpace* m_pSpace;
	PhysicsBody* m_pBody;
	ParticleSystemNode* m_pShootPS;
	Vector3D trans, dir;
	SoundSourceNode* m_pSound;
	SoundSourceNode* m_pFireSound;
	SoundSourceNode* m_pAmbSound;
	TextRenderCache textCache[100];
	UiManager ui;
	UiSkin skin;
	UiLayoutData* m_pUiLayout;
};

// our actual game module ( or provider )
class MyGame : public GameProvider
{
public:

	// called by the engine when the game must be initialized
	Result initialize();
	// called by the engine when the game must be released
	Result release();
};