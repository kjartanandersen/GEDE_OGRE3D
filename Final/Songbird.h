#pragma once

using namespace Ogre;

class Songbird
{

	

public:
	Songbird(SceneManager* scene_manager, Ogre::String mesh_file_name,
		Vector3 position);
	~Songbird();

private:
	Entity* entity_;
	SceneNode* scene_node_;
	SceneManager* scene_manager_;
};

