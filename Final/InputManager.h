#pragma once
class InputManager
{

public:
	InputManager();
	~InputManager();

	void update();
	Ogre::Vector2 getCameraMovement() const;
	Ogre::Vector2 getCharacterMovement() const;

private:
	
	

	SDL_Joystick* joystick_;
	int player_axis_x_;
	int player_axis_y_;
	int camera_x_;
	int camera_y;

};

