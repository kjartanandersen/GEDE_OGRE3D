#include "pch.h"
#include "ParticleEffect.h"

ParticleEffect::ParticleEffect(SceneManager* scene_manager, SceneNode* scene_node)
{
	scene_manager_ = scene_manager;
	scene_node_ = scene_node;
	effect_finished_ = false;
	effect_running_ = false;

	duration_seconds_ = 15.0f;
	duration_timer_ = 0.0f;

	std::stringstream ss;
	ss << "burning" << scene_node_;

	ss.str("");
	ss.clear();
	ss << "smoke" << scene_node_;
	std::string effectName = ss.str();

	// TODO: chose an appropriate particle effect
	particle_effect_ = scene_manager_->createParticleSystem(effectName, "Examples/GreenyNimbus");
	scene_node_->attachObject(particle_effect_);
}

void ParticleEffect::run()
{
	effect_running_ = true;
}

void ParticleEffect::update(float delta_time)
{
	if (effect_running_)
	{
		duration_timer_ += delta_time;
		if (duration_timer_ >= duration_seconds_)
		{
			effect_finished_ = true;
			effect_running_ = false;
		}
	}
}

bool ParticleEffect::isRunning()
{
	return effect_running_;
}

bool ParticleEffect::isFinished()
{
	return effect_finished_;
}

ParticleEffect::~ParticleEffect()
{
	scene_manager_->destroyParticleSystem(particle_effect_);
}