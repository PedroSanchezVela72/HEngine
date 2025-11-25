#pragma once 

#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include "IPhysicsSystem.h"

namespace H {

    struct MaterialKey {
        float staticFriction;
        float dynamicFriction;
        float restitution;

        bool operator==(const MaterialKey& other) const {
            return staticFriction == other.staticFriction &&
                dynamicFriction == other.dynamicFriction &&
                restitution == other.restitution;
        }
    };
}

namespace std {
    template<>
    struct hash<H::MaterialKey> {
        size_t operator()(const H::MaterialKey& k) const {
            size_t h1 = hash<float>()(k.staticFriction);
            size_t h2 = hash<float>()(k.dynamicFriction);
            size_t h3 = hash<float>()(k.restitution);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
}

namespace H {
    struct MaterialChangeRequest {
		physx::PxShape* shape;
		physx::PxMaterial* material;
    };

    class PhysicsSystem : public IPhysicsSystem {
    public:

        PhysicsSystem() = default;
        ~PhysicsSystem() override;

        void initSystem() override;
        void fixedUpdate(double deltaTime) override;
        void receive(const Message* m) override;

        /// <summary>
        /// Inicializa los componentes de fisicas que contenga la informacion de la entidad.
        /// - RigidBody
        /// - BoxCollider
        /// - SphereCollider
        /// - CapsuleCollider
        /// </summary>
        /// <param name="entity">Entidad que contiene los componentes a inicializar</param>
        /// <param name="name">Nombre de la entidad</param>
        void addEntity(Entity* entity, std::string name)override;

        /// <summary>
        /// Añade RigidBody y BoxCollider a una entidad, hecho para editor.
        /// </summary>
        /// <param name="e">Entidad</param>
        /// <param name="w">Anchura del collider.</param>
        /// <param name="h">Altura del collider.</param>
        /// <param name="d">Profundidad del collider.</param>
        /// <param name="trigger">Flag de si es trigger o no el collider (default = false).</param>
        void addBoxCollider(Entity* e, const Vector3F& size, const Vector3F&  offset, H::hdlrId hdlr = _LAST_HDLR_ID, bool trigger = false, bool drawInDebug = false)override;

        /// <summary>
        /// Activa o desactiva la gravedad de la simulacion
        /// </summary>
        void enableGravity(bool b)override;


        /// <summary>
        /// Cambia el valor de la gravedad para la simulación fisica
        /// </summary>
        /// <param name="newGrav">Valor de la nueva gravedad en el eje Y</param>
        void changeGravity(float newGrav)override;

        /// <summary>
        /// Efectua un raycast con filtro de handler type, default _hdlr_ENTITIES
        /// </summary>
        /// <param name="origin">Posicion de origen</param>
        /// <param name="direction">Dirección del rayo</param>
        /// <param name="maxDistance">Distancia máxima para el rayo</param>
        /// <param name="hitInfo">Información que devuelve el rayo</param>
        /// <param name="filter">Filtro de handler (layer) </param>
        /// <returns>True if hit, false otherwise</returns>
        bool raycast(const Vector3F& origin, const Vector3F& direction, float maxDistance, RaycastHit& hitInfo, hdlrId_type filter = hdlrId::_hdlr_ENTITIES)override;

        /// <summary>
        /// Añade una fuerza al actor del RigidBody
        /// </summary>
        /// <param name="rb">RigidBody</param>
        /// <param name="force">Fuerza que se ejerce.</param>
        void addForce(RigidBody* rb, const Vector3F& force, ForceMode mode = ForceMode::hFORCE)override;

        /// <summary>
        /// Establece la velocidad al cuerpo rigido
        /// </summary>
        void addVelocity(RigidBody* rb, const Vector3F& velocity)override;

        /// <summary>
        /// Establece la velocidad angular al cuerpo rigido
        /// </summary>
        void addAngularVelocity(RigidBody* rb, const Vector3F& angular)override;

        
        /// <returns>Velocidad del cuerpo</returns>
        Vector3F getVelocity(RigidBody* rb) const override; 

        /// <returns>Velocidad angular del cuerpo</returns>
        Vector3F getAngularVelocity(RigidBody* rb) const override; 

        /// <summary>
        /// Establece la 'layer' del collider a partir del hdlrId de ecs.h
        /// </summary>
        void setColliderFilter(Collider* collider, hdlrId_type filter)override;

        /// <summary>
        /// Activa o desactiva el collider de la entidad
        /// </summary>
        void enableCollider(Collider* collider, bool active) override;

        /// <summary>
        /// Setea un physics material a la entidad simepre y cuando tenga rigidbody y collider
        /// </summary>
        void setPhysicMaterial(H::Entity* ent, const Vector3F& mat);
    private:

#pragma region  Physx Initialization

        physx::PxFoundation* _foundation = nullptr;
        physx::PxPhysics* _physics = nullptr;
        physx::PxScene* _scene = nullptr;
        physx::PxDefaultCpuDispatcher* _cpuDispatcher = nullptr;
        physx::PxDefaultAllocator* _allocator = nullptr;
        physx::PxDefaultErrorCallback* _errorCallback = nullptr;

#pragma endregion

       // Mapa de materiales que han sido creados
        std::unordered_map<MaterialKey, physx::PxMaterial*> _materials;

        std::vector<MaterialChangeRequest> _pendingMaterialChanges;
        // Vector que define la gravedad de la simulación
        Vector3F _gravityOn = Vector3F(0, H_DEF_GRAVITY, 0);

        // Default Physics Material
        physx::PxMaterial* _defaultMat;

        // Gestor de colisiones
        CollisionManager* _collisionMngr;

        /// <summary>
		/// Setea los materiales a las shapes dentro de _pendingMaterialChanges
        /// </summary>
        void processPendingMaterialChanges();

        /// <summary>
        /// Crea un nuevo Physics Material y lo devuelve
        /// </summary>
        physx::PxMaterial* getOrCreateMaterial(const Vector3F& mat);

        // Iteración de las entitdades de padre a hijos
        void entityIteration(Entity* entity, bool activeParent);

        /// <summary>
        /// Carga los datos de la escena de lua para el RigidBody.
        /// </summary>
        /// <param name="lua">Instancia de lua.</param>
        /// <param name="entity">Enitdad.</param>
        /// <param name="name">Nombre de la entidad.</param>
        void addRigidBodyFromLua(LoadLua* lua, Entity* entity, std::string name);

        /// <summary>
        /// Carga los datos de la escena de lua para el BoxCollider.
        /// </summary>
        /// <param name="lua">Instancia de lua.</param>
        /// <param name="entity">Enitdad.</param>
        /// <param name="name">Nombre de la entidad.</param>
        void addBoxColliderFromLua(LoadLua* lua, Entity* entity, std::string name);
        /// <summary>
        /// Carga los datos de la escena de lua para el SphereCollider.
        /// </summary>
        /// <param name="lua">Instancia de lua.</param>
        /// <param name="entity">Enitdad.</param>
        /// <param name="name">Nombre de la entidad.</param>
        void addSphereColliderFromLua(LoadLua* lua, Entity* entity, std::string name);
        /// <summary>
        /// Carga los datos de la escena de lua para el CapsuleCollider.
        /// </summary>
        /// <param name="lua">Instancia de lua.</param>
        /// <param name="entity">Enitdad.</param>
        /// <param name="name">Nombre de la entidad.</param>
        void addCapsuleColliderFromLua(LoadLua* lua, Entity* entity, std::string name);

        /// <summary>
        /// Pasa la posicion y rotacion del Transform al actor del RigidBody para la simulación
        /// </summary>
        void transformToPhysics(Entity* ent);
        /// <summary>
        /// Poscion  y rotacion a physx para todas las entidades
        /// </summary>
        /// <param name="ent"></param>
        /// <param name="activeParent"></param>
        void transformToPhysicsRecursive(Entity* ent, bool activeParent);

        /// <summary>
        /// Pasa la posición y rotacion del actor del RigidBody al Transform después de la simulación física
        /// </summary>
        void physicsToTransform(Entity* ent);

        /// <summary>
        /// Activa o desactiva el/los colliders que tenga el rigidBody -> solo para el sistema, no para el usuario
        /// </summary
        void enableColliders(Entity* ent, bool active);

        void releaseShapes(physx::PxRigidActor* actor);


    };
}
#endif //PHYSICS_SYSTEM_H
