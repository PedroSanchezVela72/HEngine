#include "CollisionManager.h"
#include <PxPhysicsAPI.h>
#include "Manager.h"
#include "Entity.h"
#include "Debuglog.h"

using namespace H;

CollisionManager::CollisionManager(IManager* mngr) : _mngr(mngr) {
}

CollisionManager::~CollisionManager() {
    _mngr = nullptr;
}

#pragma region COLLISION HANDLE
void CollisionManager::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
{
    for (physx::PxU32 i = 0; i < nbPairs; i++) {
        physx::PxActor* actorA = pairHeader.actors[0];
        physx::PxActor* actorB = pairHeader.actors[1];

        Entity* entityA = static_cast<Entity*>(actorA->userData);
        Entity* entityB = static_cast<Entity*>(actorB->userData);

        if (!entityA || !entityB) continue;

        const physx::PxContactPair& pair = pairs[i];

        if (pair.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND) {
            // Agregar las entidades al conjunto de entidades en contacto
            _collisionEntities[actorA].insert(entityB);
            _collisionEntities[actorB].insert(entityA);
            sendCollisionMessage(entityA, entityB); // Enter
        }
        else if (pair.events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST) {
            // Quitar las entidades del conjunto cuando salen del contacto
            _collisionEntities[actorA].erase(entityB);
            _collisionEntities[actorB].erase(entityA);
            sendCollisionExitMessage(entityA, entityB); // Exit
        }
    }
}

void CollisionManager::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) {
    for (physx::PxU32 i = 0; i < count; i++) {
        Entity* triggerEntity = static_cast<Entity*>(pairs[i].triggerActor->userData);
        Entity* otherEntity = static_cast<Entity*>(pairs[i].otherActor->userData);

        if (!triggerEntity || !otherEntity) continue;

        const physx::PxTriggerPair& pair = pairs[i];

        if (pair.status & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND) {
            _triggerEntities[pairs[i].triggerActor].insert(otherEntity);
            sendTriggerMessage(triggerEntity, otherEntity); // Enter 
        }
        else if (pair.status & physx::PxPairFlag::eNOTIFY_TOUCH_LOST) {
            _triggerEntities[pairs[i].triggerActor].erase(otherEntity);
            sendTriggerExitMessage(triggerEntity, otherEntity); // Exit
        }
    }
}

void CollisionManager::update(double dt) {
    for (auto& triggerPair : _triggerEntities) { 
        Entity* triggerEntity = static_cast<Entity*>(triggerPair.first->userData); 
        for (Entity* otherEntity : triggerPair.second) { 
            if (!triggerEntity || !otherEntity) {
				DebugLog::instance()->throwLog("[TRIGGER] : CollisionManager::update(): Entity is null");
				continue;
			}
            sendTriggerStayMessage(triggerEntity, otherEntity); // Stay 
        }
    }
    for (auto& collisionPair : _collisionEntities) {
        Entity* entityA = static_cast<Entity*>(collisionPair.first->userData);
        for (Entity* entityB : collisionPair.second) {
            if (!entityA || !entityB) {
				DebugLog::instance()->throwLog("[COLLISION] : CollisionManager::update(): Entity is null");
                continue;
            }
            sendCollisionStayMessage(entityA, entityB); // Stay
        }
    }
} 
 
void CollisionManager::sendCollisionMessage(Entity* a, Entity* b) {
    Message* m = new Message;
    m->id = _m_ON_COLLISION_ENTER;
    m->on_collision.entityA = a;
    m->on_collision.entityB = b;
    _mngr->send(m);
}

void CollisionManager::sendTriggerMessage(Entity* triggerEnt, Entity* otherEnt) {
    Message* m = new Message;
    m->id = _m_ON_TRIGGER_ENTER;
    m->on_trigger.triggerEnt = triggerEnt;
    m->on_trigger.otherEnt = otherEnt;
    _mngr->send(m);
}
void CollisionManager::sendCollisionStayMessage(Entity* a, Entity* b) {
    
    Message* m = new Message;
    m->id = _m_ON_COLLISION_STAY;
    m->on_collision.entityA = a;
    m->on_collision.entityB = b;
    _mngr->send(m);
}

void CollisionManager::sendCollisionExitMessage(Entity* a, Entity* b) {

    Message* m = new Message;
    m->id = _m_ON_COLLISION_EXIT;
    m->on_collision.entityA = a;
    m->on_collision.entityB = b;
    _mngr->send(m);
}

void CollisionManager::sendTriggerStayMessage(Entity* triggerEnt, Entity* otherEnt) {

    Message* m = new Message;
    m->id = _m_ON_TRIGGER_STAY;
    m->on_trigger.triggerEnt = triggerEnt;
    m->on_trigger.otherEnt = otherEnt;
    _mngr->send(m);
}

void CollisionManager::sendTriggerExitMessage(Entity* triggerEnt, Entity* otherEnt) {
    Message* m = new Message;
    m->id = _m_ON_TRIGGER_EXIT;
    m->on_trigger.triggerEnt = triggerEnt;
    m->on_trigger.otherEnt = otherEnt;
    _mngr->send(m);
}
#pragma endregion