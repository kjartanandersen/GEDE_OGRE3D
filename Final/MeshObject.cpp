#include "pch.h"
#include "MeshObject.h"
#include "AnimationEvents.h"


MeshObject::MeshObject(SceneManager* scene_manager, CAudioEngine* audio_engine, GEDEResourceManager* resource_manager, Ogre::String mesh_file_name,
	Vector3 position)
{
	scene_manager_ = scene_manager;
	entity_ = scene_manager_->createEntity(mesh_file_name);
	scene_node_ = scene_manager_->getRootSceneNode()->createChildSceneNode();
	entity_->setCastShadows(false);
	audio_engine_ = audio_engine;
	resource_manager_ = resource_manager;


	scene_node_->attachObject(entity_);
	scene_node_->setPosition(position);

	for (auto animState : entity_->getAllAnimationStates()->getAnimationStates())
	{
		MeshAnimationState* meshAnimState = new MeshAnimationState;
		meshAnimState->animation = animState.second;
		animations_.emplace(animState.first, meshAnimState);
	}
		

}

MeshObject::~MeshObject()
{

}

void MeshObject::Update(Ogre::Real delta_time)
{
	for (auto anim : animation_states_)
	{
		anim.second->curr_state->animation->addTime(delta_time);
	}
	CheckAnimationEvents();
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

void MeshObject::CreateBodyPartAnimation(Ogre::String bodyPartName, Ogre::String animName)
{
	MeshAnimation* topAnim = new MeshAnimation;

	MeshAnimationState* idleTopState = new MeshAnimationState;
	idleTopState = animations_.find(animName)->second;

	topAnim->curr_state = idleTopState;
	topAnim->curr_state->animation->setEnabled(true);
	topAnim->curr_state->animation->setLoop(true);

	// Base Animation


	animation_states_.emplace(bodyPartName, topAnim);


}

void MeshObject::callback(void* callback)
{
	PlaySounds();
} 

void MeshObject::CheckAnimationEvents()
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
						callback->callback(0);
				}
				event_->hasPlayed = true;
			}
		}


	}
}


void MeshObject::PlaySounds()
{
	audio_engine_->LoadSound(resource_manager_->GetSoundsPath() + "whiff.mp3", true, false, false);
	audio_engine_->PlaySoundAtPosition(resource_manager_->GetSoundsPath() + "whiff.mp3", scene_node_->_getDerivedPosition(), false);

}

void MeshObject::AddCallbackAtTime(String animation_name, ICallback* callback_object, Ogre::Real time_)
{
	AnimationEvent* animEvent = new AnimationEvent;
	animEvent->callbacks.push_back(callback_object);
	animEvent->hasPlayed = false;
	animEvent->time = time_;


	animations_.find(animation_name)->second->events.push_back(animEvent);
}

void MeshObject::SetShadow(bool val)
{
	entity_->setCastShadows(val);
}