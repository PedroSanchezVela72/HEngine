#ifndef UI_SYSTEM_H
#define UI_SYSTEM_H

#include "IUISystem.h"

namespace H {
	class UISystem : public IUISystem {
	public:
		UISystem(Manager* mngr, Ogre::SceneManager* sceneMgr, Ogre::RenderWindow* renderWindow);

		~UISystem();

		void update(double deltaTime) override;
		void initSystem() override;
		virtual void receive(const Message* m) override;

	private:
		bool _showUIEditor; 

		Manager* _mngr;

		Ogre::SceneManager* _sceneMgr;

		Ogre::RenderWindow* _renderWindow;

		Ogre::ImguiManager* _ImguiMngr;

		Ogre::Root* _root;

		Ogre::SceneNode* _sceneNode;

		ImFont* _fontDefault; //Fuente predeterminada para textos
		std::unordered_map<std::string, ImFont*> _fontCache; // Cache para fuentes

		int numScene;
		bool fontsRemove;

		/// <summary>
		/// Renderizar un Canvas en la interfaz de usuario
		/// </summary>
		/// <param name="deltaTime">Tiempo transcurrido entre un frame y el siguiente.</param>
		/// <param name="ent">Puntero a una entidad del sistema</param>
		void renderCanvas(double deltaTime, Entity* ent);

		/// <summary>
		/// Itera sobre una entidad y sus hijos
		/// </summary>
		/// <param name="ent">Puntero a una entidad del sistema</param>
		/// <param name="activeParent">Indica si el padre de la entidad está activo.</param>
		void entityIteration(Entity* entity, bool activeParent);

		/// <summary>
		/// Renderizar una imagen en la interfaz de usuario
		/// </summary>
		/// <param name="ent">Puntero a una entidad del sistema</param>
		void renderUIImage(Entity* ent);

		/// <summary>
		/// Renderizar un botón en la interfaz de usuario. Si el botón tiene una imagen, se renderiza como un ImageButton, de lo contrario, se usa un Button estándar.
		/// </summary>
		/// <param name="ent">Puntero a una entidad del sistema</param>
		void renderButton(Entity* ent);

		/// <summary>
		/// Renderizar un texto en la interfaz de usuario. Si el texto tiene una fuente personalizada, se usa temporalmente.
		/// </summary>
		/// <param name="ent">Puntero a una entidad del sistema</param>
		void renderText(Entity* ent);

		/// <summary>
		/// Renderizar un slider en la interfaz de usuario. El slider permite ajustar un valor numérico dentro de un rango, con soporte para valores enteros o flotantes.
		/// </summary>
		/// <param name="ent">Puntero a una entidad del sistema</param>
		void renderSlider(Entity* ent);

		/// <summary>
		/// Renderizar un campo de texto en la interfaz de usuario.
		/// </summary>
		/// <param name="ent">Puntero a una entidad del sistema</param>
		void renderInputField(Entity* ent);

		/// <summary>
		/// Para que al leer del archivo lua, si se encuentra con una cadena con \\n lo convierta a \n
		/// </summary>
		/// <param name="input">Cadena para deserializar</param>
		/// <returns>Cadena deserializada</returns>
		std::string deserializeText(const std::string& input);

		/// <summary>
		/// Carga dinámicamente componentes de UI desde Lua y los asigna a una entidad.
		/// </summary>
		/// <param name="e">Puntero a una entidad del sistema</param>
		/// <param name="name">Nombre del componente en el script de configuración</param>
		void addEntity(Entity* e, std::string name);

		void addFontDefault();
#ifdef _DEBUG
		template <typename T>
		T editorConvFactor(const T& value);
#endif

	};
}
#endif // UI_SYSTEM_H

