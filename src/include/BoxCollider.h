#pragma once 
#ifndef BOX_COLLIDER_H
#define BOX_COLLIDER_H 

#include "Collider.h"

namespace H {

	class BoxCollider : public Collider {
	public:

		static constexpr cmpId_type id = _cmp_BOX_COLLIDER;

		BoxCollider(const Vector3F& i_halfExtents = Vector3F(H_BOX_COL_DEF_SIZE))
			: halfExtents(i_halfExtents) {
			type = Collider::hBOX;
			shape = nullptr;
		}
		~BoxCollider() override {}

		void initComponent() override {};
		Vector3F halfExtents;
	};
}
#endif