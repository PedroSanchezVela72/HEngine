#pragma once
#ifndef IPARTICLE_SYSTEM_H 
#define IPARTICLE_SYSTEM_H

#include <System.h>

namespace H {

	class IParticleSystem : public System
	{
	public:
		static constexpr sysId_type id = _SYS_PARTICLES;

		virtual ~IParticleSystem() {};

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
		/// Se añade un sistema de particulas de ogre con el componente renderParticle
		/// </summary>
		/// <param name="e">Entidad que contiene el nuevo componente renderParticle</param>
		virtual void addEntity(Entity* e, std::string name) = 0;

		/// <summary>
		/// Se elimina un sistema de particulas de ogre de cierta entidad
		/// </summary>
		/// <param name="e">Entidad que contiene el componente renderParticle</param>
		virtual void deleteParticleSystem(Entity* e) = 0;

	};
}
#endif // PARTICLE_SYSTEM_H

