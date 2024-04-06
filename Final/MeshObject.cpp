#include "pch.h"
#include "MeshObject.h"
#include "AnimationEvents.h"


	MeshObject::MeshObject(SceneManager* scene_manager, Ogre::String mesh_file_name,
		Vector3 position)
	{
		scene_manager_ = scene_manager;
		entity_ = scene_manager_->createEntity(mesh_file_name);
		scene_node_ = scene_manager_->getRootSceneNode()->createChildSceneNode();
		entity_->setCastShadows(false);

		Ogre::AnimationStateSet* ent_state_set = entity_->getAllAnimationStates();

		Ogre::AnimationStateMap ent_state_map = ent_state_set->getAnimationStates();

		for (auto ent : ent_state_map)
		{
			std::cout << "Animation State Name: " << ent.first << std::endl;

		}


		scene_node_->attachObject(entity_);
		scene_node_->setPosition(position);


		anim_state = entity_->getAnimationState("Attack1");
		anim_state->setLoop(true);
		anim_state->setEnabled(true);

	}

	MeshObject::~MeshObject()
	{

	}

	void MeshObject::Update(Ogre::Real delta_time)
	{
		anim_state->addTime(delta_time / 10.0f);
	}

	void MeshObject::SetScale(Ogre::Real xVal, Ogre::Real yVal, Ogre::Real zVal)
	{
		scene_node_->setScale(xVal, yVal, zVal);
	}

	void MeshObject::Rotate(Ogre::Real degrees, const Ogre::Vector3f& axis)
	{
		const Ogre::Radian rad = Radian(degrees);

		//Ogre::Quaternion quat = Quaternion(deg, axis);

		scene_node_->rotate(axis, rad);
	}

