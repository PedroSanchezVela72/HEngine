#include "Manager.h"
#include "Transform.h"

using namespace H;

Manager::~Manager() {
	for (auto& ents : _ents_grps) {
		for (auto e : ents){
			Message* m = new Message;
			m->id = _m_DELETE_ENTITY;
			m->entity.entityPtr = e;
			send(m);
			removeComponent<Transform>(e);
			delete e;
		}
	}
	for (auto& system : _systems) {
		delete system;
		system = nullptr;
	}

	for (auto& m : _msgs) {
		delete m;
		m = nullptr;
	}
}

void Manager::init(int maxGroupId, int maxHdlrId, int maxSystemId, int maxComponentId) {
	_maxGroupId = maxGroupId;
	_maxSystemId = maxSystemId;
	_maxHdlrId = maxHdlrId;
	_maxComponentId = maxComponentId;

	_ents_grps.resize(maxGroupId);
	_ents_hdlrs.resize(maxHdlrId);
	_systems.resize(maxSystemId);

	std::fill(_systems.begin(), _systems.end(), nullptr);
	for (auto& groupEntities : _ents_grps) {
		groupEntities.reserve(100);
	}
}

void Manager::clearScene() {

	//HOLA
	//Si te peta al borrar la escena es porque estas llamando a este metodo en el
	//momento incorrecto. Debes mandarte un mensaje vacio a ti mismo pero asi _mngr->send(m, true);
	//luego lo recibes desde tu sistema y ahi llamas a clearScene

	for (auto& ents : _ents_grps) {
		for (auto e : ents) {
			Message* m = new Message;
			m->id = _m_DELETE_ENTITY;
			m->entity.entityPtr = e;
			send(m);
			removeComponent<Transform>(e);
			delete e;
		}
	}
	for (int i = 0; i < _ents_hdlrs.size(); ++i)
	{
		deleteAllHandlers(i);
	}
	for (int i = 0; i < _ents_grps.size(); ++i)
	{
		_ents_grps[i].clear();
	}
	Message* m = new Message;
	m->id = _m_DELETE_CAMERA;
	send(m);
};

void Manager::clearSystems() {
	for (auto& system : _systems) {
		delete system;
		system = nullptr;
	}
}

Entity* Manager::addEntity(grpId_type gId) {
	Entity* e = new Entity(gId, _maxComponentId);
	setAlive(e, true);
	_ents_grps[gId].push_back(e);
	addComponent<Transform>(e, Vector3F(0,0,0)); // mirar si poner esto en la factoria
	return e;
}

bool Manager::addEntityParent(Entity* child, Entity* parent)
{
	if (child == nullptr || parent == nullptr || child == parent || child->_parent == parent)
		return false;

	Entity* aux = parent;
	bool isChild = false;
	while (!isChild && aux != nullptr) // comprobar que el parent no es hijo del child 
	{
		isChild = child == aux;
		aux = aux->_parent;
	}

	if (isChild) { return false; };

	if (child->_parent != nullptr)
	{
		child->_parent->_children.remove(child);
	}

	child->_parent = parent;
	parent->_children.push_back(child);

	return true;
}

//Este metodo es muy ineficiente, usarlo solo para pocas cosas que rara vez cambien de entity id
void Manager::changeEntityId(grpId_type gId, Entity* e) {
	auto& grpEnts = _ents_grps[e->_gId];
	auto it = std::find(grpEnts.begin(), grpEnts.end(), e);
	grpEnts.erase(it);
	e->_gId = gId;
	_ents_grps[gId].push_back(e);
}

void Manager::refresh() {
	for (grpId_type gId = 0; gId < _maxGroupId; gId++) {
		auto& grpEnts = _ents_grps[gId];
		for(Entity* e : grpEnts)
		{
			if (!e->isAlive())
			{
				for (Entity* eChild : e->_children)
				{
					eChild->setAlive(false);
				}
			}
		}
		grpEnts.erase(
			std::remove_if(grpEnts.begin(), grpEnts.end(),
				[this](Entity* e) {
					if (e->isAlive()) {
						return false;
					}
					else {
						Message* m = new Message;
						m->id = _m_DELETE_ENTITY;
						m->entity.entityPtr = e;
						send(m);
						delete e;
						return true;
					}
				}),
			grpEnts.end());
	}
}

std::vector<Entity*>& Manager::getEntities(grpId_type gId) {
	return _ents_grps[gId];
}

void Manager::update(double deltaTime) {
	for (auto& syst : _systems) {
		if (syst != nullptr) syst->update(deltaTime);
	}
}

void Manager::fixedUpdate(double deltaTime) {
	for (auto& syst : _systems) {
		if (syst != nullptr) syst->fixedUpdate(deltaTime);
	}
}

void Manager::lateUpdate(double deltaTime) {
	for (auto& syst : _systems) {
		if (syst != nullptr) syst->lateUpdate(deltaTime);
	}
}

void Manager::addToGroupList(grpId_type gId, Entity* e) {
	_ents_grps[gId].push_back(e);
}
