#include "pch.h"
#include "MyEngine.h"

using namespace Ogre;
using namespace OgreBites;

MyEngine::MyEngine() : ApplicationContext("T-637-GEDE Lab 2")
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
	camera_node_ = scene_manager_->getRootSceneNode()->createChildSceneNode();
	camera_ = scene_manager_->createCamera("MainCamera");

	camera_->setNearClipDistance(5);
	camera_->setAutoAspectRatio(true);

	camera_node_->setPosition(0, 10, 50);
	camera_node_->lookAt(Ogre::Vector3(0, 0, 0), Node::TransformSpace::TS_WORLD);
	camera_node_->attachObject(camera_);

	Viewport* vp = getRenderWindow()->addViewport(camera_);
	camera_->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

	// To match the BG color with the fog
	Ogre::ColourValue fadeColour(0.9, 0.9, 0.9);
	vp->setBackgroundColour(fadeColour);

	// Add fog that starts 50 units away from the camera, ends at 300 units, and increases in a linear fashion
	scene_manager_->setFog(Ogre::FOG_LINEAR, fadeColour, 0, 50, 300);
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

	camera_node_->setFixedYawAxis(true);
	camera_node_->setAutoTracking(true, player_->entity_node_);
}

bool MyEngine::frameStarted(const Ogre::FrameEvent& evt)
{
	ApplicationContext::frameStarted(evt);
	player_->Update(evt.timeSinceLastFrame);
	return true;
}