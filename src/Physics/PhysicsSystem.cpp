#include "PhysicsSystem.h"

#include "LoadLua.h"
#include "Manager.h"
#include "Transform.h"
#include "RigidBody.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "CollisionManager.h"
#include "Debuglog.h"

#include "PxPhysics.h"
#include "foundation/PxFoundation.h"
#include "PxScene.h"
#include "PxRigidDynamic.h"
#include "PxRigidStatic.h"
#include "PxShape.h"
#include "PxMaterial.h"
#include "extensions/PxDefaultAllocator.h"
#include "extensions/PxDefaultErrorCallback.h"
#include "extensions/PxDefaultCpuDispatcher.h"
#include "common/PxTolerancesScale.h"
#include "foundation/PxFoundation.h"
#include "foundation/PxPhysicsVersion.h"
#include "extensions/PxRigidBodyExt.h"


#include <stdexcept>
#include <iostream>

using namespace physx;
using namespace H;

// Filtro de Flags de physx necesarias para la getión de colisiones.
static PxFilterFlags CustomFilterShader( 
    PxFilterObjectAttributes attributes0, PxFilterData filterData0,
    PxFilterObjectAttributes attributes1, PxFilterData filterData1,
    PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
    // Permitir colisiones normales
    pairFlags = PxPairFlag::eCONTACT_DEFAULT;

    // Notificar cuando ocurre una colisión
    pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;

    // Notificar si la colisión continúa
    pairFlags |= PxPairFlag::eNOTIFY_TOUCH_PERSISTS; 

    // Notificar cuando la colisión termina  
    pairFlags |= PxPairFlag::eNOTIFY_TOUCH_LOST; 

    // Notificar puntos de contacto 
    pairFlags |= PxPairFlag::eNOTIFY_CONTACT_POINTS;

    // Habilitar detección de colisión continua
    pairFlags |= PxPairFlag::eDETECT_CCD_CONTACT;

    // Filtro por defecto para que la colisión ocurra
    return PxFilterFlag::eDEFAULT;
}

#pragma region System

void PhysicsSystem::initSystem()
{
    _allocator = new PxDefaultAllocator();
    _errorCallback = new PxDefaultErrorCallback();

    _foundation = PxCreateFoundation(PX_PHYSICS_VERSION, *_allocator, *_errorCallback);
    if (!_foundation) {
        DebugLog::instance()->throwLog(H_PX_FOUNDATION_ERROR);
        _mngr->exit();
    }

    _physics = PxCreatePhysics(PX_PHYSICS_VERSION, *_foundation, physx::PxTolerancesScale(0.5), true);
    if (!_physics) {
        DebugLog::instance()->throwLog(H_PX_PHYSICS_ERROR);
        _mngr->exit();
    }

    physx::PxSceneDesc sceneDesc(_physics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(_gravityOn.x, _gravityOn.y, _gravityOn.z);

    _cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(H_PX_THREADS);
    if (!_cpuDispatcher) {
        DebugLog::instance()->throwLog(H_CPU_DISPATCHER_ERROR);
        _mngr->exit();
    }
    else {
        sceneDesc.cpuDispatcher = _cpuDispatcher;
    }

    sceneDesc.filterShader = CustomFilterShader;
    sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;
    sceneDesc.flags |= PxSceneFlag::eENABLE_STABILIZATION;
    sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVE_ACTORS;
    sceneDesc.flags |= PxSceneFlag::eEXCLUDE_KINEMATICS_FROM_ACTIVE_ACTORS;
    sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;
    sceneDesc.flags |= PxSceneFlag::eENABLE_FRICTION_EVERY_ITERATION;
    sceneDesc.flags |= PxSceneFlag::eENABLE_EXTERNAL_FORCES_EVERY_ITERATION_TGS;
    sceneDesc.solverType = PxSolverType::eTGS;

    _scene = _physics->createScene(sceneDesc);
    if (!_scene) {
        DebugLog::instance()->throwLog("Error al crear la escena de PhysX.");
        _mngr->exit();
    }

    _defaultMat = _physics->createMaterial(
        H_DEF_PX_MAT_SFRIC,
        H_DEF_PX_MAT_DFRIC,
        H_DEF_PX_MAT_RESTITUT);
    if (!_defaultMat) {
        DebugLog::instance()->throwLog("Error al crear el material por defecto de PhysX.");
        _mngr->exit();
    }

    _collisionMngr = new CollisionManager(_mngr);
    if (!_collisionMngr) {
        DebugLog::instance()->throwLog(H_COLLISION_MNGR_ERROR);
        _mngr->exit();
        
    }

    _scene->setSimulationEventCallback(_collisionMngr);
}

PhysicsSystem::~PhysicsSystem() {
    if (_scene) {
        PxU32 nbActors = _scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
        PxActor** actors = new PxActor * [nbActors];

        _scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, actors, nbActors);

        // Liberar cada actor
        for (PxU32 i = 0; i < nbActors; ++i)
        {
            if (actors[i])
            {
                releaseShapes(static_cast<PxRigidActor*>(actors[i]));
                actors[i]->release();  // Liberar cada actor manualmente
            }
        }

        delete[] actors;  // Liberar la memoria del arreglo de actores
        _scene->release();
        _scene = nullptr;
    }

    for (auto it = _materials.begin(); it != _materials.end(); ++it) {  
        if (it->second) {  
            it->second->release();  
        }
    }
    _materials.clear();  


    if (_collisionMngr) {
        delete _collisionMngr;
        _collisionMngr = nullptr;
    }

    if (_cpuDispatcher) {
        _cpuDispatcher->release();
        _cpuDispatcher = nullptr;
    }

    if (_physics) {
        _physics->release();
        _physics = nullptr;
    }

    if (_foundation) {
        _foundation->release();
        _foundation = nullptr;
    }

    if (_allocator) {
        delete _allocator;
        _allocator = nullptr;
    }

    if (_errorCallback) {
        delete _errorCallback;
        _errorCallback = nullptr;
    }
}

void PhysicsSystem::releaseShapes(PxRigidActor* actor)
{
    PxU32 nbShapes = actor->getNbShapes();
    PxShape** shapes = new PxShape * [nbShapes];
    actor->getShapes(shapes, nbShapes);

    for (PxU32 i = 0; i < nbShapes; ++i)
    {
        if (shapes[i])
        {
            auto it = std::find_if(_pendingMaterialChanges.begin(), _pendingMaterialChanges.end(),
                [shapes, i](const MaterialChangeRequest& request) {
                    return request.shape == shapes[i];
                });

            if (it != _pendingMaterialChanges.end())
            {
                _pendingMaterialChanges.erase(it);
            }
            shapes[i]->release();
        }
    }
    delete[] shapes;
}

void PhysicsSystem::receive(const Message* m)
{
    switch (m->id) {
    case _m_INIT_ENTITY: 
        addEntity(m->entity.entityPtr, m->entity.entityName);
        break;
    case _m_DELETE_ENTITY: {
        if(!_mngr->hasComponent<RigidBody>(m->entity.entityPtr))
            break; 
        PxRigidActor* actor = _mngr->getComponent<RigidBody>(m->entity.entityPtr)->actor;
        releaseShapes(actor); 
        actor->release(); 
        break; 
        }
    }
}

void PhysicsSystem::fixedUpdate(double deltaTime) {

    const std::vector<Entity*>& gameObjects = _mngr->getEntities(_grp_PARENTENTS);
    // Solo se simula la física en release. 

#ifdef _DEBUG
    if (_mngr->isRunningGame()) {
        // Para ajustes que se hacen desde el tranform que no dependen de la fisica
        for (Entity* ent : gameObjects) {
            transformToPhysicsRecursive(ent, true);
        }
        _scene->simulate(deltaTime);
        _scene->fetchResults(true);
        processPendingMaterialChanges();
    }
#else
    // Para ajustes que se hacen desde el tranform que no dependen de la fisica
    for (Entity* ent : gameObjects) {
        if(_mngr->hasComponent<RigidBody>(ent))
            transformToPhysicsRecursive(ent, true);
    }
    _scene->simulate(deltaTime);
    _scene->fetchResults(true);

    processPendingMaterialChanges();
#endif


    for (Entity* ent : gameObjects) {
        entityIteration(ent, true);
    }
    _collisionMngr->update(deltaTime);
}

void PhysicsSystem::entityIteration(Entity* entity, bool activeParent) 
{
    Transform* tr = _mngr->getComponent<Transform>(entity);

    if (_mngr->hasComponent<RigidBody>(entity)) {
        RigidBody* rb = _mngr->getComponent<RigidBody>(entity);

        if (activeParent && entity->isActive()) {
            rb->actor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false);
            if(!rb->rbStatic && !rb->rbKinematic)
                // En caso de que algun actor esté dormido por no estar en movimiento
                rb->actor->is<PxRigidDynamic>()->wakeUp(); 

            bool isDynamic = !rb->rbStatic && !rb->rbKinematic;

#ifdef _DEBUG
        if(!_mngr->isRunningGame()){
            transformToPhysics(entity); // Para el movimiento en el editor
        
            enableColliders(entity, true); // Asi no interactuan con nada de la escena en Editor
        }
        else if (isDynamic){
                physicsToTransform(entity);
        }
#else // DEBUG
        physicsToTransform(entity);
#endif
        enableColliders(entity, true); 
        }
        else {
            rb->actor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true);
            enableColliders(entity, false);
        }
    }

    std::list<Entity*> children = entity->getChildren();
    for (Entity* child : children) {
        entityIteration(child, activeParent && entity->isActive());
    }
    return;
}

void PhysicsSystem::transformToPhysics(Entity* ent) { 
    RigidBody* rb = _mngr->getComponent<RigidBody>(ent);
    if (!rb) return; 
    Transform* tr = _mngr->getComponent<Transform>(ent);
    Vector3F pos = tr->getGlobalPosition();
    QuaternionF rot = tr->getGlobalRotation();

    rb->actor->setGlobalPose(PxTransform(PxVec3(pos.x, pos.y, pos.z), PxQuat(rot.x, rot.y, rot.z, rot.w).getNormalized()));
}

void H::PhysicsSystem::transformToPhysicsRecursive(Entity* ent, bool activeParent)
{
    if (activeParent && ent->isActive()) {
        if (_mngr->hasComponent<RigidBody>(ent)) {
            RigidBody* rb = _mngr->getComponent<RigidBody>(ent);
            if (rb->rbStatic || rb->rbKinematic)
                transformToPhysics(ent);
        }
    }

    std::list<Entity*> children = ent->getChildren();
    for (Entity* child : children) {
        transformToPhysicsRecursive(child, activeParent && ent->isActive());
    }
    return;
}

void PhysicsSystem::physicsToTransform(Entity* ent) {
    RigidBody* rb = _mngr->getComponent<RigidBody>(ent);
    if (!rb) return;
    Transform* tr = _mngr->getComponent<Transform>(ent);
    PxVec3 rbPos = rb->actor->getGlobalPose().p; 
    PxQuat rbRot = rb->actor->getGlobalPose().q; 
    Vector3F pos = { rbPos.x, rbPos.y, rbPos.z };  
    H::QuaternionF physxQuat(rbRot.w, rbRot.x, rbRot.y, rbRot.z); 
   
    
    QuaternionF rot = (physxQuat).normalize();
     
    if (ent->getParent() == nullptr) { 
        tr->position = pos; 
        tr->rotation = rot; 
        return;
    }

    Transform* parentTr = _mngr->getComponent<Transform>(ent->getParent()); 
    if (!parentTr) return;
     
    // Inversas para pasar de espacio global a local
    QuaternionF parentRotInv = parentTr->getGlobalRotation().normalize().conjugate(); // si rotacion es unidad 
    Vector3F parentScale = parentTr->getGlobalScale(); // suponiendo no hay escala 0 

    // Posición local: deshacer la rotacion y escala del padre
    Vector3F localPos = pos - parentTr->getGlobalPosition(); 
    localPos = parentRotInv * localPos; 
    localPos = { 
        localPos.x / parentScale.x, 
        localPos.y / parentScale.y, 
        localPos.z / parentScale.z 
    };

    // Rotación local: deshacer la rotacion del padre
    QuaternionF localRot = parentRotInv * rot; 

    tr->position = localPos; 
    tr->rotation = localRot; 

}

#pragma endregion

#pragma region ComponentCreation

void PhysicsSystem::addEntity(Entity* entity, std::string name)
{
    LoadLua* lua = LoadLua::instance();

    // RIGIDBODY
    if (lua->hasComponent(name, "RigidBody")) 
        addRigidBodyFromLua(lua, entity, name);

    // SPHERE COLLIDER 
    if (lua->hasComponent(name, "SphereCollider"))
        addSphereColliderFromLua(lua, entity, name);

    // BOXCOLLIDER
    if (lua->hasComponent(name, "BoxCollider")) 
        addBoxColliderFromLua(lua, entity, name);
    
    // CAPSULE COLLIDER
    if (lua->hasComponent(name, "CapsuleCollider")) 
        addCapsuleColliderFromLua(lua, entity, name);

	
}

void PhysicsSystem::addRigidBodyFromLua(LoadLua* lua, Entity* entity, std::string name) {

    std::string compName = "RigidBody";

    float mass = H_RB_DEF_MASS; 
    bool isStatic = false;
    bool isKinematic = false;
    if (lua->hasProperty(name, { compName, "mass" }))
        mass = lua->loadFloatValue(name, { "RigidBody", "mass" });
    
    if (lua->hasProperty(name, { compName, "rbStatic" }))
        isStatic = lua->loadBoolValue(name, { "RigidBody", "rbStatic" });
    if (lua->hasProperty(name, { compName, "rbKinematic" }))
        isKinematic = lua->loadBoolValue(name, { "RigidBody", "rbKinematic" });

    if (mass <= 0 && !isKinematic && !isStatic) {
        DebugLog::instance()->throwLog(H_RB_MASS_ERROR);
        return;
    }

    if (lua->hasProperty(name, { "PhysicMaterial" })) {
		Vector3F material = Vector3F(H_DEF_PX_MAT_SFRIC, H_DEF_PX_MAT_DFRIC, H_DEF_PX_MAT_RESTITUT); 

        if (lua->hasProperty(name, {"PhysicMaterial", "staticFriction" }))
            material.x = lua->loadFloatValue(name, { "PhysicMaterial", "staticFriction" });
        if (lua->hasProperty(name, { "PhysicMaterial","dynamicFriction" }))
            material.y = lua->loadFloatValue(name, { "PhysicMaterial", "dynamicFriction" });
        if (lua->hasProperty(name, { "PhysicMaterial", "restitution" }))
            material.z = lua->loadFloatValue(name, { "PhysicMaterial", "restitution" });


    }

    Transform* tr = _mngr->getComponent<Transform>(entity);
    RigidBody* rb = _mngr->addComponent<RigidBody>(entity, mass, isStatic, isKinematic);

    Vector3F pos = tr->getGlobalPosition();
    PxVec3 pxPos(pos.x, pos.y, pos.z);
    QuaternionF rot = tr->getGlobalRotation().normalize();
    PxQuat pxRot(rot.x, rot.y, rot.z, rot.w);

    PxTransform pxTr(pxPos, pxRot);


    if (rb->rbStatic)
    {
        rb->actor = _physics->createRigidStatic(pxTr);
        if (!rb->actor) {
			DebugLog::instance()->throwLog(H_RIGID_STATIC_ERROR);
			return;
        }
    }
    else
    {
        rb->actor = _physics->createRigidDynamic(pxTr);
        if (!rb->actor) {
			DebugLog::instance()->throwLog(H_RIGID_DYNAMIC_ERROR);
			return;
        }

        PxRigidDynamic* dynamicActor = static_cast<PxRigidDynamic*>(rb->actor); 
        dynamicActor->setMass(rb->mass);
        dynamicActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, rb->rbKinematic);

        // Lockeo de ejes para rotacion y movimiento del RigidBody
        Vector3<bool> lockPos(false,false,false);
        Vector3<bool> lockRot(false, false, false);
        if (lua->hasProperty(name, { compName, "lock" })) {
            if (lua->hasProperty(name, { compName, "lock", "posL"})) {
                lockPos.x = lua->loadBoolValue(name, { compName, "lock", "posL", "x" });
                lockPos.y = lua->loadBoolValue(name, { compName, "lock", "posL", "y" });
                lockPos.z = lua->loadBoolValue(name, { compName, "lock", "posL", "z" });
            }
            if (lua->hasProperty(name, { compName, "lock", "rotL" })) {
                lockRot.x = lua->loadBoolValue(name, { compName, "lock", "rotL", "x" });
                lockRot.y = lua->loadBoolValue(name, { compName, "lock", "rotL", "y" });
                lockRot.z = lua->loadBoolValue(name, { compName, "lock", "rotL", "z" });
            }
        }
        PxRigidDynamicLockFlags lockFlags; 

        // Lock de posición
        if (lockPos.x) lockFlags |= PxRigidDynamicLockFlag::eLOCK_LINEAR_X; 
        if (lockPos.y) lockFlags |= PxRigidDynamicLockFlag::eLOCK_LINEAR_Y; 
        if (lockPos.z) lockFlags |= PxRigidDynamicLockFlag::eLOCK_LINEAR_Z; 

        // Lock de rotación
        if (lockRot.x) lockFlags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_X; 
        if (lockRot.y) lockFlags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y; 
        if (lockRot.z) lockFlags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z; 

        dynamicActor->setRigidDynamicLockFlags(lockFlags); 
    }

    rb->actor->userData = static_cast<void*>(entity);
    _scene->addActor(*rb->actor);

}

void PhysicsSystem::addBoxColliderFromLua(LoadLua* lua, Entity* entity, std::string name) {
    RigidBody* rb = _mngr->getComponent<RigidBody>(entity);

    if (rb != nullptr) {
        Vector3F halfExtents = {
            lua->loadFloatValue(name, {"BoxCollider", "halfExtents", "x"}),
            lua->loadFloatValue(name, {"BoxCollider", "halfExtents", "y"}),
            lua->loadFloatValue(name, {"BoxCollider", "halfExtents", "z"})
        };
		if (halfExtents.x <= 0 || halfExtents.y <= 0 || halfExtents.z <= 0) {
			DebugLog::instance()->throwLog("[PHYSICS] : se ha asignado un valor no valido a las dimensiones del box collider");
			return;
		}

        Vector3F offset = {
            lua->loadFloatValue(name, {"BoxCollider", "positionOffset", "x"}),
            lua->loadFloatValue(name, {"BoxCollider", "positionOffset", "y"}),
            lua->loadFloatValue(name, {"BoxCollider", "positionOffset", "z"})
        };

        BoxCollider* sc = _mngr->addComponent<BoxCollider>(entity, halfExtents);
        sc->shape = _physics->createShape(PxBoxGeometry(PxVec3(halfExtents.x, halfExtents.y, halfExtents.z)), *_defaultMat,true);
        sc->trigger = lua->loadBoolValue(name, { "BoxCollider", "trigger" });

        sc->shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
        sc->shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !sc->trigger);
        sc->shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, sc->trigger);

        sc->positionOffset = offset;
        sc->shape->setLocalPose(PxTransform(PxVec3(offset.x, offset.y, offset.z)));

        setColliderFilter(sc, H::hdlrId::_hdlr_ENTITIES); // Default
        rb->actor->attachShape(*sc->shape);

        bool draw = false;
        if (lua->hasProperty(name, { "BoxCollider", "draw" }))
            draw = lua->loadBoolValue(name, { "BoxCollider", "draw" });
        sc->draw = draw;

        bool active = true;
        if (lua->hasProperty(name, { "BoxCollider", "active" }))
            active = lua->loadBoolValue(name, { "BoxCollider", "active" });
        sc->active = active;

        if (!rb->rbStatic && !rb->rbKinematic) {
            if (rb->mass <= 0) DebugLog::instance()->throwLog("[PHYSICS] : se ha asignado mass = 0 a un cuerpo dinamico"); 
            PxRigidBodyExt::updateMassAndInertia(*static_cast<PxRigidDynamic*>(rb->actor), rb->mass);
        }
    }
    else {
        std::cerr << H_ADD_COLLIDER_ERROR; 
    }
}

void PhysicsSystem::addSphereColliderFromLua(LoadLua* lua, Entity* entity, std::string name) {
    RigidBody* rb = _mngr->getComponent<RigidBody>(entity);

    if (rb != nullptr) {
        float ratius = lua->loadFloatValue(name, { "SphereCollider", "ratius" });
		if (ratius <= 0) {
			DebugLog::instance()->throwLog("[PHYSICS] : se ha asignado un valor no valido a las dimensiones del sphere collider");
			return;
		}

        Vector3F offset = {
           lua->loadFloatValue(name, {"SphereCollider", "positionOffset", "x"}),
           lua->loadFloatValue(name, {"SphereCollider", "positionOffset", "y"}),
           lua->loadFloatValue(name, {"SphereCollider", "positionOffset", "z"})
        };

       
        SphereCollider* sc = _mngr->addComponent<SphereCollider>(entity, ratius);

        sc->shape = _physics->createShape(PxSphereGeometry(ratius), *_defaultMat, true);

        sc->trigger = lua->loadBoolValue(name, { "SphereCollider", "trigger" });
        sc->shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !sc->trigger);
        sc->shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, sc->trigger);

        sc->positionOffset = offset;
        sc->shape->setLocalPose(PxTransform(PxVec3(offset.x, offset.y, offset.z)));

        setColliderFilter(sc, H::hdlrId::_hdlr_ENTITIES); // Default
        rb->actor->attachShape(*sc->shape);

        bool draw = false;
        if (lua->hasProperty(name, { "SphereCollider", "draw" }))
            draw = lua->loadBoolValue(name, { "SphereCollider", "draw" });
        sc->draw = draw;

        bool active = true;
        if (lua->hasProperty(name, { "SphereCollider", "active" }))
            active = lua->loadBoolValue(name, { "SphereCollider", "active" });
        sc->active = active;
        if (!rb->rbStatic && !rb->rbKinematic) {
            if (rb->mass <= 0) DebugLog::instance()->throwLog("[PHYSICS] : se ha asignado mass = 0 a un cuerpo dinamico");
            PxRigidBodyExt::updateMassAndInertia(*static_cast<PxRigidDynamic*>(rb->actor), rb->mass);
        }
    }
    else {
		DebugLog::instance()->throwLog(H_ADD_COLLIDER_ERROR);     
    }
}

void PhysicsSystem::addCapsuleColliderFromLua(LoadLua* lua, Entity* entity, std::string name) { 
    RigidBody* rb = _mngr->getComponent<RigidBody>(entity);

    if (rb) {
        float radius = lua->loadFloatValue(name, { "CapsuleCollider", "ratius" });
        float halfHeight = lua->loadFloatValue(name, { "CapsuleCollider", "halfHeight" });

        if (radius <= 0 || halfHeight <= 0) {
			DebugLog::instance()->throwLog("[PHYSICS] : se ha asignado un valor no valido a las dimensiones del capsule collider");
            return;
        }

        Vector3F offset = {
          lua->loadFloatValue(name, {"CapsuleCollider", "positionOffset", "x"}),
          lua->loadFloatValue(name, {"CapsuleCollider", "positionOffset", "y"}),
          lua->loadFloatValue(name, {"CapsuleCollider", "positionOffset", "z"})
        };

        CapsuleCollider* sc = _mngr->addComponent<CapsuleCollider>(entity, radius, halfHeight);
        sc->shape = _physics->createShape(PxCapsuleGeometry(radius, halfHeight), *_defaultMat, true);
        
        sc->trigger = lua->loadBoolValue(name, { "CapsuleCollider", "trigger" });

        sc->shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !sc->trigger);
        sc->shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, sc->trigger);

        PxGeometryType::Enum geometryType = sc->shape->getGeometry().getType(); 

        if (geometryType == PxGeometryType::eCAPSULE) { 
            const PxCapsuleGeometry& capsuleGeometry = static_cast<const PxCapsuleGeometry&>(sc->shape->getGeometry());
        }

        PxQuat zUpToYUpRot = PxQuat(PxPi / 2, PxVec3(1.0f, 0.0f, 0.0f));
        PxQuat xRot = PxQuat(PxPi / 2, PxVec3(0.0f, 0.f, 1.f));
        PxQuat combined = (xRot * zUpToYUpRot).getNormalized(); 
        sc->shape->setLocalPose(PxTransform(PxVec3(offset.x, offset.y, offset.z), combined)); 
        sc->positionOffset = offset;

        setColliderFilter(sc, H::hdlrId::_hdlr_ENTITIES); // Default
        rb->actor->attachShape(*sc->shape);

        if (!rb->rbStatic && !rb->rbKinematic) {
            if (rb->mass <= 0) DebugLog::instance()->throwLog("[PHYSICS] : se ha asignado mass = 0 a un cuerpo dinamico");
            PxRigidBodyExt::updateMassAndInertia(*static_cast<PxRigidDynamic*>(rb->actor), rb->mass); 
        }

        bool draw = false;
        if(lua->hasProperty(name, { "CapsuleCollider", "draw" }))
            draw = lua->loadBoolValue(name, { "CapsuleCollider", "draw" });
        sc->draw = draw;

        bool active = true;
        if (lua->hasProperty(name, { "CapsuleCollider", "active" }))
            active= lua->loadBoolValue(name, { "CapsuleCollider", "active" });
        sc->active = active;

    }
    else {
		DebugLog::instance()->throwLog(H_ADD_COLLIDER_ERROR);
    }
}

void PhysicsSystem::addBoxCollider(Entity* e, const Vector3F& size, const Vector3F& offset, H::hdlrId hdlr, bool trigger, bool drawInDebug){
    Transform* tr = _mngr->getComponent<Transform>(e);
    Vector3F pos = tr->getGlobalPosition();
    PxVec3 pxPos(pos.x, pos.y, pos.z);
    QuaternionF rot = tr->getGlobalRotation().normalize();
    PxQuat pxRot(rot.x, rot.y, rot.z, rot.w);

    PxTransform pxTr(pxPos, pxRot);
    RigidBody* rb = _mngr->addComponent<RigidBody>(e, 0, false, false);
    rb->actor = _physics->createRigidDynamic(pxTr);
    _scene->addActor(*rb->actor);

    BoxCollider* sc = _mngr->addComponent<BoxCollider>(e, size);
    sc->shape = _physics->createShape(PxBoxGeometry(PxVec3(size.x, size.y, size.z)), *_defaultMat, true);
    sc->shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true); 
    sc->trigger = trigger;
    sc->draw = drawInDebug;
    setColliderFilter(sc, hdlr); 

    sc->positionOffset = offset;
    sc->shape->setLocalPose(PxTransform(PxVec3(offset.x, offset.y, offset.z)));
    sc->active = true;
    sc->shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, sc->trigger);
    sc->shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !sc->trigger);
    rb->actor->attachShape(*sc->shape);
}

#pragma endregion

#pragma region Others

void PhysicsSystem::enableGravity(bool b) {
    physx::PxVec3 grav;
    b ? grav = physx::PxVec3(0, _gravityOn.y, 0) : grav = physx::PxVec3(0, 0, 0);
    _scene->setGravity(grav);
}

void PhysicsSystem::changeGravity(float grav) {
    _scene->setGravity(PxVec3(0, grav, 0));
    _gravityOn = { 0, grav, 0 };
}

bool PhysicsSystem::raycast(const Vector3F& origin, const Vector3F& direction, float maxDistance, RaycastHit& hitInfo, hdlrId_type filter)
{
    PxRaycastBuffer hit;
    PxVec3 pxOrigin(origin.x, origin.y, origin.z);
    PxVec3 pxDirection(direction.x, direction.y, direction.z);


    PxQueryFilterData filterData;
    filterData.data.word0 = filter;
    filterData.data.word1 = filter;
    bool hasHit = _scene->raycast(pxOrigin, pxDirection.getNormalized(), maxDistance, hit, PxHitFlag::eDEFAULT, filterData);
    
    if (hasHit && hit.hasBlock)
    {
        PxRigidActor* actor = hit.block.actor;
        if (!actor) return false;

        // Buscar la entidad asociada al actor
        for (Entity* ent : _mngr->getHandler(filter)) {
            RigidBody* rb = _mngr->getComponent<RigidBody>(ent);
            if (rb && rb->actor == actor) {
                hitInfo.entity = ent;
                hitInfo.point = { hit.block.position.x, hit.block.position.y, hit.block.position.z };
                hitInfo.normal = { hit.block.normal.x, hit.block.normal.y, hit.block.normal.z };
                hitInfo.distance = hit.block.distance;
                return true;
            }
        }
    }
    return false;
}

void PhysicsSystem::addForce(RigidBody* rb, const Vector3F& force, ForceMode mode) {
    if (rb->rbStatic || rb->rbKinematic) {
        // Si no es dinámico, imprimimos el mensaje
		DebugLog::instance()->throwLog(H_ADD_FORCE_ERROR);
        return;
    }

    PxVec3 pxForce(force.x, force.y, force.z);
    PxForceMode::Enum pxMode = PxForceMode::eFORCE; 
    switch (mode)
    {
    case hIMPULSE:
        pxMode = PxForceMode::eIMPULSE;
        break;
    case hACCELERATION:
        pxMode = PxForceMode::eACCELERATION;
        break;
    case hVELOCITY_CHANGE:
        pxMode = PxForceMode::eVELOCITY_CHANGE;
        break;
    default:
        break;
    }
    static_cast<PxRigidDynamic*>(rb->actor)->addForce(pxForce, pxMode); 
}

void PhysicsSystem::addVelocity(RigidBody* rb, const Vector3F& velocity) {
    if (rb->rbStatic) {
		DebugLog::instance()->throwLog(H_ADD_VEL_ERROR);
        return;
    }
    PxVec3 pxVel = { velocity.x, velocity.y,velocity.z };
    static_cast<PxRigidDynamic*>(rb->actor)->setLinearVelocity(pxVel);

}

void PhysicsSystem::addAngularVelocity(RigidBody* rb, const Vector3F& angular) {
    if (rb->rbStatic) {
        DebugLog::instance()->throwLog(H_ADD_VEL_ERROR);
        return;
    }
    PxVec3 pxVel = { angular.x, angular.y,angular.z };
    static_cast<PxRigidDynamic*>(rb->actor)->setAngularVelocity(pxVel);

}

Vector3F PhysicsSystem::getVelocity(RigidBody* rb) const {
    if (rb && rb->rbStatic) {
        DebugLog::instance()->throwLog("[PHYSICS] : Se quiso revisar la velocidad de un cuerpo estatico, se devuelve (0,0,0) en su lugar");
    }
    if (!rb) return Vector3F::zero();
    PxVec3 rbVel = static_cast<PxRigidDynamic*>(rb->actor)->getLinearVelocity();
    return Vector3F(rbVel.x, rbVel.y, rbVel.z);
}

Vector3F PhysicsSystem::getAngularVelocity(RigidBody* rb) const {
    if (rb->rbStatic) {
        DebugLog::instance()->throwLog("[PHYSICS] : Se quiso revisar la velocidad angular de un cuerpo estatico, se devuelve (0,0,0) en su lugar");
    }
    PxVec3 rbVel = static_cast<PxRigidDynamic*>(rb->actor)->getAngularVelocity();
    return Vector3F(rbVel.x, rbVel.y, rbVel.z);
}

void PhysicsSystem::setColliderFilter(Collider* collider, hdlrId_type filter) {
    PxFilterData data; 
    data.word0 = filter;
    data.word1 = filter;
    data.word2 = physx::PxPairFlag::eNOTIFY_TOUCH_FOUND | 
        physx::PxPairFlag::eNOTIFY_TOUCH_LOST; 
	if (!collider->trigger) data.word2 |= physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS;

    collider->shape->setSimulationFilterData(data);
    collider->shape->setQueryFilterData(data);
}

void PhysicsSystem::enableCollider(Collider* collider, bool active) {
    collider->shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, active);
    collider->shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, active && !collider->trigger); 
    collider->active = active;
}

void PhysicsSystem::enableColliders(Entity* ent, bool active) {
    if (!_mngr->hasComponent<RigidBody>(ent)) return;

    RigidBody* rb = _mngr->getComponent<RigidBody>(ent);
    PxU32 shapeCount = rb->actor->getNbShapes();
    if (shapeCount == 0) return;

    std::vector<PxShape*> shapes(shapeCount);
    rb->actor->getShapes(shapes.data(), shapeCount);

    // Flags por componente
    bool boxShouldEnable = false;
    bool sphereShouldEnable = false;
    bool capsuleShouldEnable = false;

    BoxCollider* bC = _mngr->getComponent<BoxCollider>(ent);
    if (bC) boxShouldEnable = bC->active && active;

    SphereCollider* sC = _mngr->getComponent<SphereCollider>(ent);
    if (sC) sphereShouldEnable = sC->active && active;

    CapsuleCollider* cC = _mngr->getComponent<CapsuleCollider>(ent);
    if (cC) capsuleShouldEnable = cC->active && active;

    // Índices de seguimiento si hay más de un collider por tipo
    size_t boxIdx = 0;
    size_t sphereIdx = 0;
    size_t capsuleIdx = 0;

    for (PxShape* shape : shapes) {
        PxGeometryType::Enum type = shape->getGeometry().getType();

        bool enableThis = false;
        switch (type) {
        case PxGeometryType::eBOX:
            enableThis = boxShouldEnable;
            boxIdx++;
            break;

        case PxGeometryType::eSPHERE:
            enableThis = sphereShouldEnable;
            sphereIdx++;
            break;

        case PxGeometryType::eCAPSULE:
            enableThis = capsuleShouldEnable;
            capsuleIdx++;
            break;

        default:
            enableThis = active;
            break;
        }

        // Activar/desactivar shape flags según si es trigger o no
        shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, enableThis);

        PxShapeFlags flags = shape->getFlags();
        bool isTrigger = flags.isSet(PxShapeFlag::eTRIGGER_SHAPE);

        shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, enableThis && !isTrigger);
    }
}

physx::PxMaterial* PhysicsSystem::getOrCreateMaterial(const Vector3F& mat) {
    MaterialKey key{ mat.x, mat.y, mat.z };

    auto it = _materials.find(key);
    if (it != _materials.end()) {
        return it->second;
    }

    physx::PxMaterial* material = _physics->createMaterial(mat.x, mat.y, mat.z);
    if (!material) {
        DebugLog::instance()->throwLog("[PHYSICS] : No se pudo crear el material físico");
        return nullptr;
    }

    _materials[key] = material;
    return material;
}

void PhysicsSystem::processPendingMaterialChanges() {
    for (auto& req : _pendingMaterialChanges) {
        req.shape->setMaterials(&req.material, 1);
    }
    _pendingMaterialChanges.clear();
}

void PhysicsSystem::setPhysicMaterial(H::Entity* ent, const Vector3F& mat) {
    RigidBody* rb = _mngr->getComponent<RigidBody>(ent);
    if (!rb || !rb->actor) {
        DebugLog::instance()->throwLog("[PHYSICS] : Se intentó asignar un material físico a una entidad sin RigidBody o actor válido");
        return;
    }

    physx::PxMaterial* material = getOrCreateMaterial(mat);
    if (!material) return;

    PxU32 shapeCount = rb->actor->getNbShapes();
    std::vector<physx::PxShape*> shapes(shapeCount);
    rb->actor->getShapes(shapes.data(), shapeCount);

    for (physx::PxShape* shape : shapes) {
        // En lugar de cambiarlo directamente, lo añadimos a la lista
        _pendingMaterialChanges.push_back({ shape, material });
    }
}

#pragma endregion







