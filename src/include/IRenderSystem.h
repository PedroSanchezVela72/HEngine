#pragma once
#ifndef IRENDER_SYSTEM_H
#define IRENDER_SYSTEM_H

#include "System.h"
#include "Quaternion.h"

namespace Ogre {

#ifdef _DEBUG
	namespace DEBUG_BUILD_REQUIRED {
		class Root;
	}
	using Root = DEBUG_BUILD_REQUIRED::Root;  // Define Ogre::Root como alias en Debug
#else
	class Root;
#endif
	class SceneManager;
	class ManualObject;
	class SceneNode;
	class RenderWindow;
	class Light;
	class OverlaySystem;
	class OverlayManager;
	class Overlay;
}

struct SDL_Window;

namespace H {
	
	class Color;
	class CameraComponent;
	class Collider;
	class Transform;

	// Datos necesarios para renderizado de rayos (raycast)
	struct RayWithTTL {
		Ogre::ManualObject* rayObject = nullptr;
		Ogre::SceneNode* rayNode = nullptr;
		float timeToLive;
		float elapsedTime;
	};

	class IRenderSystem : public System
	{
	public:
		static constexpr sysId_type id = _sys_RENDER;

		virtual ~IRenderSystem() {};

		/// <summary>
		/// Inicializacion del sistema
		/// </summary>
		virtual void initSystem() = 0;

		/// <summary>
		/// Funcion update
		/// </summary>
		virtual void update(double deltaTime) = 0;

		/// <summary>
		/// Metodo de gestion de mensajes
		/// </summary>
		/// <param name="m">Mensaje enviado por el manager</param>
		virtual void receive(const Message* m) = 0;

		/// <summary>
		/// Se dibuja un rayo en pantalla
		/// <summary>
		/// <param name="origin">Posición desde la que sale el rayo</param>
		/// <param name="direction">Dirección del rayo</param>
		/// <param name="length">Dirección del rayo</param>
		/// <param name="color">Color del rayo</param>
		/// <param name="lifeTime">Tiempo de vida del rayo</param>
		virtual void drawRay(const Vector3F& origin, const Vector3F& direction, float length, const Color& color, float ttl) = 0;

		/// <summary>
		/// Devuelve un puntero al sceneManager activo
		/// </summary>
		virtual Ogre::SceneManager* getSceneManager() const = 0;

		/// <summary>
		/// Devuelve un puntero a la entidad que contiene la camara principal
		/// </summary>
		virtual Entity* getCamera() = 0;

		/// <summary>
		/// Devuelve un puntero a la raíz de OGRE
		/// </summary>
		virtual Ogre::Root* getRoot() = 0;

		/// <summary>
		/// Devuelve el RenderWindow del sistema
		/// </summary>
		/// <returns></returns>
		virtual Ogre::RenderWindow* getRenderWindow() = 0;

		/// <summary>
		/// Se añaden los componentes pertenecientes al render system
		/// </summary>
		/// <param name="e">Entidad que contiene el componente RenderMesh</param>
		/// <param name="entity">Entidad a liberar</param>
		/// <param name="name">Nombre de la entidad en la escena de lua</param>
		virtual void addEntity(Entity* entity, std::string name) = 0;

		/// <summary>
		/// Look at de una posicion a otra posicion
		/// </summary>
		/// <param name="camera"></param>
		/// <param name="target"></param>
		virtual QuaternionF lookAt(const Vector3F& from, const Vector3F& to, const Vector3F& up = Vector3F::up()) = 0; 


		/// <summary>
		/// Cambia el material de la entidad
		/// </summary>
		/// <param name="ent">Entidad a la que se le asigna el material</param>
		/// <param name="material">Nombre del material</param>
		/// <param name="subEntInd">Indice de la Ogre::SubEntity que se quiere cambiar. Si el indice es -1, se asigna el material a todas las SubEntities</param>
		virtual void setMaterialName(H::Entity* ent, std::string material, int subEntInd = -1) = 0;

	};
}
#endif // RENDER_SYSTEM_H

