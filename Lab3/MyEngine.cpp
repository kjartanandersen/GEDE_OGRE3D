#include "pch.h"
#include "MyEngine.h"

using namespace Ogre;
using namespace OgreBites;

MyEngine::MyEngine() : ApplicationContext("T-637-GEDE Lab 3")
{
}

void MyEngine::setup()
{
	ApplicationContext::setup();
	addInputListener(this);
	setupSceneManager();
	setupCamera();
	populateScene();

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
}

void MyEngine::setupCamera()
{
	// Add camera

	roaming_camera_ = new RoamingCamera(scene_manager_, getRenderWindow(), Vector3(0, 0, 50));

	
}

void MyEngine::populateScene()
{
	// Add sinbad model
	player_ = new PlayerAvatar(scene_manager_, "Sinbad.mesh");

	// Add ground
	Plane plane(Ogre::Vector3::UNIT_Y, -5);
	MeshPtr groundMesh = MeshManager::getSingleton()
		.createPlane("MainGround", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			plane, 1000, 1000, 100, 100, true, 1, 50, 50, Ogre::Vector3::UNIT_Z);

	// Build tangent vector for out mesh, to show the normal texture
	// This will make the floor tiles look like they have depth
	groundMesh->buildTangentVectors();

	Entity* groundEntity = scene_manager_->createEntity("LightPlaneEntity", "MainGround");
	scene_manager_->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity);
	groundEntity->setCastShadows(false);

	// Set material that is going to to be used to render the floor tiles
	groundEntity->setMaterialName("Custom/BrickTiles");

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
	PickupManager::addPickupObject("cube.mesh");
	PickupManager::addPickupObject("cube.mesh");
	PickupManager::addPickupObject("cube.mesh");
	PickupManager::addPickupObject("cube.mesh");
	PickupManager::addPickupObject("cube.mesh");
	PickupManager::addPickupObject("cube.mesh");

	
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

	// Update any subsystem
	if (player_ != nullptr)
		player_->Update(delta_time, state);

	if (roaming_camera_ != nullptr)
		roaming_camera_->update(delta_time, state);

	PickupManager::Update(delta_time, state);


	return true;
}