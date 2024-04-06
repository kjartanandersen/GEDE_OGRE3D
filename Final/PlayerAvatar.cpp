#include "pch.h"
#include "PlayerAvatar.h"


PlayerAvatar::PlayerAvatar(SceneManager* scene_manager, GEDEResourceManager* resource_manager, String mesh_file_name, CAudioEngine* audio_engine)
{
	scene_manager_ = scene_manager;
	resource_manager_ = resource_manager;
	audio_engine_ = audio_engine;
	entity_ = scene_manager_->createEntity(mesh_file_name);
	entity_->setCastShadows(true);
	entity_node_ = scene_manager_->getRootSceneNode()->createChildSceneNode();
	entity_node_->attachObject(entity_);



	// Base Idle Animation State
	MeshAnimation* baseAnim = new MeshAnimation;
	MeshAnimationState* baseIdleAnimState = new MeshAnimationState;
	baseIdleAnimState->animation = entity_->getAnimationState("IdleBase");
	// Base Run Animation State
	MeshAnimationState* baseRunAnimState = new MeshAnimationState;
	baseRunAnimState->animation = entity_->getAnimationState("RunBase");
	AnimationEvent* footstepEvent1 = new AnimationEvent;
	footstepEvent1->time = 0.1f;
	footstepEvent1->hasPlayed = false;
	
	AnimationEvent* footstepEvent2 = new AnimationEvent;
	footstepEvent2->time = 0.4f;
	footstepEvent2->hasPlayed = false;
	baseRunAnimState->events.push_back(footstepEvent1);
	
	baseRunAnimState->events.push_back(footstepEvent2);
	// Base Animation
	baseAnim->name = "Base";
	baseAnim->animStates.insert({ "Idle", baseIdleAnimState });
	baseAnim->animStates.insert({"Run", baseRunAnimState });
	baseAnim->curr_state = baseIdleAnimState;
	animations_.push_back(baseAnim);

	// Top Idle Animation State
	MeshAnimationState* topIdleAnimState = new MeshAnimationState;
	topIdleAnimState->animation = entity_->getAnimationState("IdleTop");
	// Top Run Animation State
	MeshAnimationState* topRunAnimState = new MeshAnimationState;
	topRunAnimState->animation = entity_->getAnimationState("RunTop");
	// Top Animation
	MeshAnimation* topAnim = new MeshAnimation;
	topAnim->name = "Top";
	topAnim->animStates.insert({"Idle",topIdleAnimState });
	topAnim->animStates.insert({"Run",topRunAnimState });
	topAnim->curr_state = topIdleAnimState;
	animations_.push_back(topAnim);
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

	//animation_state_base_->addTime(delta_time);
	//animation_state_top_->addTime(delta_time);

	for ( auto anim : animations_)
	{
		anim->curr_state->animation->addTime(delta_time);
	}
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

	for (auto anim : animations_)
	{
		anim->curr_state->animation->addTime(delta_time);
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
	for (auto anim : animations_)
	{
		if (anim->name == "Base" && anim->curr_state->animation->getAnimationName() != "IdleBase")
		{
			StopAnimationLoop();
			anim->curr_state = anim->animStates.find("Idle")->second;
			StartAnimationLoop();
		}

		if (anim->name == "Top" && anim->curr_state->animation->getAnimationName() != "IdleTop")
		{
			StopAnimationLoop();
			anim->curr_state = anim->animStates.find("Idle")->second;
			StartAnimationLoop();
		}
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
	for (auto anim : animations_)
	{
		if (anim->name == "Base" && anim->curr_state->animation->getAnimationName() != "RunBase")
		{
			StopAnimationLoop();
			anim->curr_state = anim->animStates.find("Run")->second;
			StartAnimationLoop();
		}

		if (anim->name == "Top" && anim->curr_state->animation->getAnimationName() != "RunTop")
		{
			StopAnimationLoop();
			anim->curr_state = anim->animStates.find("Run")->second;
			StartAnimationLoop();
		}
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
	for (auto anim : animations_)
	{
		if (anim->name == "Base" && anim->curr_state->animation->getEnabled())
		{
			anim->curr_state->animation->setTimePosition(0.0f);
			anim->curr_state->animation->setEnabled(false);
			
		}

		if (anim->name == "Top" && anim->curr_state->animation->getEnabled())
		{
			anim->curr_state->animation->setTimePosition(0.0f);
			anim->curr_state->animation->setEnabled(false);

		}
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
	for (auto anim : animations_)
	{
		if (anim->name == "Base" && !anim->curr_state->animation->getEnabled())
		{
			anim->curr_state->animation->setLoop(true);
			anim->curr_state->animation->setEnabled(true);
			for (auto event_ : anim->curr_state->events)
			{
				event_->hasPlayed = false;
			}
		}

		if (anim->name == "Top" && !anim->curr_state->animation->getEnabled())
		{
			anim->curr_state->animation->setLoop(true);
			anim->curr_state->animation->setEnabled(true);
			for (auto event_ : anim->curr_state->events)
			{
				event_->hasPlayed = false;
			}
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
	for (auto anim : animations_)
	{
		Ogre::Real anim_time = anim->curr_state->animation->getTimePosition();
		for (auto event_ : anim->curr_state->events)
		{
			if (anim_time <= 0.1)
				event_->hasPlayed = false;
			if (anim_time >= event_->time && !event_->hasPlayed)
			{
				PlayFootstepSounds();
				event_->hasPlayed = true;
			}
		}

	}
}

Ogre::Vector3 PlayerAvatar::GetPlayerPosition()
{
	return entity_node_->getPosition();
}