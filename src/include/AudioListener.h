
#ifndef AUDIO_LISTENER_H
#define AUDIO_LISTENER_H

#include "Component.h"

namespace H {

	class AudioListener : public Component
	{
	public:
		static constexpr cmpId_type id = _cmp_AUDIO_LISTENER;

		AudioListener() : forward(0.0f, 0.0f, -1.0f), up(0.0f, 1.0f, 0.0f), velocity(0.0f, 0.0f, 0.0f) {}
		AudioListener(Vector3F forw, Vector3F u, Vector3F vel) :
			forward(forw),
			up(u),
			velocity(vel) {
		}

		Vector3F forward;
		Vector3F up;
		Vector3F velocity;
	};
}

#endif

