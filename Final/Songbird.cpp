#include "pch.h"
#include "Songbird.h"

Songbird::Songbird(SceneManager* scene_manager, Ogre::String mesh_file_name,
	Vector3 position)
{
	scene_manager_ = scene_manager;
	entity_ = scene_manager_->createEntity(mesh_file_name);
	scene_node_ = scene_manager_->getRootSceneNode()->createChildSceneNode();
	scene_node_->attachObject(entity_);
	scene_node_->setPosition(position);
	scene_node_->setScale(500, 500, 500);
	entity_->setCastShadows(true);

}

Songbird::~Songbird()
{

}