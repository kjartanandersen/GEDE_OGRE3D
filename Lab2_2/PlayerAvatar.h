#pragma once
#include "pch.h"

using namespace Ogre;
using namespace OgreBites;

class PlayerAvatar
{
public:
	SceneNode* entity_node_;

	PlayerAvatar(SceneManager* scene_manager, String mesh_file_name);

	//
	void Update(Ogre::Real delta_time);
	//


private:
	SceneManager* scene_manager_;
	Entity* entity_;
	
	AnimationState* animation_state_base_;
	AnimationState* animation_state_top_;

	void Move(Ogre::Vector3 translate_vector, Ogre::Real delta_time);
	Ogre::Radian GetRotation(const Ogre::Vector3& vec);
	void SetIdleAnimationLoop();
	void SetRunAnimatonLoop();
	void StopAnimationLoop(void) const;
	void StartAnimationLoop(void) const;

};

