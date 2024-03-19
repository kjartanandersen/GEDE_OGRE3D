#pragma once
#include "pch.h"
#include "IPickupEffect.h"

using namespace Ogre;
using namespace OgreBites;

class ParticleEffect : public IPickupEffect
{
public:
    ParticleEffect(SceneManager* scene_manager, SceneNode* scene_node);
    ~ParticleEffect();
    // Inherited via IPickupEffect
    void run() override;
    void update(float delta_time) override;
    bool isRunning() override;
    bool isFinished() override;

private:
    SceneManager* scene_manager_;
    SceneNode* scene_node_;
    bool effect_running_;
    bool effect_finished_;
    float duration_seconds_;
    float duration_timer_;

    ParticleSystem* particle_effect_;
};
