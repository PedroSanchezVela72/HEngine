#pragma once

#ifndef AUDIO_SYSTEM_H
#define AUDIO_SYSTEM_H

#include "IAudioSystem.h"

namespace H {

	class AudioSystem : public IAudioSystem{

	public:

		AudioSystem();
		/// <summary>
		/// Cierra el sistema de sonido
		/// </summary>
		~AudioSystem();
		/// <summary>
		/// Crea e inicializa el sistema de sonido principal, que no es del tipo ecs::System sino FMOD::System
		/// </summary>
		void initSystem() override;

		/// <summary>
		/// Actualiza el sistema de sonido y los atributos de los AudioListeners y AudioSources en funcion de los transforms de las entidades
		/// </summary>
		void update(double deltaTime) override;

		void receive(const Message* m) override;

		/// <summary>
		/// Reproduce un sonido en una posicion 3d creando un canal para ello a partir de una entidad que contiene un componente AudioSource
		/// </summary>
		/// <param name="soundName"></param>
		void playSound(Entity* ent, std::string soundName)override;
		/// <summary>
		/// Para el canal de sonido del AudioSource de una entidad
		/// </summary>
		/// <param name="channelName"></param>
		void stopChannel(Entity* ent)override;
		/// <summary>
		/// Pausa el canal de sonido del AudioSource de una entidad
		/// </summary>
		/// <param name="channelName"></param>
		/// <param name="pause"></param>
		void pauseChannel(Entity* ent, bool pause)override;
		/// <summary>
		/// Cambia el volumen de el canal del AudioSource de una entidad
		/// </summary>
		/// <param name="channelName"></param>
		/// <param name="volume"></param>
		void setVolume(Entity* ent, float volume)override;
		/// <summary>
		/// Obtiene un puntero a un sonido del mapa del AudioSource de una entidad a partir de un nombre
		/// </summary>
		/// <param name="soundName"></param>
		/// <returns></returns>
		FMOD::Sound* getSound(Entity* ent, std::string soundName) const override;
		/// <summary>
		/// Anade un sonido al AudioSource de la entidad
		/// </summary>
		/// <param name="ent"></param>
		/// <param name="soundName"></param>
		/// <param name="filePath"></param>
		void addSound(Entity* ent, std::string soundName, std::string filePath) override;
		/// <summary>
		/// Obtiene un puntero al canal que tiene una entidad en el AudioSource
		/// </summary>
		/// <param name="channelName"></param>
		/// <returns></returns>
		FMOD::Channel* getChannel(Entity* ent)const override;

		FMOD::System* getSoundSystem() const override { return _soundSystem; }

		/// <summary>
		/// Anade componentes de audio a la entidad si la encuentra en el archivo lua
		/// </summary>
		/// <param name="entity">Puntero a la entidad</param>
		/// <param name="name">Nombre de la entidad que tiene que aparecer en el lua</param>
		void addEntity(Entity* entity, std::string name) override;

	private:
		FMOD::System* _soundSystem;
	};
}

#endif