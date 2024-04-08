#include "pch.h"
#include "ParticleEffect.h"


FootstepParticle::FootstepParticle(SceneManager* scene_manager)
{
	scene_manager_ = scene_manager;
	

	entity_node_ = scene_manager_->getRootSceneNode()->createChildSceneNode();
	currID = 0;
	duration_seconds_ = 1.0f;
	// TODO: chose an appropriate particle effect
	
}

void FootstepParticle::callback(void* item)
{
	DustParticle* particle = new DustParticle;
	particle->effect_running = true;
	particle->duration_timer = 0.0f;
	particle->effect_finished = false;

	std::stringstream ss;
	ss << "footstepParticle" << currID;

	

	std::string particleName = ss.str();
	particle->particle_system = scene_manager_->createParticleSystem(particleName, "MySmoke1");
	currID++;
	entity_node_->attachObject(particle->particle_system);
	particles.push_back(particle);
	Vector3* vecItem = (Vector3*)item;
	entity_node_->setPosition(vecItem->x, vecItem->y, vecItem->z);
	
}

void FootstepParticle::update(float delta_time)
{

	for (auto particle : particles)
	{
		if (particle->effect_running)
		{
			particle->duration_timer += delta_time;
			if (particle->duration_timer >= duration_seconds_)
			{
				scene_manager_->destroyParticleSystem(particle->particle_system);
				particles.erase(std::remove(particles.begin(), particles.end(), particle), particles.end());
				delete particle;
			}
		}
	}

	
}



FootstepParticle::~FootstepParticle()
{
	for (auto particle : particles)
	{
		scene_manager_->destroyParticleSystem(particle->particle_system);
	}

	
}