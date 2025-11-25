
#ifndef IAUDIO_SYSTEM_H
#define IAUDIO_SYSTEM_H

#include "System.h"


namespace FMOD {
	class Channel;
	class Sound; 
	class System;
}

namespace H {

	class IAudioSystem : public System{

	public:
		static constexpr sysId_type id = _sys_AUDIO;

		/// <summary>
		/// Cierra el sistema de sonido
		/// </summary>
		virtual ~IAudioSystem() {};
		/// <summary>
		/// Crea e inicializa el sistema de sonido principal, que no es del tipo ecs::System sino FMOD::System
		/// </summary>
		virtual void initSystem() = 0;

		/// <summary>
		/// Actualiza el sistema de sonido y los atributos de los AudioListeners y AudioSources en funcion de los transforms de las entidades
		/// </summary>
		virtual void update(double deltaTime) = 0;

		virtual void receive(const Message* m) = 0;

		/// <summary>
		/// Reproduce un sonido en una posicion 3d creando un canal para ello a partir de una entidad que contiene un componente AudioSource
		/// </summary>
		/// <param name="soundName"></param>
		virtual void playSound(Entity* ent, std::string soundName) = 0;
		/// <summary>
		/// Para el canal de sonido del AudioSource de una entidad
		/// </summary>
		/// <param name="channelName"></param>
		virtual void stopChannel(Entity* ent) = 0;
		/// <summary>
		/// Pausa el canal de sonido del AudioSource de una entidad
		/// </summary>
		/// <param name="channelName"></param>
		/// <param name="pause"></param>
		virtual void pauseChannel(Entity* ent, bool pause) = 0;
		/// <summary>
		/// Cambia el volumen de el canal del AudioSource de una entidad
		/// </summary>
		/// <param name="channelName"></param>
		/// <param name="volume"></param>
		virtual void setVolume(Entity* ent, float volume) = 0;
		/// <summary>
		/// Obtiene un puntero a un sonido del mapa del AudioSource de una entidad a partir de un nombre
		/// </summary>
		/// <param name="soundName"></param>
		/// <returns></returns>
		virtual FMOD::Sound* getSound(Entity* ent, std::string soundName) const = 0;
		/// <summary>
		/// Anade un sonido al AudioSource de la entidad
		/// </summary>
		/// <param name="ent"></param>
		/// <param name="soundName"></param>
		/// <param name="filePath"></param>
		virtual void addSound(Entity* ent, std::string soundName, std::string filePath) = 0;
		/// <summary>
		/// Obtiene un puntero al canal que tiene una entidad en el AudioSource
		/// </summary>
		/// <param name="channelName"></param>
		/// <returns></returns>
		virtual FMOD::Channel* getChannel(Entity* ent) const = 0;

		virtual FMOD::System* getSoundSystem() const = 0;

		/// <summary>
		/// Anade componentes de audio a la entidad si la encuentra en el archivo lua
		/// </summary>
		/// <param name="entity">Puntero a la entidad</param>
		/// <param name="name">Nombre de la entidad que tiene que aparecer en el lua</param>
		virtual void addEntity(Entity* entity, std::string name) = 0;
	};
}

#endif