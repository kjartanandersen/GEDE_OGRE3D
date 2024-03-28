#pragma once
#include "pch.h"
#include "AudioEngine.h"

using namespace Ogre;
using namespace OgreBites;

class PlayerAvatar
{
public:
	SceneNode* entity_node_;

	PlayerAvatar(SceneManager* scene_manager, String mesh_file_name, CAudioEngine* audio_engine);

	//
	void Update(Ogre::Real delta_time, const Uint8* state);
	void Update(Ogre::Real delta_time, const Ogre::Vector2 camera_direction, const Ogre::Vector2 character_movement);

	//
	SceneNode* GetPlayerEntityNode();
	Ogre::Vector3 GetPlayerPosition();

private:
	SceneManager* scene_manager_;
	CAudioEngine* audio_engine_;
	
	
	AnimationState* animation_state_base_;
	AnimationState* animation_state_top_;

	Ogre::Real walking_speed_;

	Ogre::Real rotation_;
	Ogre::Real rotation_speed_;

	float speed_;

	bool isWalking;

	void Move(Ogre::Vector3 translate_vector, float rotation, Ogre::Real delta_time);
	Ogre::Radian GetRotation(const Ogre::Vector3& vec);
	void SetIdleAnimationLoop();
	void SetRunAnimatonLoop();
	void StopAnimationLoop(void) const;
	void StartAnimationLoop(void) const;
	

protected:
	Entity* entity_;
};

