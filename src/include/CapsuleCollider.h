#pragma once 
#ifndef CAPSULE_COLLIDER_H
#define CAPSULE_COLLIDER_H 

#include "Collider.h"

namespace H {
	class CapsuleCollider : public Collider {
	public:

		static constexpr cmpId_type id = _cmp_CAPSULE_COLLIDER;

		CapsuleCollider(float i_ratius = H_CAPSULE_COL_DEF_RAT, float i_halfHeight = H_CAPSULE_COL_DEF_HH) :
			ratius(i_ratius), halfHeight(i_halfHeight) {
			type = Collider::hCAPSULE;
			shape = nullptr;
		}

		~CapsuleCollider() override {}

		void initComponent() override {};

		float ratius;
		float halfHeight;
	};
}
#endif