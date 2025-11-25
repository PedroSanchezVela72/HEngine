#pragma once
#ifndef SYSTEM_H
#define SYSTEM_H
#include "ecs.h"

namespace H {
	class System {
	public:
		virtual ~System() { }
		void setContext(IManager* mngr) {
			_mngr = mngr;
		}
		virtual void initSystem() {}
		virtual void update(double deltaTime) {}
		virtual void lateUpdate(double deltaTime) { }
		virtual void fixedUpdate(double deltaTime) { }
		virtual void receive(const Message* m) { }

	protected:
		IManager* _mngr = nullptr;
	};
}
#endif