#include "pch.h"
#include "MyEngine.h"


using namespace Ogre;
using namespace OgreBites;

	MyEngine::MyEngine() : ApplicationContext("T-637-GEDE Final")
	{
	}

	void MyEngine::setup()
	{
		ApplicationContext::setup();
		
		addInputListener(this);
		setupSceneManager();
		setupCamera();
		populateScene();
		setupInputManager();

		isController = false;

		audio_engine_ = new CAudioEngine();
		audio_engine_->Init();



	}

	bool MyEngine::keyPressed(const OgreBites::KeyboardEvent& evt)
	{
		// Check if Escape key is pressed
		if (evt.keysym.sym == OgreBites::SDLK_ESCAPE)
		{
			// Close program
			getRoot()->queueEndRendering();
		}
		return true;
	}

	void MyEngine::setupSceneManager()
	{
		// Get pointers to root and create scene manager
		root_ = getRoot();
		scene_manager_ = root_->createSceneManager();

		// Register scene with the RTSS
		RTShader::ShaderGenerator* shaderGenerator = RTShader::ShaderGenerator::getSingletonPtr();
		shaderGenerator->addSceneManager(scene_manager_);

		// Setup Resource Manager
		resource_manager_ = new GEDEResourceManager();
		
	}

	void MyEngine::setupCamera()
	{
		// Add camera

		roaming_camera_ = new RoamingCamera(scene_manager_, audio_engine_, getRenderWindow(), Vector3(0, 0, 0));


	}

	void MyEngine::populateScene()
	{

		// Add sinbad model
		//player_ = new PlayerAvatar(scene_manager_, "Sinbad.mesh");
		
		// Add sinbad 
		player_ = new PlayerSinbadAvatar(scene_manager_, resource_manager_, audio_engine_);
		
		// Add ground
		Plane plane(Ogre::Vector3::UNIT_Y, -5);
		MeshPtr groundMesh = MeshManager::getSingleton()
			.createPlane("MainGround", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
				plane, 1000, 1000, 100, 100, true, 1, 100, 100, Ogre::Vector3::UNIT_Z);

		// Build tangent vector for out mesh, to show the normal texture
		// This will make the floor tiles look like they have depth
		groundMesh->buildTangentVectors();

		Entity* groundEntity = scene_manager_->createEntity("LightPlaneEntity", "MainGround");
		scene_manager_->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity);
		groundEntity->setCastShadows(false);

		// Set material that is going to to be used to render the floor tiles
		groundEntity->setMaterialName("Custom/BrickTiles");

		// Add Ninja
		ninja_char = new MeshObject(scene_manager_, audio_engine_, resource_manager_, "ninja.mesh", Vector3(0, -5, -20));
		ninja_char->SetScale(0.05f, 0.05f, 0.05f);
		ninja_char->Rotate(00.0f, Vector3f(0.0f, 1.0f, 0.0f));
		ninja_char->CreateBodyPartAnimation("Body", "Attack1");
		ninja_char->AddCallbackAtTime("Attack1",ninja_char ,0.33f );
		ninja_char->AddCallbackAtTime("Attack1",ninja_char ,0.68f );

		// Add Ninja
		elephant_char = new MeshObject(scene_manager_, audio_engine_, resource_manager_, "elephant.mesh", Vector3(0, -5, -40));
		elephant_char->SetScale(3.00f, 3.00f, 3.00f);
		elephant_char->Rotate(60.0f, Vector3f(0.0f, 1.0f, 0.0f));

		// Set Shadow Technique
		scene_manager_->setShadowTechnique(Ogre::ShadowTechnique::SHADOWTYPE_STENCIL_MODULATIVE);


		// Add Directional Light
		scene_manager_->setAmbientLight(Ogre::ColourValue(0.2f, 0.5f, 0.8f));
		Light* directionalLight = scene_manager_->createLight("DirectionalLight");
		directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
		directionalLight->setDiffuseColour(1, 1, 1);
		directionalLight->setSpecularColour(.4, .4, .4);
		SceneNode* directionalLightNode = scene_manager_->getRootSceneNode()->createChildSceneNode();
		directionalLightNode->attachObject(directionalLight);
		directionalLightNode->setDirection(Ogre::Vector3(0, -1, -1));

		PickupManager::initialize(scene_manager_, player_->GetPlayerEntityNode());

	}

	bool MyEngine::frameStarted(const Ogre::FrameEvent& evt)
	{
		// Main game loop of the application

		// Let parent handle this callback as well
		ApplicationContext::frameStarted(evt);
		// Store the time since last frame
		const Ogre::Real delta_time = evt.timeSinceLastFrame;

		// Check what keys are being pressed
		const Uint8* state = SDL_GetKeyboardState(nullptr);

		const Ogre::Vector2 camera_direction = roaming_camera_->getDirection();
		const Ogre::Vector2 character_movement = input_manager->getCharacterMovement();



		// Update Input Manager
		if (input_manager != nullptr)
			input_manager->update();

		// Update any subsystem
		if (player_ != nullptr)
		{
			if (isController)
			{
				player_->Update(delta_time, camera_direction, character_movement, state);
			}
			else
			{
				player_->Update(delta_time, state);
			}

		}
		
		ninja_char->Update(delta_time);
		elephant_char->Update(delta_time);
		const Ogre::Vector3 player_postion = player_->GetPlayerPosition();
		const Ogre::Vector2 camera_movement = input_manager->getCameraMovement();

		if (roaming_camera_ != nullptr)
		{
			if (isController)
			{
				roaming_camera_->update(delta_time, camera_movement, player_postion);

			}
			else
			{
				roaming_camera_->update(delta_time, state);
			}

		}

		PickupManager::Update(delta_time, state);

		audio_engine_->Update();

		return true;
	}

	void MyEngine::setupInputManager()
	{

		input_manager = new InputManager();
	}

