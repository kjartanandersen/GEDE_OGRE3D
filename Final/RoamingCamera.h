#pragma once
#include "pch.h"
#include "AudioEngine.h"

using namespace Ogre;
using namespace OgreBites;


	// ROAMING CAMERA
// Simple camera that can be moved around the scene using the mouse and keyboard (WASD)
// Activate the camera motion mode by holding down the right mouse button
	class RoamingCamera
	{
	public:
		// CONSTRUCTOR
		// scene_manager: The main scene manager
		// render_window: The window to which the camera is attached and will display within
		// position: The Initial position of the camera in world coordinates
		// lookat_position: A point in world coordinates that the camera is pointing at
		RoamingCamera(SceneManager* scene_manager, CAudioEngine* audio_engine, RenderWindow* render_window,
			Vector3 position, Vector3 lookat_position = Vector3(0, 0, 0));
		~RoamingCamera();
		// UPDATE
		// Updates position and rotation of the camera on time that has passed and any key presses
		// delta_time: Time since last update in seconds
		// state: Keyboard state, retrieved using SDL_GetKeyboardState
		void update(Ogre::Real delta_time, const Uint8* keyboard_state);
		void update(Ogre::Real delta_time, const Ogre::Vector2 camera_movement, const Ogre::Vector3 player_position);

		Ogre::Vector2 getDirection();
	private:
		SceneManager* scene_manager_;	// The main scene manager
		CAudioEngine* audio_engine_;		// The audio engine
		Camera* camera_;                // The camera itself
		SceneNode* camera_yaw_node_;    // The scene node that holds the camera
		SceneNode* camera_pitch_node_;    // The scene node that holds the camera

		float movement_speed_;          // How fast the camera moves when WASD keys are pressed
		float run_speed;
		float rotation_speed_;          // How fast the camera rotates when mouse is moved
		float camera_angle_;


	};

