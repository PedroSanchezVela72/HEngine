#pragma once
#ifndef RENDER_PARTICLE_H
#define RENDER_PARTICLE_H
#include <Component.h>

namespace Ogre {
	class SceneNode;
	class ParticleSystem;
}

namespace H {

	struct RenderParticle : public Component
	{
	public:
		static constexpr cmpId_type id = _cmp_RENDER_PARTICLE;

		RenderParticle() = default;
		RenderParticle(const std::string& material, int pWidth, int pHeight, int quota, bool cullEach, bool sorted, Vector3F direction,
			Vector3F position, float angle, float emissionRate, float timeToLive, float minVelocity, float maxVelocity, bool addImageAffector, const std::string& image,
			float rotationRangeStart, float rotationRangeEnd, float rotationSpeedRangeStart, float rotationSpeedRangeEnd,
			float scaleRate) :
			material(material), particleWidth(pWidth), particleHeight(pHeight), quota(quota),
			cullEach(cullEach), sorted(sorted), direction(direction), position(position),
			angle(angle), emissionRate(emissionRate), timeToLive(timeToLive),
			minVelocity(minVelocity), maxVelocity(maxVelocity), addImageAffector(addImageAffector),
			image(image),
			rotationRangeStart(rotationRangeStart), rotationRangeEnd(rotationRangeEnd),
			rotationSpeedRangeStart(rotationSpeedRangeStart), rotationSpeedRangeEnd(rotationSpeedRangeEnd), rate(scaleRate) {
		}

		//BASE STATS
		std::string material;
		int particleWidth, particleHeight;
		int quota;
		bool cullEach;
		bool sorted;

		//EMITTER POINT
		Vector3F direction;
		Vector3F position;
		float angle;
		float emissionRate;
		float timeToLive;
		float minVelocity, maxVelocity;

		//AFFECTOR COLOUR IMAGE
		bool addImageAffector;
		std::string image;

		//AFFECTOR ROTATOR
		float rotationRangeStart, rotationRangeEnd;
		float rotationSpeedRangeStart, rotationSpeedRangeEnd;

		//AFFECTOR SCALER
		float rate;

		Ogre::SceneNode* sceneNode = nullptr;
		Ogre::ParticleSystem* particleSystem = nullptr;
	};
}
#endif // RENDER_PARTICLE_H
