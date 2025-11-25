#pragma once

#ifndef RIGID_BODY_H
#define RIGID_BODY_H

#include <Component.h>

namespace physx { 
	class PxRigidActor;
}

namespace H {

	class RigidBody : public Component {
	public:

		static constexpr cmpId_type id = _cmp_RIGIDBODY;

		RigidBody(float i_mass, bool i_isStatic = false, bool i_isKinematic = false) :
			mass(i_mass), rbStatic(i_isStatic), rbKinematic(i_isKinematic), actor(nullptr) {
		}
		~RigidBody() override {
			// No es necesario limpiar actor porque ya lo hace el PhysicsSystem
			actor = nullptr;
		}
		void initComponent() override {};

		physx::PxRigidActor* actor;
		float mass;
		bool rbStatic;
		bool rbKinematic;
	};
}
#endif //RIGID_BODY_H

