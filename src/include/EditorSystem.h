#pragma once

#ifdef _DEBUG
#ifndef EDITOR_SYSTEM_H 
#define EDITOR_SYSTEM_H 


#include "IEditorSystem.h"

namespace H {

	//Como su nombre indica la clase es de prueba
	class EditorSystem : public IEditorSystem {
	public:

		EditorSystem(Manager* mngr, Ogre::DEBUG_BUILD_REQUIRED::Root* root, Ogre::SceneManager* sceneMgr, Ogre::RenderWindow* renderWindow);

		~EditorSystem() override;
		 
		void loadHierarchy();
		void update(double deltaTime) override;
		virtual void receive(const Message* m) override;
		void showEntityComponentWindow(double deltaTime) override;
		/// <summary>
		/// Para que al guardar en el archivo de lua los componentes, si un componente de texto tiene un salto de linea, lo escriba literalmente
		/// y no haga el salto de linea.
		/// </summary>
		std::string serializeText(const std::string& input)override;

		/// <summary>
		/// Guarda las entidades y sus componentes y propiedades de de la jerarquia ordenadamente en un archivo lua
		/// </summary>
		/// <param name="hierarchy">Mapa con key nombre de entidad y valor mapa con key nombre de componente valor struct con propiedades
		/// del componente</param>
		void saveHierarchyToLua(HierarchyType hierarchy)override;
		void playAndSave(double deltaTime)override;
		void saveButton()override;
	private:

		Manager* _mngr;

		Entity* _editorCamera;

		Ogre::SceneManager* _sceneMgr;

		Ogre::RenderWindow* _renderWindow;

		Ogre::ImguiManager* _ImguiMngr;

		Ogre::DEBUG_BUILD_REQUIRED::Root* _root;

		Entity* _arrows;

		Entity* _torus;

		Entity* _scalators;

		AxisSelected _moveInAxis = AXIS_NONE;

		ArrowsMode _arrowsMode = MOVE;

		Ogre::Plane* _referencePlane;

		Ogre::Plane* _axisPlanes[H_NUM_AXIS_PLANE];

		Vector3F _prevPoint;

		Vector3F _origTr;

		QuaternionF _origRot;

		bool _rotationCamera = false;

		std::pair<float, float> _anguloAcumulado;

		float _cameraSpeed = H_MAIN_CAMERA_SPEED;

		Transform* _selectedTr = nullptr;

		std::vector<std::string> entityNames;

		std::map<std::string, Entity*> entityMap;

		HierarchyType _hierarchy;

		bool _unsavedChanges = false;

		void createArrows();

		void createTorus(); 

		void createScalators();

		void changeArrowsTipe(ArrowsMode arrowsMode);

		/// <summary>
		/// Permite cambiar entre el primero y el segundo materalpass
		/// </summary>
		/// <param name="ent"></param>
		/// <param name="firstMat"></param>
		void AlternateMaterial(Entity* ent, bool SecondMat = false);

		void raycast();

		void moveCameraInXY(KeyCode key);

		void moveCameraInZ(int dir);

		void rotateCamera(float x, float y);

		void LanzarMotorRelease();

		std::wstring FindMSBuildPath();
	};
}
#endif // !EDITOR_SYSTEM_H 

#endif
