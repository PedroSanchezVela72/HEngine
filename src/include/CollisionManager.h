#pragma once
#ifndef COLLISION_MANAGER_H
#define COLLISION_MANAGER_H

#include <cstdint>
#include "PxSimulationEventCallback.h"
#include <unordered_set>
#include <unordered_map>
namespace physx {
    struct PxContactPairHeader;
    struct PxContactPair; 
    struct PxTriggerPair; 
    struct PxConstraintInfo;
    class PxRigidBody; 
    class PxActor;
}

namespace H {
    class IManager; 
    class Entity;

    /// <summary>
    /// Clase para la gestion de colisiones
    /// </summary>
    class CollisionManager : public  physx::PxSimulationEventCallback {
    public:
        CollisionManager(IManager* mngr);
        ~CollisionManager() override;

        void onContact(const  physx::PxContactPairHeader& pairHeader, const  physx::PxContactPair* pairs, uint32_t nbPairs) override;
        void onTrigger(physx::PxTriggerPair* pairs, uint32_t count) override;
        void onConstraintBreak(physx::PxConstraintInfo*, uint32_t)override {}
        void onWake(physx::PxActor**, uint32_t) override {}
        void onSleep(physx::PxActor**, uint32_t) override {}
        void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const uint32_t count) override {}
        void update(double dt); 
    private:
        IManager* _mngr = nullptr; 
        std::unordered_map<physx::PxActor*, std::unordered_set<Entity*>> _triggerEntities; 
        std::unordered_map<physx::PxActor*, std::unordered_set<Entity*>> _collisionEntities; 

        void sendCollisionMessage(Entity* a, Entity* b); 
        void sendTriggerMessage(Entity* triggerEnt, Entity* otherEnt);
        void sendCollisionStayMessage(Entity* a, Entity* b);
        void sendCollisionExitMessage(Entity* a, Entity* b);
        void sendTriggerStayMessage(Entity* triggerEnt, Entity* otherEnt);
        void sendTriggerExitMessage(Entity* triggerEnt, Entity* otherEnt);
    };
}
#endif // COLLISION_MANAGER_H