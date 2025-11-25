// Engine.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include "Engine.h"
#include <SDL_timer.h>

#include <InputSystem.h>
#include <RenderSystem.h>
#include <PhysicsSystem.h>
#include <AudioSystem.h>
#include <AnimationSystem.h>
#include <ParticleSystem.h>
#include <UISystem.h>
#include <EditorSystem.h>

#include <Manager.h>
#include <LoadLua.h>
#include <Debuglog.h>

#include "H_Defs.h"

using namespace H; 

typedef unsigned __int64    Uint64;
typedef void(__cdecl* ManagerEntryPoint)(IManager*, IDebugLog*);
typedef bool(__cdecl* GameEntryPoint)(IManager*);
typedef void(__cdecl* SceneEntryPoint)(ILoadLua*);
typedef void(__cdecl* FlushMessages)(IManager*);

//Para lanzar el juego con la demo
// #define DEMO

Engine::Engine()
{
	_mngr = new Manager();
	_lua = LoadLua::init(_mngr);

#ifndef DEMO
#ifdef _DEBUG
	_hinstLib = LoadLibrary(TEXT("HEngineExport_d"));
	if (_hinstLib == NULL) {
		DebugLog::instance()->throwLog("No se ha encontrado la dll del juego");
		_mngr->exit();
	}
#else
	_hinstLib = LoadLibrary(TEXT("HEngineExport"));
	if (_hinstLib == NULL) {
		DebugLog::instance()->throwLog("No se ha encontrado la dll del juego");
		_mngr->exit();
	}
#endif // _DEBUG

	ManagerEntryPoint mngrJuego = (ManagerEntryPoint)GetProcAddress(_hinstLib, "InitManager");
	if (mngrJuego) {
		mngrJuego(_mngr, DebugLog::instance());
	}
	else {
		DebugLog::instance()->throwLog("No se pudo encontrar la funcion InitManager");
	}
#else
	_mngr->init();
#endif
}


Engine::~Engine()
{
	delete _mngr;
	LoadLua::close();
	DebugLog::close();
	FreeLibrary(_hinstLib);
}

void Engine::init()
{
	if (_mngr->getExit())
		return;

	_mngr->addSystem<RenderSystem>(H_WINDOW_WIDTH, H_WINDOW_HEIGHT);
	_mngr->addSystem<PhysicsSystem>();
	_mngr->addSystem<InputSystem>();

#pragma region Solo Debug
#ifdef _DEBUG
	_mngr->addSystem<EditorSystem>(
		_mngr,
		_mngr->getSystem<RenderSystem>()->getRoot(),
		_mngr->getSystem<RenderSystem>()->getSceneManager(),
		_mngr->getSystem<RenderSystem>()->getRenderWindow()
		);
#else
	_mngr->addSystem<ParticleSystem>();
	_mngr->addSystem<AnimationSystem>();
	_mngr->addSystem<AudioSystem>();
#endif
	_mngr->addSystem<UISystem>(
		_mngr,
		_mngr->getSystem<RenderSystem>()->getSceneManager(),
		_mngr->getSystem<RenderSystem>()->getRenderWindow()
	);
	DebugLog::instance()->throwLog("Sistemas base iniciados");

#ifndef DEMO
#ifndef _DEBUG
	GameEntryPoint initJuego = (GameEntryPoint)GetProcAddress(_hinstLib, "InitJuego");

	if (initJuego) {
		if (initJuego(_mngr)) {
			DebugLog::instance()->throwLog("DLL del juego cargada");
		}
		else {
			DebugLog::instance()->throwLog("No se pudo inicializar el juego");
			_mngr->exit();
		}
	}
	else {
		DebugLog::instance()->throwLog("No se pudo encontrar la funcion InitJuego");
		_mngr->exit();
	}
#endif

	SceneEntryPoint sceneJuego = (SceneEntryPoint)GetProcAddress(_hinstLib, "InitScene");


	if (sceneJuego) {
		sceneJuego(_lua);
	}
	else {
		DebugLog::instance()->throwLog("No se pudo encontrar la funcion InitScene");
		_mngr->exit();
	}
#else
	LoadLua::instance()->loadScene("Assets/ScriptsLua/Scene.lua");
#endif
#pragma endregion

	_mngr->flushMessages();
#ifdef _DEBUG
	_mngr->getSystem<EditorSystem>()->loadHierarchy();
#endif
	_lua->closeLuaSceneFile();

}

void Engine::run() {

	FlushMessages flushMessages = (FlushMessages)GetProcAddress(_hinstLib, "FlushMessages");


	if (!flushMessages) {
		DebugLog::instance()->throwLog("No se pudo encontrar la funcion FlushMessages");
		return;
	}

	Uint64 startTime = 0;
	Uint64 currentTime = SDL_GetPerformanceCounter();
	_delay = 1.0f; 
	while (!_mngr->getExit()) {
		startTime = currentTime;
		currentTime = SDL_GetPerformanceCounter();
		_deltaTime = (double)((currentTime - startTime) / ((double)SDL_GetPerformanceFrequency() / _delay));

#ifdef _DEBUG
		if (_mngr->isRunningGame())
			flushMessages(_mngr);
		else
			_mngr->flushMessages();
#else
		flushMessages(_mngr);
#endif

		_FUDeltaTime += _deltaTime;
		_mngr->update(_deltaTime);
		if (_FUDeltaTime >= _FUG) {
			_mngr->fixedUpdate(_FUDeltaTime);
			_FUDeltaTime -= _deltaTime;
		}

		_mngr->lateUpdate(_deltaTime);

		_mngr->refresh();

#ifdef _DEBUG
		if (_mngr->getMode() && !_mngr->isRunningGame()) {
			RunGameDebug(currentTime);
		}
		else if (_mngr->getMode() && _mngr->isRunningGame()) {
			ExitRunGameDebug(currentTime);
		}
#endif
	}
}

#ifdef _DEBUG
void Engine::RunGameDebug(Uint64 & currentTime) {
	_mngr->toggleRunning();
	_mngr->changeMode();

	_lua->setImGUINoInitialized();

	_mngr->clearScene();
	_mngr->clearSystems();

	_mngr->addSystem<RenderSystem>(H_WINDOW_WIDTH, H_WINDOW_HEIGHT);
	_mngr->addSystem<PhysicsSystem>();
	_mngr->addSystem<InputSystem>();
	_mngr->addSystem<ParticleSystem>();
	_mngr->addSystem<AnimationSystem>();
	_mngr->addSystem<AudioSystem>();

	_mngr->addSystem<UISystem>(
		_mngr,
		_mngr->getSystem<RenderSystem>()->getSceneManager(),
		_mngr->getSystem<RenderSystem>()->getRenderWindow()
	);

	GameEntryPoint initJuego = (GameEntryPoint)GetProcAddress(_hinstLib, "InitJuego");

	if (initJuego) {
		if (initJuego(_mngr)) {
			DebugLog::instance()->throwLog("DLL del juego cargada");
		}
		else {
			DebugLog::instance()->throwLog("No se pudo inicializar el juego");
			_mngr->exit();
		}
	}
	else {
		DebugLog::instance()->throwLog("No se pudo encontrar la funcion InitJuego");
		_mngr->exit();
	}

	SceneEntryPoint sceneJuego = (SceneEntryPoint)GetProcAddress(_hinstLib, "InitScene");


	if (sceneJuego) {
		sceneJuego(_lua);
	}
	else {
		DebugLog::instance()->throwLog("No se pudo encontrar la funcion InitScene");
		_mngr->exit();
	}

	_mngr->flushMessages();

	_lua->closeLuaSceneFile();

	currentTime = SDL_GetPerformanceCounter();
}

void Engine::ExitRunGameDebug(Uint64& currentTime) {

	_mngr->toggleRunning();
	_mngr->changeMode();

	_lua->setImGUINoInitialized();

	_mngr->clearScene();
	_mngr->clearSystems();

	_mngr->addSystem<RenderSystem>(H_WINDOW_WIDTH, H_WINDOW_HEIGHT);
	_mngr->addSystem<PhysicsSystem>();
	_mngr->addSystem<InputSystem>();

	_mngr->addSystem<EditorSystem>(
		_mngr,
		_mngr->getSystem<RenderSystem>()->getRoot(),
		_mngr->getSystem<RenderSystem>()->getSceneManager(),
		_mngr->getSystem<RenderSystem>()->getRenderWindow()
	);

	_mngr->addSystem<UISystem>(
		_mngr,
		_mngr->getSystem<RenderSystem>()->getSceneManager(),
		_mngr->getSystem<RenderSystem>()->getRenderWindow()
	);

	SceneEntryPoint sceneJuego = (SceneEntryPoint)GetProcAddress(_hinstLib, "InitScene");

	if (sceneJuego) {
		sceneJuego(_lua);
	}
	else {
		DebugLog::instance()->throwLog("No se pudo encontrar la funcion InitScene");
		_mngr->exit();
	}

	_mngr->flushMessages();

	_mngr->getSystem<EditorSystem>()->loadHierarchy();

	_lua->closeLuaSceneFile();

	currentTime = SDL_GetPerformanceCounter();
}
#endif