#pragma once
#ifndef PARTICLE_SYSTEM_H 
#define PARTICLE_SYSTEM_H

#include <IParticleSystem.h>

namespace H {

	class ParticleSystem : public IParticleSystem
	{
	private:
		enum AffectorIndex { ROTATOR, SCALER, COLOUR_IMAGE };
	public:
		/// <summary>
		/// Crea el sistema
		/// </summary>
		ParticleSystem();
		~ParticleSystem();

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
		/// Se añade un sistema de particulas de ogre con el componente renderParticle
		/// </summary>
		/// <param name="e">Entidad que contiene el nuevo componente renderParticle</param>
		void addEntity(Entity* e, std::string name) override;

		/// <summary>
		/// Se elimina un sistema de particulas de ogre de cierta entidad
		/// </summary>
		/// <param name="e">Entidad que contiene el componente renderParticle</param>
		void deleteParticleSystem(Entity* e) override;

	};
}
#endif // PARTICLE_SYSTEM_H

