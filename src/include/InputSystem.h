#pragma once
#ifndef INPUT_SYSTEM_H
#define INPUT_SYSTEM_H

#include <IInputSystem.h>

namespace H {

	class InputSystem : public IInputSystem
	{
	private:
		std::vector<void*> _controllers;
		std::unordered_map<int, bool> _keyStates;
		std::unordered_map<uint8_t, bool> _mouseButtonStates;
		std::unordered_map<int, std::unordered_map<uint8_t, bool>> _gamepadButtonStates;
		std::unordered_map<int, std::unordered_map<uint8_t, int16_t>> _gamepadAxisStates;
		HPoint _mousePosition;
		int32_t _mouseWheel;

		/// <summary>
		/// Metodo de gestion de tecla pulsada
		/// </summary>
		/// <param name="key">Codigo de la tecla</param>
		void handleKeyDown(int key);

		/// <summary>
		/// Metodo de gestion de tecla levantada
		/// </summary>
		/// <param name="key">Codigo de la tecla</param>
		void handleKeyUp(int key);

		/// <summary>
		/// Metodo de gestion de tecla mantenida
		/// </summary>
		/// <param name="key">Codigo de la tecla</param>
		void handleKeyHeld(int key);

		/// <summary>
		/// Metodo de gestion de movimiento del raton
		/// </summary>
		/// <param name="x">Posicion X del raton</param>
		/// <param name="y">Posicion Y del raton</param>
		/// <param name="xrel">Desplazamiento relativo en X</param>
		/// <param name="yrel">Desplazamiento relativo en Y</param>
		void handleMouseMotion(int x, int y, float xrel, float yrel);

		/// <summary>
		/// Metodo de gestion de la rueda del raton
		/// </summary>
		/// <param name="y">Desplazamiento de la rueda del raton</param>
		void handleMouseWheel(int32_t y);

		/// <summary>
		/// Metodo de gestion de pulsar el click del raton
		/// </summary>
		/// <param name="button">Valor del boton del raton</param>
		void handleMouseDown(uint8_t button);

		/// <summary>
		/// Metodo de gestion de soltar el click del raton
		/// </summary>
		/// <param name="button">Valor del boton del raton</param>
		void handleMouseUp(uint8_t button);

		/// <summary>
		/// Metodo de gestion de mantener el click del raton
		/// </summary>
		/// <param name="button">Valor del boton del raton</param>
		void handleMouseHeld(uint8_t button);

		/// <summary>
		/// Metodo de gestion de pulsar el boton del mando
		/// </summary>
		/// <param name="gamepadIndex">Indice del gamepad</param>
		/// <param name="button">Valor del boton del gamepad</param>
		void handleGamepadButtonDown(int gamepadIndex, uint8_t button);

		/// <summary>
		/// Metodo de gestion de soltar el boton del mando
		/// </summary>
		/// <param name="gamepadIndex">Indice del gamepad</param>
		/// <param name="button">Valor del boton del gamepad</param>
		void handleGamepadButtonUp(int gamepadIndex, uint8_t button);

		/// <summary>
		/// Metodo de gestion de mantener el boton del mando
		/// </summary>
		/// <param name="gamepadIndex">Indice del gamepad</param>
		/// <param name="button">Valor del boton del gamepad</param>
		void handleGamepadButtonHeld(int gamepadIndex, uint8_t button);

		/// <summary>
		/// Metodo de gestion de movimiento del joystick y gatillos del mando
		/// </summary>
		/// <param name="gamepadIndex">Indice del gamepad</param>
		/// <param name="axis">Valor identificativo del joystick</param>
		/// <param name="value">Valor del eje del joystick</param>
		void handleGamepadAxisMotion(int gamepadIndex, uint8_t axis, int16_t value);


		H::KeyCode sdlKeycodeToKeyCode(int key);

		H::MouseButton sdlMouseButtonToMouseButton(uint8_t sdlButton);

		H::GamepadButton sdlGamepadButtonToGamepadButton(uint8_t sdlButton);

		H::GamepadAxis sdlGamepadAxisToGamepadAxis(uint8_t sdlAxis);

	public:
		/// <summary>
		/// Creacion del sistema
		/// </summary>
		InputSystem();

		// <summary>
		/// Destruccion del sistema
		/// </summary>
		~InputSystem();

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
		/// <returns>Descripción del valor retornado si lo hubiese</returns>
		void receive(const Message* m) override;

		/// <summary>
		/// Comprobacion de si la tecla esta pulsada
		/// </summary>
		/// <param name="key">Codigo de la tecla</param>
		/// <returns>True si ha sido presionado, False en caso contrario</returns>
		bool isKeyPressed(int key)override;

		/// <summary>
		/// Comprobacion de si el boton del mouse esta pulsado
		/// </summary>
		/// <param name="button">Valor del boton del raton</param>
		/// <returns>True si ha sido presionado, False en caso contrario</returns>
		bool isMouseButtonPressed(uint8_t button)override;

		/// <summary>
		/// Devuelve la posicion del raton
		/// </summary>
		/// <returns>Posicion del raton</returns>
		HPoint getMousePosition()override;

		/// <summary>
		/// Comprobacion de si el gamepad esta presionado
		/// </summary>
		/// <param name="button">Valor del boton del gamepad</param>
		/// <returns>True si ha sido presionado, False en caso contrario</returns>
		bool isGamepadButtonPressed(int gamepadIndex, uint8_t button)override;

		/// <summary>
		/// Obtiene el eje de movimiento del gampad
		/// </summary>
		/// <param name="axis">Valor identificativo del joystick</param>
		/// <returns>Valor del axis</returns>
		int16_t getGamepadAxis(int gamepadIndex, uint8_t axis)override;

	};
}
#endif // INPUT_SYSTEM_H