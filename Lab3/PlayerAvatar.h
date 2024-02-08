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
	void Update(Ogre::Real delta_time, const Uint8* state);
	//
	SceneNode* GetPlayerEntityNode();

private:
	SceneManager* scene_manager_;
	Entity* entity_;
	
	AnimationState* animation_state_base_;
	AnimationState* animation_state_top_;

	Ogre::Real walking_speed_;

	Ogre::Real rotation_;
	Ogre::Real rotation_speed_;

	bool isWalking;

	void Move(Ogre::Vector3 translate_vector, float rotation, Ogre::Real delta_time);
	Ogre::Radian GetRotation(const Ogre::Vector3& vec);
	void SetIdleAnimationLoop();
	void SetRunAnimatonLoop();
	void StopAnimationLoop(void) const;
	void StartAnimationLoop(void) const;

};

