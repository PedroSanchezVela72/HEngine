#pragma once
#ifndef IMANAGER_H
#define IMANAGER_H

#include <vector>
#include <array>
#include "System.h"
#include "Entity.h"
#include <list>

namespace H {

	class IManager {
	protected:
		std::vector<System*> _systems;
		std::vector<Message*> _msgs;
		std::vector<Message*> _aux_msgs;
	public:
		IManager() = default;
		virtual ~IManager() {}
		virtual void init(int maxGroupId = _LAST_GRP_ID, int maxHdlrId = _LAST_HDLR_ID, int maxSystemId = _LAST_SYS_ID, int maxComponentId = _LAST_CMP_ID) = 0;
		virtual Entity* addEntity(grpId_type gId) = 0;
		virtual bool addEntityParent(Entity* child, Entity* parent) = 0; // Devuelve si se ha podido emparentar
		virtual void changeEntityId(grpId_type gId, Entity* e) = 0;
		virtual void clearScene() = 0;
		virtual void clearSystems() = 0;
		virtual void refresh() = 0;

		virtual void setHandler(hdlrId_type hId, Entity* e) = 0;
		virtual void deleteHandler(hdlrId_type hId, Entity* e) = 0;

		virtual void deleteAllHandlers(hdlrId_type hId) = 0;
		virtual std::list<Entity*> getHandler(hdlrId_type hId) const = 0;

		// Components
		template<typename T, typename ...Ts>
		inline T* addComponent(Entity* e, Ts&& ...args) {
			constexpr cmpId_type cId = T::id;
			T* c = new T(std::forward<Ts>(args)...);
			removeComponent<T>(e);
			e->_comps[cId] = c;
			c->setContext(e, this);
			c->initComponent();
			return c;
		}
		template<typename T>
		inline void removeComponent(Entity* e) {
			constexpr cmpId_type cId = T::id;
			if (e->_comps[cId] != nullptr) {
				delete e->_comps[cId];
				e->_comps[cId] = nullptr;
			}
		}
		template<typename T>
		inline bool hasComponent(Entity* e) {
			constexpr cmpId_type cId = T::id;
			return e->_comps[cId] != nullptr;
		}
		template<typename T>
		inline T* getComponent(Entity* e) {
			constexpr cmpId_type cId = T::id;
			return static_cast<T*>(e->_comps[cId]);
		}
		// Entities
		virtual void setAlive(Entity* e, bool alive) = 0;
		virtual bool isAlive(Entity* e) = 0;
		virtual grpId_type groupId(Entity* e) = 0;

		// Systems
		template<typename T, typename ...Ts>
		inline T* addSystem(Ts &&... args) {
			constexpr sysId_type sId = T::id;
			removeSystem<T>();
			System* s = new T(std::forward<Ts>(args)...);
			s->setContext(this);
			s->initSystem();
			_systems[sId] = s;
			return static_cast<T*>(s);
		}

		template<typename T>
		inline T* getSystem() {
			constexpr sysId_type sId = T::id;
			return static_cast<T*>(_systems[sId]);
		}

		template<typename T>
		inline void removeSystem() {
			constexpr sysId_type sId = T::id;
			if (_systems[sId] != nullptr) {
				delete _systems[sId];
				_systems[sId] = nullptr;
			}
		}

		virtual void send(Message* m, bool delay = false) = 0;


		virtual void flushMessages() = 0;

		virtual std::vector<Entity*>& getEntities(/*grpId_type gId = _grp_GENERAL*/grpId_type gId) = 0;
		virtual void addToGroupList(grpId_type gId, Entity* e) = 0;

		virtual bool getExit() = 0;
		virtual void exit() = 0;

#ifdef _DEBUG
		virtual bool isRunningGame() = 0;
		virtual void toggleRunning() = 0;

		virtual bool getMode() = 0;
		virtual void changeMode() = 0;
#endif

		virtual std::string getSceneName() = 0;
		virtual void setSceneName(std::string name) = 0;
	};
}
#endif 