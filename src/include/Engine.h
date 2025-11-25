#pragma once
#ifndef ENGINE_H
#define ENGINE_H

#include <vector>
#include <crtdbg.h>
#include <Windows.h>

namespace H {
	class Manager;
	class LoadLua;

	class Engine {
	public:
		Engine();
		~Engine();
		//--------Puede que esto no vaya aqui
		void init();
		/// <summary>
		/// Inicia el bucle principal del motor
		/// </summary>
		void run();

	private:

		Manager* _mngr;
		LoadLua* _lua;

		float _delay;
		double _deltaTime;
		double _FUDeltaTime;

		//Tiempo entre fixed updates
		double _FUG = 1/60;

		HMODULE _hinstLib; //A ver si funciona

		void RunGameDebug(unsigned __int64& currentTime);

		void ExitRunGameDebug(unsigned __int64& currentTime);
	};
}
#endif // ENGINE_H 