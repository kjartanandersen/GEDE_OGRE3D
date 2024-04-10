#pragma once
#include "AnimationEvents.h"
#include "AudioEngine.h"
#include "ResourceManager.h"

using namespace Ogre;


	class MeshObject : public ICallback
{



public:
	MeshObject(SceneManager* scene_manager, CAudioEngine* audio_engine, GEDEResourceManager* resource_manager, Ogre::String mesh_file_name,
		Vector3 position);
	~MeshObject();

	void Update(Ogre::Real delta_time);
	void SetScale(Ogre::Real xVal, Ogre::Real yVal, Ogre::Real zVal);
	void Rotate(Ogre::Real degrees, const Ogre::Vector3f& axis);
	void CreateBodyPartAnimation(Ogre::String bodyPartName, Ogre::String animName);
	void callback(void* item);
	void AddCallbackAtTime(String animation_name, ICallback* callback_object, Ogre::Real time_);
	void SetShadow(bool val);

private:
	Entity* entity_;
	SceneNode* scene_node_;
	SceneManager* scene_manager_;
	CAudioEngine* audio_engine_;
	GEDEResourceManager* resource_manager_;

	// Animation Data
	std::map<std::string, MeshAnimationState*> animations_;
	std::map<std::string, MeshAnimation*> animation_states_;

	void CheckAnimationEvents();
	void PlaySounds();
};



