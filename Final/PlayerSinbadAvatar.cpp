#include "pch.h"
#include "PlayerSinbadAvatar.h"

PlayerSinbadAvatar::PlayerSinbadAvatar(SceneManager* scene_manager, CAudioEngine* audio_engine) : PlayerAvatar(scene_manager, "Sinbad.mesh", audio_engine)
{
	isHandsClosed = false;

	entity_->getSubEntity(1)->setMaterialName("Sinbad/Body2");
	

	sword1Entity_ = scene_manager->createEntity("Sword.mesh");
	sword2Entity_ = scene_manager->createEntity("Sword.mesh");

	entity_->attachObjectToBone("Handle.L", sword1Entity_);
	entity_->attachObjectToBone("Handle.R", sword2Entity_);

	
}

void PlayerSinbadAvatar::Update(Ogre::Real delta_time, Ogre::Vector2 camera_direction, Ogre::Vector2 character_movement, const Uint8* state)
{
	PlayerAvatar::Update(delta_time, camera_direction, character_movement);

	if (state[SDL_SCANCODE_R])
	{
		isHandsClosed = !isHandsClosed;
	}

	if (isHandsClosed)
	{
		SetCloseHandsAmin();
	}
	else
	{
		SetOpenHandsAmin();
	}

	
}

void PlayerSinbadAvatar::Update(Ogre::Real delta_time, const Uint8* state)
{
	PlayerAvatar::Update(delta_time, state);

	if (state[SDL_SCANCODE_R])
	{
		isHandsClosed = !isHandsClosed;
	}

	if (isHandsClosed)
	{
		SetCloseHandsAmin();
	}
	else
	{
		SetOpenHandsAmin();
	}


}

void PlayerSinbadAvatar::SetOpenHandsAmin()
{
	if (hand_animation_state_ == nullptr)
	{
		hand_animation_state_ = entity_->getAnimationState("HandsRelaxed");
		StartHandAnimLoop();
	}
	else if (hand_animation_state_->getAnimationName() != "HandsRelaxed")
	{
		StopHandAnimLoop();
		hand_animation_state_ = entity_->getAnimationState("HandsRelaxed");
		StartHandAnimLoop();
	}
}

void PlayerSinbadAvatar::SetCloseHandsAmin()
{
	if (hand_animation_state_ == nullptr)
	{
		hand_animation_state_ = entity_->getAnimationState("HandsClosed");
		StartHandAnimLoop();
	}
	else if (hand_animation_state_->getAnimationName() != "HandsClosed")
	{
		StopHandAnimLoop();
		hand_animation_state_ = entity_->getAnimationState("HandsClosed");
		StartHandAnimLoop();
	}
}

void PlayerSinbadAvatar::StartHandAnimLoop(void) const
{
	if (hand_animation_state_ != nullptr &&
		!hand_animation_state_->getEnabled())
	{
		hand_animation_state_->setLoop(true);
		hand_animation_state_->setEnabled(true);
	}
}

void PlayerSinbadAvatar::StopHandAnimLoop(void) const
{
	if (hand_animation_state_ != nullptr &&
		hand_animation_state_->getEnabled())
	{
		hand_animation_state_->setTimePosition(0.0f);
		hand_animation_state_->setEnabled(false);
	}
}