#pragma once 
#ifndef H_DEFS_H
#define H_DEFS_H


#define H_ENGINE_NAME				"HEngine"

// H_AUDIO

#define H_MAX_AUDIO_CHANNELS		512

// H_RENDER

#define H_WINDOW_WIDTH				1280
#define H_WINDOW_HEIGHT				720

// H_ENGINE

#define H_PI		3.14159265358979323846
#define H_PIf		3.14159265358979323846f

// H_INPUT
	// Input debug macros
#define H_ACTIVE_KEY false
#define H_ACTIVE_MOUSE_BUTTON false
#define H_ACTIVE_MOUSE_MOTION false
#define H_ACTIVE_MOUSE_WHEEL false

// H_GUI

#define H_UI_DEF_FONT				"Apple"
#define H_UI_DEF_TXT_SIZE			16

#define H_UI_ELEM_DEF_COLOR			{0.2f,0.2f,0.2f,1.f}
#define H_UI_ELEM_DEF_BG_COLOR		{0.8f, 0.8f, 0.8f, 1.f}
#define H_UI_WHITEHOVERED			{0.8f,0.8f,0.8f,1.f}
#define H_UI_WHITEACTIVE			{0.5f,0.5f,0.5f,1.f}
#define H_UI_BLACKHOVERED			{0.3f,0.3f,0.3f,1.f}
	//INPUT_FIELD
#define H_INPUT_FIELD_MAX_LEN		256
#define H_PLACE_HOLDER_TXT_DEF		"Enter text..."
	//SLIDER
#define H_SLIDER_MIN_DEF_VALUE		0.0f
#define H_SLIDER_MAX_DEF_VALUE		1.0f
		//Tamaño de las ventans hijas de UI
#define H_SIZE_WIND_WIDTH			100.f
#define H_SIZE_WIND_HEIGHT			100.f

		//RUTA DEL EXPORTADOR DE MAPAS A LA ESCENA
#define H_MAP_EXPORTER				"Assets/ScriptsLua/MapExporter.lua"
#define H_LUA_UTILS			"Assets/ScriptsLua/utils.lua"

// H_EDITOR

#ifdef _DEBUG

	// CANVAS


		// Numero de ejes para flechas de editor
	#define H_NUM_AXIS_PLANE			3
		// Velocidad de la MainCamera
	#define H_MAIN_CAMERA_SPEED			0.5
		// Posicion inicial de la MainCamera
	#define H_INIT_CAMERA_POS			{0, 20, 20}
		// Clip distance
	#define H_NEAR_CLIP_DIST			5
	#define H_FAR_CLIP_DIST				500
		// Orientacion inicial de la MainCamera
	#define H_INIT_CAMERA_ORENTATION	{-45, 0, 0}
		// Color de fondo para el editor
	#define H_BACKGROUND_COLOR			{0.1f, 0.1f, 0.1f}

		// EDITOR_ARROWS
	#define H_ARROW_MESH				"Arrow.mesh"
	#define H_TORUS_MESH				"Torus.mesh"
	#define H_SCALATOR_MESH				"Scalator.mesh"
	#define H_ARROW_SIZE				{2.f, 0.2f, 0.5f}
	#define H_TORUS_SIZE				{ 0.2f, 3.f, 3.f }
	#define H_ARROW_OFFSET				{1.2f, 0.f, 0.f}
			// ARROW_X
	#define H_ARROW_X_ROTATION			{0, 0, 0}
	#define H_ARROW_X_MAT				"Red"
			// ARROW_Y
	#define H_ARROW_Y_ROTATION			{0, 0, 90}
	#define H_ARROW_Y_MAT				"Green"
			// ARROW_Z
	#define H_ARROW_Z_ROTATION			{0, -90, 0}
	#define H_ARROW_Z_MAT				"Blue"

		// EDITOR_INSPECTOR
	#define H_INSPECTOR_WIDTH			400
	#define H_INSPECTOR_HEIGHT			H_WINDOW_HEIGHT * 0.8
	#define H_INSPECTOR_POS				{0,0}
	
		// EDITOR_PLAY_AND_SAVE
			// Tamaño de ventana de Play Controls
	#define H_PLAY_CTRL_WND_WIDTH		H_INSPECTOR_WIDTH
	#define H_PLAY_CTRL_WND_HEIGHT		H_WINDOW_HEIGHT - H_INSPECTOR_HEIGHT
	#define H_PLAY_CTRL_BUTTON_COLOR	{0.0f, 0.5f, 0.0f, 1.0f}
	#define H_PLAY_CTRL_POS				{0, H_INSPECTOR_HEIGHT} 
			// Ruta de Main.exe para lanzar el motor en Release
	#define H_EXECUTABLE_ROOT			L"..\\..\\..\\build\\x64\\Release\\Main.exe"
			// Mensaje para la ventana de carga
	#define H_LOADING_LAUNCH_MSG		L"Cargando... por favor espere"
			// LAUNCH_ERRORS
	#define H_EXE_ERROR					L"Error"
	#define H_MAIN_EXE_NOT_FOUND_ERROR	L"ERROR: No se encontró el ejecutable Main.exe"
	#define H_MAIN_EXE_EXECUTE_ERROR	L"ERROR: No se pudo ejecutar Main.exe"
	#define H_LOADING_WINDOW_ERROR		L"ERROR: No se pudo crear la ventana de carga"
			// Tamaño de la pantalla de carga 
	#define H_LAUNCH_WND_WIDTH			300
	#define H_LAUNCH_WND_HEIGHT			100

	#define H_CANVAS_EDITOR_WIDTH		H_WINDOW_WIDTH - H_INSPECTOR_WIDTH
	#define H_CANVAS_EDITOR_HEIGHT		(H_CANVAS_EDITOR_WIDTH) * (H_WINDOW_HEIGHT) / (H_WINDOW_WIDTH)
	#define H_CANVAS_EDITOR_POS			{H_INSPECTOR_WIDTH, 0}
#endif // !_DEBUG

// H_PHYSICS
#define H_PX_THREADS				2
	//COLLIDERS
#define H_BOX_COL_DEF_SIZE			{5, 5, 5}
#define H_CAPSULE_COL_DEF_RAT		2.5
#define H_CAPSULE_COL_DEF_HH		4.
#define H_SPHERE_COL_DEF_RAT		2.5
	//RIGIDBODY
#define H_RB_DEF_MASS				1
	//GRAVITY
#define H_DEF_GRAVITY				-9.81f
	//PHYSICS MATERIAL
#define H_DEF_PX_MAT_SFRIC			0.5f
#define H_DEF_PX_MAT_DFRIC			0.5f
#define H_DEF_PX_MAT_RESTITUT		0.1f
	//ERRORS
#define H_PX_FOUNDATION_ERROR		"[PHYSICS] : PhysX Foundation initialization failed!"
#define H_PX_PHYSICS_ERROR			"[PHYSICS] : PhysX Physics initialization failed!"
#define H_CPU_DISPATCHER_ERROR		"[PHYSICS] : Failed to create CPU dispatcher"
#define H_COLLISION_MNGR_ERROR		"[PHYSICS] : CollisionManager initialization failed."
#define H_RIGID_DYNAMIC_ERROR		"[PHYSICS] : No se pudo crear el cuerpo dinamico en PhysX."
#define H_RIGID_STATIC_ERROR		"[PHYSICS] : No se pudo crear el cuerpo estatico en PhysX."
#define H_ADD_COLLIDER_ERROR		"[PHYSICS] : PARA AÑADIR UN COLLIDER ES NECESARIO QUE LA ENTIDAD TENGA UN RIGIDBODY"
#define H_ADD_FORCE_ERROR			"[PHYSICS] : No se puede aplicar fuerza a un elemento estático o cinemático."
#define H_ADD_VEL_ERROR			    "[PHYSICS] : No se puede aplicar velocidad a un elemento estático."
#define H_RB_MASS_ERROR				"[PHYSICS] : Se ha asignado un valor no valido a la masa del rigid body, no se ha creado el RigidBody"


// H_RENDER
	// CAMERA CMP
#define H_CAM_CMP_BG_COLOR			{0.2f,0.2f,0.2f} 
		// Near clip distance
#define H_CAM_CMP_NEAR_CDIST		5.f
		// Far clip distance
#define H_CAM_CMP_FAR_CDIST			200.f

	// PARTICLE SYSTEM
#define H_PART_SYS_MAX_QUOTA_DEF	500
#define H_PART_SYS_DEF_EMMITER		0

#define H_PART_DEF_WIDTH			1
#define H_PART_DEF_HEIGHT			1
#define H_PART_DEF_QUOTA			100
#define H_PART_DEF_RATE				10
#define H_PART_DEF_TTL				10
#define H_PART_DEF_MIN_VEL			30
#define H_PART_DEF_MAX_VEL			30

	// SYSTEM
#define H_RESOURCES_PATH			"Assets/resources.cfg" 
#define H_RESOURCES_PATH_ASSETS		"Assets/resources_h.cfg"

	// DEBUGLOG
#define H_LOG_FILE			"logs.txt"
	// AUTOMATIZACION BLENDER
#define MAPS_FOLDER					"Assets/Mapas/"
#define EXPORT_SCRIPT				"src/Scripting/exportToLua.py"
#define BLENDER_EXECUTABLE			"Dependencies/Physx/src/physx/tools/BlenderPortable/blender.exe"

#endif // H_DEFS_H