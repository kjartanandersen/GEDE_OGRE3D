#pragma once

#include "PlayerAvatar.h"
#include "PlayerSinbadAvatar.h"
#include "MeshObject.h"
#include "RoamingCamera.h"
#include "PickupManager.h"
#include "InputManager.h"
#include "AudioEngine.h"
#include "OgreAssimpLoader.h"
#include "ResourceManager.h"


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

		void setupInputManager();

	private:
		// OGRE root 
		Ogre::Root* root_;
		// Scene Manager
		Ogre::SceneManager* scene_manager_;
		GEDEResourceManager* resource_manager_;
		CAudioEngine* audio_engine_;
		// Camera
		RoamingCamera* roaming_camera_;

		//
		PlayerSinbadAvatar* player_;
		

		InputManager* input_manager;
		// Songbird
		MeshObject* ninja_char;
		MeshObject* elephant_char;


		bool isController;
	};

