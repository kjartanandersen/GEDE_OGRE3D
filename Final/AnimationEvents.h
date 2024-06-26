#pragma once
#include "pch.h"
#include "ICallback.h"

using namespace Ogre;


	struct AnimationEvent
	{
		std::vector<ICallback*> callbacks;
		float time;
		bool hasPlayed;
	};

	struct MeshAnimationState
	{
		AnimationState* animation;
		std::list<AnimationEvent*> events;
	};

	struct MeshAnimation
	{
		MeshAnimationState* curr_state;
	};

