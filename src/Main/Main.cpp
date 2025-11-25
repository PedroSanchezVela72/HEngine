// Main.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.

#include "Engine.h"

#ifdef _DEBUG
    #include <crtdbg.h>
    #include <iostream>
#endif

#ifdef _DEBUG
int main(int argc, char** argv) {
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
#endif

#ifdef _DEBUG
    //_CrtSetBreakAlloc(166); //Para ver cuando se crea lo que no se destruye, se tiene que poner como parametro el bloque de memory que aparece en la salida del visual
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    H::Engine* engine = new H::Engine();
    engine->init();
    engine->run();
    delete engine;


#ifdef _DEBUG

    // Estado de la memoria antes de salir
    _CrtMemState memState;
    _CrtMemCheckpoint(&memState);  // Guarda el estado actual de la memoria
#endif
    
    return 0;
}

