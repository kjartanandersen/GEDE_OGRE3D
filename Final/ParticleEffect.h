#pragma once
#include "ICallback.h"
#include "PlayerAvatar.h"

using namespace Ogre;
using namespace OgreBites;



struct DustParticle
{
	ParticleSystem* particle_system;
	bool effect_running;
	bool effect_finished;
	float duration_timer;
	
};

class FootstepParticle : public ICallback
{
public:
	FootstepParticle(SceneManager* scene_manager);
	~FootstepParticle();

	void callback(void* item);
	void update(float delta_time);
	bool isRunning();
	bool isFinished();
	

private:
	SceneManager* scene_manager_;
	
	SceneNode* entity_node_;
	unsigned int currID;
	float duration_seconds_;

	std::vector<DustParticle*> particles;
};

