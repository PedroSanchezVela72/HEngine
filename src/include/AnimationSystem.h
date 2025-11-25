#pragma once
#ifndef ANIMATIO_SYSTEM_H
#define ANIMATIO_SYSTEM_H

#include <IAnimationSystem.h>

namespace H {



	class AnimationSystem : public IAnimationSystem
	{
	private:
		Ogre::SceneManager* _sceneMgr = nullptr;
		Ogre::Animation* _animation = nullptr;
		Ogre::AnimationState* _animationState = nullptr;
		Ogre::NodeAnimationTrack* _track = nullptr;

		/// <summary>
		/// Se cambia la animacion de una entidad
		/// </summary>
		/// <param name="e">Entidad a cambiar la animacion</param>
		/// <param name="name">Nombre de la animacion</param>
		/// <param name="loop">Settear el loop de la animacion</param>
		/// <param name="duration">Settear la duracion de la animacion</param>
		void changeAnimation(Entity* e, std::string name, bool loop, float duration);

	public:
		/// <summary>
		/// Se crea el sistema de animaciones
		/// </summary>
		AnimationSystem() {};

		/// /// <summary>
		/// Se destruye el sistema de animaciones
		/// </summary>
		~AnimationSystem() {};

		/// <summary>
		/// Inicializacion del sistema
		/// </summary>
		void initSystem() override;

		/// <summary>
		/// Funcion update
		/// </summary>
		void update(double deltaTime) override;

		/// <summary>
		/// Metodo de gestion de mensajes
		/// </summary>
		/// <param name="m">Mensaje enviado por el manager</param>
		void receive(const Message* m) override;

		/// <summary>
		/// Se añade un sistema de animaciones de ogre
		/// </summary>
		void addAnimationSystem();

		/// <summary>
		/// Se añade una entidad con el componente de animacion
		/// </summary>
		/// <param name="e">Entidad que contiene el nuevo componente AnimationComponent</param>
		void addEntity(Entity* e, std::string name);
	};
}
#endif // ANIMATION_SYSTEM_H
