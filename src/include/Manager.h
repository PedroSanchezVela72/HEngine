#pragma once
#ifndef MANAGER_H
#define MANAGER_H


#include "IManager.h"


namespace H {

	class Manager : public IManager{ 
	private:
		std::vector<std::vector<Entity*>> _ents_grps;
		std::vector<std::list<Entity*>> _ents_hdlrs;
		bool _exit = false;
		bool _running = false;
		bool _DebugMode = false;

		int _maxGroupId;
		int _maxHdlrId;
		int _maxSystemId;
		int _maxComponentId;
		std::string _sceneName; 
	public:
		Manager() = default;
		virtual ~Manager();
		virtual void init(int maxGroupId = _LAST_GRP_ID, int maxHdlrId = _LAST_HDLR_ID, int maxSystemId = _LAST_SYS_ID, int maxComponentId = _LAST_CMP_ID) override ;
		virtual Entity* addEntity(grpId_type gId) override;
		virtual bool addEntityParent(Entity* child, Entity* parent) override; // Devuelve si se ha podido emparentar
		virtual void changeEntityId(grpId_type gId, Entity* e) override;
		virtual void clearScene() override;
		virtual void clearSystems() override;
		virtual void refresh() override;

		void update(double deltaTime);
		void fixedUpdate(double deltaTime);
		void lateUpdate(double deltaTime);

		inline void setHandler(hdlrId_type hId, Entity* e) override {
			_ents_hdlrs[hId].push_back(e);
		}
		inline void deleteHandler(hdlrId_type hId, Entity* e) override {
			_ents_hdlrs[hId].remove(e);
		}
		inline void deleteAllHandlers(hdlrId_type hId) override {
			_ents_hdlrs[hId].clear();
		}
		inline std::list<Entity*> getHandler(hdlrId_type hId) const override {
			return _ents_hdlrs[hId];
		}
		// Entities
		inline void setAlive(Entity* e, bool alive) override {
			if (e != nullptr)
				e->setAlive(alive);
		}
		inline bool isAlive(Entity* e)override {
			return e->isAlive();
		}
		inline grpId_type groupId(Entity* e) override {
			return e->_gId;
		}

		inline void send(Message* m, bool delay = false) override {
			if (!delay) {
				for (System* s : _systems) {
					if (s != nullptr)
						s->receive(m);
				}
				delete m;
			}
			else {
				_msgs.emplace_back(m);
			}
		}

		inline void flushMessages() override {
			std::swap(_msgs, _aux_msgs);
			for (Message* m : _aux_msgs) {
				for (System* s : _systems) {
					if (s != nullptr)
						s->receive(m);
				}
				delete m;
			}
			_aux_msgs.clear();
		}

		virtual std::vector<Entity*>& getEntities(/*grpId_type gId = _grp_GENERAL*/grpId_type gId) override;
		void addToGroupList(grpId_type gId, Entity* e) override;

		inline bool getExit()override { return _exit; };
		inline void exit() override { _exit = true; };

#ifdef _DEBUG
		inline bool isRunningGame() override { return _running; };
		inline void toggleRunning() override { _running = !_running; };

		inline bool getMode() override { return _DebugMode; };
		inline void changeMode() override { _DebugMode = !_DebugMode; };
#endif // DEBUG
    
		inline std::string getSceneName() override { return _sceneName; }
		inline void setSceneName(std::string name) override { _sceneName = name; }
	};
}
#endif