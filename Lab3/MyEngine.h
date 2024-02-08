#pragma once
#include "pch.h"
#include "PlayerAvatar.h"
#include "RoamingCamera.h"
#include "PickupManager.h"

using namespace Ogre;
using namespace OgreBites;

class MyEngine : public ApplicationContext, public InputListener
{
public:
	// Engine Contructor
	MyEngine();
	virtual ~MyEngine() {};
	// Engine setup function
	void setup();
	// Set up key stroke event listener
	bool keyPressed(const OgreBites::KeyboardEvent& evt);
	// Setup scene and shader
	void setupSceneManager();
	// Setup camera
	void setupCamera();
	//
	void populateScene();
	//
	bool frameStarted(const Ogre::FrameEvent& evt);

private:
	// OGRE root 
	Ogre::Root* root_;
	// Scene Manager
	Ogre::SceneManager* scene_manager_;
	// Camera
	RoamingCamera* roaming_camera_;
	
	//
	PlayerAvatar* player_;
};