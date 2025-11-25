#pragma once
#ifndef COMPONENT_H
#define COMPONENT_H

#include "ecs.h"

namespace H {

	struct Component 
	{
	protected:
		Entity* _ent;
		IManager* _mngr;
	public: 
		virtual ~Component() {}
		/// <summary>
		/// Establece referencia al manager y la entidad a la que pertenece el componente
		/// </summary>
		/// <param name="ent">entidad a la que pertenece</param>
		/// <param name="mngr">manager de la escena</param>
		inline void setContext(Entity* ent, IManager* mngr) {
			_ent = ent;
			_mngr = mngr;
		}
		/// <summary>
		/// Inicializa el componente despuese de crearlo y establecer el manager
		/// </summary>
		virtual void initComponent() { }
	};
}
#endif