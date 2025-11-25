#pragma once 
#ifndef SPHERE_COLLIDER_H
#define SPHERE_COLLIDER_H 

#include "Collider.h"

namespace H {

	class SphereCollider : public Collider {
	public:

		static constexpr cmpId_type id = _cmp_SPHERE_COLLIDER;

		SphereCollider(float i_ratius = H_SPHERE_COL_DEF_RAT) :
			ratius(i_ratius) {
			type = Collider::hSPHERE;
			shape = nullptr;
		}

		~SphereCollider() override {}

		void initComponent() override {};
		float ratius;
	};
}
#endif // SPHERE_COLLIDER_H