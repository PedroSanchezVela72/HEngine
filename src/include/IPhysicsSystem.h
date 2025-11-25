#pragma once 

#ifndef IPHYSICS_SYSTEM_H
#define IPHYSICS_SYSTEM_H

#include "System.h"

namespace physx {
    class PxPhysics;
    class PxScene;
    class PxFoundation;
    class PxDefaultCpuDispatcher;
    class PxDefaultAllocator;
    class PxDefaultErrorCallback;
    class PxMaterial;
    class PxRigidActor;
	class PxShape;
}

namespace H {
    class CollisionManager;
    class LoadLua;
    class Transform;
    class RigidBody;
    class Collider;

    // Información devuelta por el rayo que es lanzado
    struct RaycastHit {
        Entity* entity = nullptr;
        Vector3F point;
        Vector3F normal;
        float distance = 0.0f;
    };
    enum ForceMode {
        hFORCE = 0,
        hIMPULSE,
        hACCELERATION,
        hVELOCITY_CHANGE
    };

    class IPhysicsSystem : public System {
    public:

        static constexpr sysId_type id = _sys_PHYSICS;

        ~IPhysicsSystem() {};

        virtual void initSystem() = 0;
        virtual void fixedUpdate(double deltaTime) = 0;
        virtual void receive(const Message* m) = 0;

        /// <summary>
        /// Inicializa los componentes de fisicas que contenga la informacion de la entidad.
        /// - RigidBody
        /// - BoxCollider
        /// - SphereCollider
        /// - CapsuleCollider
        /// </summary>
        /// <param name="entity">Entidad que contiene los componentes a inicializar</param>
        /// <param name="name">Nombre de la entidad</param>
        virtual void addEntity(Entity* entity, std::string name) = 0;

        /// <summary>
        /// Añade RigidBody y BoxCollider a una entidad, hecho para editor.
        /// </summary>
        /// <param name="e">Entidad</param>
        /// <param name="w">Anchura del collider.</param>
        /// <param name="h">Altura del collider.</param>
        /// <param name="d">Profundidad del collider.</param>
        /// <param name="trigger">Flag de si es trigger o no el collider (default = false).</param>
        virtual void addBoxCollider(Entity* e, const Vector3F& size, const Vector3F&  offset, H::hdlrId hdlr = _LAST_HDLR_ID, bool trigger = false, bool drawInDebug = false) = 0;

        /// <summary>
        /// Activa o desactiva la gravedad de la simulacion
        /// </summary>
        virtual void enableGravity(bool b) = 0;


        /// <summary>
        /// Cambia el valor de la gravedad para la simulación fisica
        /// </summary>
        /// <param name="newGrav">Valor de la nueva gravedad en el eje Y</param>
        virtual void changeGravity(float newGrav) = 0;

        /// <summary>
        /// Efectua un raycast con filtro de handler type, default _hdlr_ENTITIES
        /// </summary>
        /// <param name="origin">Posicion de origen</param>
        /// <param name="direction">Dirección del rayo</param>
        /// <param name="maxDistance">Distancia máxima para el rayo</param>
        /// <param name="hitInfo">Información que devuelve el rayo</param>
        /// <param name="filter">Filtro de handler (layer) </param>
        /// <returns>True if hit, false otherwise</returns>
        virtual bool raycast(const Vector3F& origin, const Vector3F& direction, float maxDistance, RaycastHit& hitInfo, hdlrId_type filter = hdlrId::_hdlr_ENTITIES) = 0;

        /// <summary>
        /// Añade una fuerza al actor del RigidBody
        /// </summary>
        /// <param name="rb">RigidBody</param>
        /// <param name="force">Fuerza que se ejerce.</param>
        virtual void addForce(RigidBody* rb, const Vector3F& force, ForceMode mode = ForceMode::hFORCE) = 0;

        /// <summary>
        /// Establece la velocidad al cuerpo rigido
        /// </summary>
        virtual void addVelocity(RigidBody* rb, const Vector3F& velocity) = 0;

        /// <summary>
        /// Establece la velocidad angular al cuerpo rigido
        /// </summary>
        virtual void addAngularVelocity(RigidBody* rb, const Vector3F& angular) = 0;

        /// <returns>Velocidad del cuerpo</returns>
        virtual Vector3F getVelocity(RigidBody* rb) const = 0; 

        /// <returns>Velocidad angular del cuerpo</returns>
        virtual Vector3F getAngularVelocity(RigidBody* rb)const = 0; 

        /// <summary>
        /// Establece la 'layer' del collider a partir del hdlrId de ecs.h
        /// </summary>
        virtual void setColliderFilter(Collider* collider, hdlrId_type filter) = 0;

        /// <summary>
        /// Activa o desactiva el collider de la entidad
        /// </summary>
        virtual void enableCollider(Collider* collider, bool active) = 0;

        /// <summary>
        /// Setea un physics material a la entidad simepre y cuando tenga rigidbody y collider
        /// </summary>
        virtual void setPhysicMaterial(H::Entity* ent, const Vector3F& mat) = 0;
    };
}
#endif //PHYSICS_SYSTEM_H
