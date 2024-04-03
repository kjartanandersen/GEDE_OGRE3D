#include "pch.h"
#include "Songbird.h"

Songbird::Songbird(SceneManager* scene_manager, Ogre::String mesh_file_name,
	Vector3 position)
{
	scene_manager_ = scene_manager;
	entity_ = scene_manager_->createEntity(mesh_file_name);
	scene_node_ = scene_manager_->getRootSceneNode()->createChildSceneNode();

	
	Ogre::AnimationStateSet* ent_state_set = entity_->getAllAnimationStates();

	Ogre::AnimationStateMap ent_state_map = ent_state_set->getAnimationStates();

	for (auto ent : ent_state_map)
	{
		std::cout << "Animation State Name: " << ent.first << std::endl;
		
	}
	
	
	scene_node_->attachObject(entity_);
	scene_node_->setPosition(position);
	scene_node_->setScale(1, 1, 1);

	const Ogre::Radian rad = Radian(90.0f);

	Ogre::Quaternion quat = Quaternion(rad, Vector3f(1.0f, 0.0f, 0.0f));

	scene_node_->rotate(quat);
	entity_->setCastShadows(true);

	anim_state = entity_->getAnimationState("Take 001");
	anim_state->setLoop(true);
	anim_state->setEnabled(true);

}

Songbird::~Songbird()
{

}

void Songbird::Update(Ogre::Real delta_time)
{
	anim_state->addTime(delta_time);
}