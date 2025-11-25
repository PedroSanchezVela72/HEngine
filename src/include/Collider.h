#pragma once 
#ifndef COLLIDER_H
#define COLLIDER_H 

#include "Component.h"

namespace physx {
	class PxShape;
}

namespace H {

	class Collider : public Component {
	public:

		enum ColliderType {
			hBOX = 0,
			hSPHERE,
			hCAPSULE
		};

		virtual ~Collider() override {
			// PxPhysics de PhysicsSystem se encarga de liberar la memoria de PxShape
			shape = nullptr;
		}
		virtual void initComponent() override {}

		// Flag para saber si se renderiza o no el collider
		bool draw = false;
		//Forma que está enlazada al RigidBody y que define al propio collider
		physx::PxShape* shape = nullptr;
		//Flag para definir si es un trigger o no 
		bool trigger = false;
		//Tipo de collider 
		ColliderType type;
		// Desplazamiento del objeto dentro de la entidad
		Vector3F positionOffset;
		bool active = false;
	};
}
#endif // COLLIDER_H