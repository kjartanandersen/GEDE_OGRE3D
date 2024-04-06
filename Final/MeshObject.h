#pragma once

using namespace Ogre;


	class MeshObject
{



public:
	MeshObject(SceneManager* scene_manager, Ogre::String mesh_file_name,
		Vector3 position);
	~MeshObject();

	void Update(Ogre::Real delta_time);
	void SetScale(Ogre::Real xVal, Ogre::Real yVal, Ogre::Real zVal);
	void Rotate(Ogre::Real degrees, const Ogre::Vector3f& axis);

private:
	Entity* entity_;
	SceneNode* scene_node_;
	SceneManager* scene_manager_;

	AnimationState* anim_state;
};



