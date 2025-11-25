#pragma once
#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include "IRenderSystem.h"

namespace H {

	class RenderSystem : public IRenderSystem
	{
	private:

		Ogre::Root* _root = nullptr;
		Ogre::RenderWindow* _window = nullptr;
		SDL_Window* _sdlWindow = nullptr;
		Ogre::SceneManager* _sceneMgr = nullptr;

		Ogre::OverlaySystem* _overlaySystem = nullptr;
		Ogre::OverlayManager* _overlayManager = nullptr;


		unsigned int _width;
		unsigned int _height;
		bool _fullScreen;
		const std::string _name;
		const char* _nameWindow;
		std::unordered_map<std::string, int> lightTypes;
		Entity* _mCamera = nullptr;

		// Lista de rayos activos en la escena 
		std::vector<RayWithTTL> _activeRays;

		//Lista de colliders a renderizar
		std::vector<Ogre::ManualObject* > _colliderToRender;

		/// <summary>
		/// Metodo de comprobacion de rutas
		/// </summary>	
		/// <param name="path">Ruta a comprobar</param>
		bool fileExists(const std::string& path);

		// Metodos para el init
		/// <summary>
		/// Configuracion,Creacion SDL y ventana
		/// </summary>
		void SDL();
		/// <summary>
		/// ruta de ogre
		/// </summary>
		void rutas();
		/// <summary>
		/// Carga de resources, el base de ogre y el que contiene carpeta de assets
		/// </summary>
		void resources();
		/// <summary>
		/// Buscar un resource detro de ogre
		/// </summary>
		bool resourceExistsInAnyGroup(const std::string& resourceName);
		/// <summary>
		/// Creacion Ogre, su ventana, camara, escena
		/// </summary>
		void ogre();
		void imgui();

		/// <summary>
		/// Llama a la funcion de renderizado correspondiente a cada Collider
		/// </summary>
		/// <param name="collider">Collider a dibujar</param>
		/// <param name="tr">Transform de la entidad</param>
		void drawCollider(Collider* collider, Transform* tr);

		/// <summary>
		/// Pinta la caja que representa la geometria del BoxCollider
		/// </summary>
		/// <param name="position">Posicion de la entidad</param>
		/// <param name="rotation">Rotacion de la entidad</param>
		/// <param name="geom">Geometria del collider</param>
		void drawBoxCollider(const Vector3F& position, const QuaternionF& rotation, const Vector3F& halfExtents);

		/// <summary>
		/// Pinta la esfera que representa la geometria del SphereCollider
		/// </summary>
		/// <param name="position">Posicion de la entidad</param>
		/// <param name="rotation">Rotacion de la entidad</param>
		/// <param name="geom">Geometria del collider</param>
		void drawSphereCollider(const Vector3F& position, const QuaternionF& rotation, float radius);

		/// <summary>
		/// Pinta la capsula que representa la geometria del CapsuleCollider
		/// </summary>
		/// <param name="position">Posicion de la entidad</param>
		/// <param name="rotation">Rotacion de la entidad</param>
		/// <param name="capsuleGeom">Geometria del collider</param>
		void drawCapsuleCollider(const Vector3F& position, const QuaternionF& rotation, float radius, float halfHeight);

		/// <summary>
		/// Se encarag de actualizar la informacion de los rayos en la escena. Basicamente el ttl(time to life)
		/// </summary>
		void updateRays(double deltaTime);

		/// <summary>
		/// Metodo recursivo para la iteracion de entidades
		/// </summary>	
		/// <param name="path">Ruta a comprobar</param>
		void entityIteration(Entity* entity, bool activeParent);

		void deleteEntity(Entity* ent);
	public:

		/// <summary>
		/// Crea el sistema
		/// </summary>
		/// <param name="width">Anchura de la ventana</param>
		/// <param name="height">Altura de la ventana</param>
		/// <param name="fullScreen">Se crea en pantalla complena</param>
		/// <param name="name">Mensaje enviado por el manager</param>
		RenderSystem(unsigned int width, unsigned int height, bool fullScreen = false, const char* name = H_ENGINE_NAME);
		~RenderSystem();

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
		/// Se dibuja un rayo en pantalla
		/// <summary>
		/// <param name="origin">Posición desde la que sale el rayo</param>
		/// <param name="direction">Dirección del rayo</param>
		/// <param name="length">Dirección del rayo</param>
		/// <param name="color">Color del rayo</param>
		/// <param name="lifeTime">Tiempo de vida del rayo</param>
		void drawRay(const Vector3F& origin, const Vector3F& direction, float length, const Color& color, float ttl)override;

		/// <summary>
		/// Devuelve un puntero al sceneManager activo
		/// </summary>
		Ogre::SceneManager* getSceneManager() const override {	return _sceneMgr; };

		/// <summary>
		/// Devuelve un puntero a la entidad que contiene la camara principal
		/// </summary>
		Entity* getCamera() override { return _mCamera; };

		/// <summary>
		/// Devuelve un puntero a la raíz de OGRE
		/// </summary>
		Ogre::Root* getRoot() override { return _root; };

		/// <summary>
		/// Devuelve el RenderWindow del sistema
		/// </summary>
		/// <returns></returns>
		Ogre::RenderWindow* getRenderWindow() override { return _window; };

		/// <summary>
		/// Se añaden los componentes pertenecientes al render system
		/// </summary>
		/// <param name="e">Entidad que contiene el componente RenderMesh</param>
		/// <param name="entity">Entidad a liberar</param>
		/// <param name="name">Nombre de la entidad en la escena de lua</param>
		void addEntity(Entity* entity, std::string name)override;

		QuaternionF lookAt(const Vector3F& from, const Vector3F& to, const Vector3F& up = Vector3F::up()) override; 

		/// <summary>
		/// Cambia el material de la entidad
		/// </summary>
		/// <param name="ent">Entidad a la que se le asigna el material</param>
		/// <param name="material">Nombre del material</param>
		/// <param name="subEntInd">Indice de la Ogre::SubEntity que se quiere cambiar. Si el indice es -1, se asigna el material a todas las SubEntities</param>
		void setMaterialName(H::Entity* ent, std::string material, int subEntInd = -1) override ;

	};
}
#endif // RENDER_SYSTEM_H

