#pragma once

#ifdef _DEBUG
#ifndef IEDITOR_SYSTEM_H 
#define IEDITOR_SYSTEM_H 


#include "System.h"
#include <map>
#include <unordered_map>
#include <vector>
#include <Quaternion.h>

#include "H_Defs.h"

namespace Ogre {
	class ImguiManager;
	namespace DEBUG_BUILD_REQUIRED {
		class Root;
	}
	class SceneManager;
	class Plane;
	class RenderWindow;
}

class ImVec2; 

namespace H {
	class Manager;
	class Transform;



	enum AxisSelected { AXIS_X = 0, AXIS_Y, AXIS_Z, AXIS_NONE };
	enum ArrowsMode { MOVE = 0, ROTATE, SCALE, NONE };
	typedef std::list<PropertyData> HierarchyType;

	//Como su nombre indica la clase es de prueba
	class IEditorSystem : public System {
	public:
		static constexpr sysId_type id = _sys_EDITOR;

		virtual ~IEditorSystem() {};

		virtual void loadHierarchy() = 0;
		virtual void update(double deltaTime) = 0;
		virtual void receive(const Message* m) = 0;
		virtual void showEntityComponentWindow(double deltaTime) = 0;
		/// <summary>
		/// Para que al guardar en el archivo de lua los componentes, si un componente de texto tiene un salto de linea, lo escriba literalmente
		/// y no haga el salto de linea.
		/// </summary>
		virtual std::string serializeText(const std::string& input) = 0;

		/// <summary>
		/// Guarda las entidades y sus componentes y propiedades de de la jerarquia ordenadamente en un archivo lua
		/// </summary>
		/// <param name="hierarchy">Mapa con key nombre de entidad y valor mapa con key nombre de componente valor struct con propiedades
		/// del componente</param>
		virtual void saveHierarchyToLua(HierarchyType hierarchy) = 0;
		virtual void playAndSave(double deltaTime) = 0;
		virtual void saveButton() = 0;
	};
}
#endif // !EDITOR_SYSTEM_H 
#endif
