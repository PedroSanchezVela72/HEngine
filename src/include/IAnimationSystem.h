#pragma once
#ifndef IANIMATIO_SYSTEM_H
#define IANIMATIO_SYSTEM_H

#include <System.h>

namespace Ogre {
	class SceneManager;
	class Animation;
	class AnimationState; 
	class NodeAnimationTrack;
	class TransformKeyFrame;
	class Entity; 
}

namespace H {



	class IAnimationSystem : public System
	{

	public:
		static constexpr sysId_type id = _sys_ANIM;

		/// /// <summary>
		/// Se destruye el sistema de animaciones
		/// </summary>
		virtual ~IAnimationSystem() {};

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
		/// Se añade un sistema de animaciones de ogre
		/// </summary>
		virtual void addAnimationSystem() = 0;

		/// <summary>
		/// Se añade una entidad con el componente de animacion
		/// </summary>
		/// <param name="e">Entidad que contiene el nuevo componente AnimationComponent</param>
		virtual void addEntity(Entity* e, std::string name) = 0;
	};
}
#endif // ANIMATION_SYSTEM_H
