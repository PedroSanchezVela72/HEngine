#pragma once
#ifndef ENTITY_H
#define ENTITY_H

#include <iostream>
#include <array>
#include <vector>
#include <list>
#include "Component.h"

namespace H {
	class Manager;

	class Entity {
	private:
		bool _active;
		bool _alive;
		Entity* _parent;
		std::list<Entity*> _children;

		friend Manager;
		grpId_type _gId;
	public:
		std::vector<Component*> _comps;

		Entity(grpId_type gId, int maxComponentId) : _gId(gId), _comps(), _active(true), _parent(nullptr), _children(0), _alive(true) {
			_comps.resize(maxComponentId);
		};
		Entity(const Entity&) = delete;
		Entity& operator=(const Entity&) = delete;


		bool isAlive() const { return _alive; };
		void setAlive(bool alive) { _alive = alive; };


		bool isActive() const { return _active; };
		void setActive(bool active) { _active = active; };

		virtual ~Entity() {
			/*if (_parent != nullptr)
				_parent->_children.remove(this);*/

			for (auto c : _comps) {
				if (c != nullptr) {
					delete c;
				}
			}
		}

		Entity* getParent() {
			return _parent;
		}

		std::list<Entity*> getChildren() {
			return _children;
		}

	};
}
#endif