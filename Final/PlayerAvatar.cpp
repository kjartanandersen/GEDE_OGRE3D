#include "pch.h"
#include "PlayerAvatar.h"
#include <string>

PlayerAvatar::PlayerAvatar(SceneManager* scene_manager, GEDEResourceManager* resource_manager, String mesh_file_name, CAudioEngine* audio_engine)
{
	scene_manager_ = scene_manager;
	resource_manager_ = resource_manager;
	audio_engine_ = audio_engine;
	entity_ = scene_manager_->createEntity(mesh_file_name);
	entity_->setCastShadows(true);
	entity_node_ = scene_manager_->getRootSceneNode()->createChildSceneNode();
	entity_node_->attachObject(entity_);
	feet_position_ = new Vector3(entity_node_->getPosition());
	prev_frame_anim_time = 0.0f;
	


	for (auto animState : entity_->getAllAnimationStates()->getAnimationStates())
	{
		MeshAnimationState* meshAnimState = new MeshAnimationState;
		meshAnimState->animation = animState.second;
		animations_.emplace(animState.first, meshAnimState);
	}
	

	// Base Idle Animation State
	MeshAnimation* baseAnim = new MeshAnimation;

	// Base Run Animation State
	MeshAnimationState* idleRunAnimState = new MeshAnimationState;
	idleRunAnimState->animation = animations_.find("IdleBase")->second->animation;

	baseAnim->curr_state = idleRunAnimState;
	

	MeshAnimation* topAnim = new MeshAnimation;

	MeshAnimationState* idleTopState = new MeshAnimationState;
	idleTopState->animation = animations_.find("IdleTop")->second->animation;

	topAnim->curr_state = idleTopState;

	
	// Base Animation

	
	animation_states_.emplace("Base", baseAnim);
	animation_states_.emplace("Top", topAnim);

	AddCallbackAtTime("RunBase", this, 0.3f);
	AddCallbackAtTime("RunBase", this, 0.6f);
	
	footstep_particle_ = new FootstepParticle(scene_manager_);
	AddCallbackAtTime("RunBase", footstep_particle_, 0.3f);
	AddCallbackAtTime("RunBase", footstep_particle_, 0.6f);

	
	StartAnimationLoop();


	walking_speed_ = 5.0f;

	rotation_ = 0.0;
	rotation_speed_ = 5.0f;
	isWalking = false;

	speed_ = 4.0f;
}

void PlayerAvatar::Update(Ogre::Real delta_time, const Uint8* state)
{


	// Leave if right mouse button is being held down
	if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_RMASK)
		return;

	// Construct the movement
	Ogre::Vector3 translate_vector(0, 0, 0);
	Ogre::Vector3 dir(sin(rotation_), 0, cos(rotation_));

	isWalking = false;

	if (state[SDL_SCANCODE_W]) {
		translate_vector = walking_speed_ * dir;
		isWalking = true;
	}
	if (state[SDL_SCANCODE_S]) {
		translate_vector = walking_speed_ * -1 * dir;
		isWalking = true;
	}
	if (state[SDL_SCANCODE_A]) {
		rotation_ += delta_time * rotation_speed_;
	}
	if (state[SDL_SCANCODE_D]) {
		rotation_ -= delta_time * rotation_speed_;
	}

	Move(translate_vector, rotation_, delta_time);

	if (isWalking)
	{
		SetRunAnimatonLoop();

		//Vector3f playerPos = GetPlayerPosition();

		
	}
	else
	{
		SetIdleAnimationLoop();

	}

	CheckAnimationEvents();
	footstep_particle_->update(delta_time);
	//animation_state_base_->addTime(delta_time);
	//animation_state_top_->addTime(delta_time);

	

	animation_states_.find("Base")->second->curr_state->animation->addTime(delta_time);
	animation_states_.find("Top")->second->curr_state->animation->addTime(delta_time);
	feet_position_->x = entity_node_->getPosition().x;
	feet_position_->y = entity_node_->getPosition().y - 6.0f;
	feet_position_->z = entity_node_->getPosition().z;
}

void PlayerAvatar::Update(Ogre::Real delta_time, const Ogre::Vector2 camera_direction, const Ogre::Vector2 character_movement)
{
	Ogre::Vector3 translate(camera_direction.x * character_movement.y, 0, camera_direction.y * character_movement.y);
	translate = Ogre::Vector3(translate.x + camera_direction.y * character_movement.x, 0, translate.z - camera_direction.x * character_movement.x);
	translate = translate.normalisedCopy();
	translate = translate * speed_;

	// Keep player facing away from the camera
	rotation_ = (GetRotation(Ogre::Vector3(camera_direction.x, 0, camera_direction.y)) + Ogre::Radian(Ogre::Math::PI)).valueRadians();

	Move(translate, rotation_, delta_time);

	if (!translate.isZeroLength())
	{
		SetRunAnimatonLoop();
	}
	else
	{
		SetIdleAnimationLoop();
	}
	//animation_state_base_->addTime(delta_time);
	//animation_state_top_->addTime(delta_time);

	for (auto anim : animation_states_)
	{
		anim.second->curr_state->animation->addTime(delta_time);
	}
}

void PlayerAvatar::PlayFootstepSounds()
{
	
	audio_engine_->LoadSound(resource_manager_->GetSoundsPath() + "footstep.ogg", true, false, false);
	audio_engine_->PlaySoundAtPosition(resource_manager_->GetSoundsPath() + "footstep.ogg", entity_node_->_getDerivedPosition(), false);

}

SceneNode* PlayerAvatar::GetPlayerEntityNode()
{
	return entity_node_;
}

void PlayerAvatar::Move(Ogre::Vector3 translate_vector, float rotation, Ogre::Real delta_time)
{

	entity_node_->translate(translate_vector * delta_time * walking_speed_);
	entity_node_->resetOrientation();
	// Rotate the player to face in the direction of the translation
	entity_node_->yaw(Ogre::Radian(rotation));
}

Ogre::Radian PlayerAvatar::GetRotation(const Ogre::Vector3& vec)
{
	Ogre::Vector3 v(vec);
	v.normalise();
	return((v.x <= 0 ? -1.0 : 1.0) * Ogre::Radian(acos(v.z)));
}

void PlayerAvatar::SetIdleAnimationLoop()
{
	if (animation_states_.find("Base")->second->curr_state->animation->getAnimationName() != "IdleBase")
	{
		StopAnimationLoop();
		animation_states_.find("Base")->second->curr_state = animations_.find("IdleBase")->second;
		animation_states_.find("Base")->second->curr_state = animations_.find("IdleBase")->second;
		StartAnimationLoop();
	}

	if (animation_states_.find("Top")->second->curr_state->animation->getAnimationName() != "IdleTop")
	{
		StopAnimationLoop();
		animation_states_.find("Top")->second->curr_state = animations_.find("IdleTop")->second;
		animation_states_.find("Top")->second->curr_state = animations_.find("IdleTop")->second;
		StartAnimationLoop();
	}

	/*
	if (animation_state_top_ == nullptr)
	{
		animation_state_base_ = entity_->getAnimationState("IdleBase");
		animation_state_top_ = entity_->getAnimationState("IdleTop");
		StartAnimationLoop();

	}
	// Stop other animation if there is any already running
	else if (animation_state_top_->getAnimationName() != "IdleTop")
	{
		StopAnimationLoop();
		animation_state_base_ = entity_->getAnimationState("IdleBase");
		animation_state_top_ = entity_->getAnimationState("IdleTop");
		StartAnimationLoop();
	}
	*/
}

void PlayerAvatar::SetRunAnimatonLoop()
{
	if (animation_states_.find("Base")->second->curr_state->animation->getAnimationName() != "RunBase")
	{
		StopAnimationLoop();
		animation_states_.find("Base")->second->curr_state = animations_.find("RunBase")->second;
		StartAnimationLoop();
	}

	if (animation_states_.find("Top")->second->curr_state->animation->getAnimationName() != "RunTop")
	{
		StopAnimationLoop();
		animation_states_.find("Top")->second->curr_state = animations_.find("RunTop")->second;
		StartAnimationLoop();
	}
	


	/*
	if (animation_state_top_ == nullptr)
	{
		animation_state_base_ = entity_->getAnimationState("RunBase");
		animation_state_top_ = entity_->getAnimationState("RunTop");
		StartAnimationLoop();

	}
	// Stop other animation if there is any already running
	else if (animation_state_top_->getAnimationName() != "RunTop")
	{
		StopAnimationLoop();
		animation_state_base_ = entity_->getAnimationState("RunBase");
		animation_state_top_ = entity_->getAnimationState("RunTop");
		StartAnimationLoop();
	}
	*/
	/*
	Ogre::Real anim_time_pos = animation_state_base_->getTimePosition();
	std::cout << anim_time_pos << std::endl;

	if ( anim_time_pos < 0.3)
		playSound = false;

	if (anim_time_pos >= 0.3 && !playSound)
	{
		playSound = true;
		audio_engine_->LoadSound("C:\\Ogre\\GEDE\\GEDE\\LabFiles\\Sounds\\footstep.ogg", true, false, false);
		audio_engine_->PlaySound("C:\\Ogre\\GEDE\\GEDE\\LabFiles\\Sounds\\footstep.ogg", entity_node_->_getDerivedPosition(), false);

	}
	*/

}

void PlayerAvatar::StopAnimationLoop(void) const
{

	if (animation_states_.find("Base")->second->curr_state->animation->getEnabled())
	{
		animation_states_.find("Base")->second->curr_state->animation->setTimePosition(0.0f);
		animation_states_.find("Base")->second->curr_state->animation->setEnabled(false);
	}

	if (animation_states_.find("Top")->second->curr_state->animation->getEnabled())
	{
		animation_states_.find("Top")->second->curr_state->animation->setTimePosition(0.0f);
		animation_states_.find("Top")->second->curr_state->animation->setEnabled(false);
	}

	/*
	if (animation_state_base_ != nullptr &&
		animation_state_base_->getEnabled())
	{
		animation_state_base_->setTimePosition(0.0f);
		animation_state_base_->setEnabled(false);
	}
	if (animation_state_top_ != nullptr &&
		animation_state_top_->getEnabled())
	{
		animation_state_top_->setTimePosition(0.0f);
		animation_state_top_->setEnabled(false);
	}
	*/
}

void PlayerAvatar::StartAnimationLoop(void) const
{

	if (!animation_states_.find("Base")->second->curr_state->animation->getEnabled())
	{
		animation_states_.find("Base")->second->curr_state->animation->setLoop(true);
		animation_states_.find("Base")->second->curr_state->animation->setEnabled(true);
		for (auto event_ : animation_states_.find("Base")->second->curr_state->events)
		{
			event_->hasPlayed = false;
		}
	}

	if (!animation_states_.find("Top")->second->curr_state->animation->getEnabled())
	{
		animation_states_.find("Top")->second->curr_state->animation->setLoop(true);
		animation_states_.find("Top")->second->curr_state->animation->setEnabled(true);
		for (auto event_ : animation_states_.find("Top")->second->curr_state->events)
		{
			event_->hasPlayed = false;
		}
	}

	/*
	if (animation_state_base_ != nullptr &&
		!animation_state_base_->getEnabled())
	{
		animation_state_base_->setLoop(true);
		animation_state_base_->setEnabled(true);
	}
	if (animation_state_top_ != nullptr &&
		!animation_state_top_->getEnabled())
	{
		animation_state_top_->setLoop(true);
		animation_state_top_->setEnabled(true);
	}
	*/
}

void PlayerAvatar::CheckAnimationEvents()
{
	for (auto anim : animation_states_)
	{
		Ogre::Real anim_time = anim.second->curr_state->animation->getTimePosition();
		for (auto event_ : anim.second->curr_state->events)
		{
			if (anim_time < 0.1 && event_->hasPlayed == true)
				event_->hasPlayed = false;
			if (anim_time >= event_->time && !event_->hasPlayed)
			{
				for (auto callback : event_->callbacks)
				{

					if (callback)
					callback->callback(feet_position_);
				}
				event_->hasPlayed = true;
			}
		}


	}
	
}

Ogre::Vector3 PlayerAvatar::GetPlayerPosition()
{
	return entity_node_->getPosition();
}

void PlayerAvatar::AddCallbackAtTime(String animation_name, ICallback* callback_object, Ogre::Real time_)
{
	AnimationEvent* animEvent = new AnimationEvent;
	animEvent->callbacks.push_back(callback_object);
	animEvent->hasPlayed = false;
	animEvent->time = time_;


	animations_.find(animation_name)->second->events.push_back(animEvent);
}

void PlayerAvatar::callback(void* item)
{
	PlayFootstepSounds();
}