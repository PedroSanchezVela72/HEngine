#pragma once
#ifndef ANIMATION_COMPONENT_H
#define ANIMATION_COMPONENT_H

#include <Component.h>

namespace Ogre {
	class AnimationState;
	class Animation;
}

namespace H {

	struct AnimationComponent : public Component
	{
	public:
		static constexpr cmpId_type id = _cmp_ANIMATION_COMPONENT;
		AnimationComponent() {};
		~AnimationComponent() override {};

		float duration = 0.0f;
		bool loop = true;
		std::string currentAnimation;
		Ogre::AnimationState* animationState = nullptr;
		Ogre::Animation* animation = nullptr;
	};
}
#endif // ANIMATION_COMPONENT_H