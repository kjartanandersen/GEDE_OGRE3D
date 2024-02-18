#pragma once

#include "PlayerAvatar.h"

class PlayerSinbadAvatar : public PlayerAvatar
{
public:
	
	PlayerSinbadAvatar(SceneManager* scene_manager);
	void Update(Ogre::Real delta_time, const Uint8* state);


private:
	Entity* sword1Entity_;
	Entity* sword2Entity_;

	// Hand amination 
	AnimationState* hand_animation_state_;

	bool isHandsClosed;

	void SetOpenHandsAmin();
	void SetCloseHandsAmin();

	void StartHandAnimLoop(void) const;
	void StopHandAnimLoop(void) const;
};

