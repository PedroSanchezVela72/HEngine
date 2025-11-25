#pragma once
#ifndef IINPUT_SYSTEM_H
#define IINPUT_SYSTEM_H

#include <System.h>

namespace H {

	struct HPoint
	{
		int x;
		int y;
	};

	class IInputSystem : public System
	{
	public:
		static constexpr sysId_type id = _sys_INPUT;

		// <summary>
		/// Destruccion del sistema
		/// </summary>
		virtual ~IInputSystem() {};

		/// <summary>
		/// Inicializacion del sistema
		/// </summary>
		virtual void initSystem() = 0;

		/// <summary>
		/// Funcion update
		/// </summary>
		virtual void update(double deltaTime) = 0;

		/// <summary>
		/// Metodo de gestion de mensajes
		/// </summary>
		/// <param name="m">Mensaje enviado por el manager</param>
		/// <returns>Descripción del valor retornado si lo hubiese</returns>
		virtual void receive(const Message* m) = 0;

		/// <summary>
		/// Comprobacion de si la tecla esta pulsada
		/// </summary>
		/// <param name="key">Codigo de la tecla</param>
		/// <returns>True si ha sido presionado, False en caso contrario</returns>
		virtual bool isKeyPressed(int key) = 0;

		/// <summary>
		/// Comprobacion de si el boton del mouse esta pulsado
		/// </summary>
		/// <param name="button">Valor del boton del raton</param>
		/// <returns>True si ha sido presionado, False en caso contrario</returns>
		virtual bool isMouseButtonPressed(uint8_t button) = 0;

		/// <summary>
		/// Devuelve la posicion del raton
		/// </summary>
		/// <returns>Posicion del raton</returns>
		virtual HPoint getMousePosition() = 0;

		/// <summary>
		/// Comprobacion de si el gamepad esta presionado
		/// </summary>
		/// <param name="button">Valor del boton del gamepad</param>
		/// <returns>True si ha sido presionado, False en caso contrario</returns>
		virtual bool isGamepadButtonPressed(int gamepadIndex, uint8_t button) = 0;

		/// <summary>
		/// Obtiene el eje de movimiento del gampad
		/// </summary>
		/// <param name="axis">Valor identificativo del joystick</param>
		/// <returns>Valor del axis</returns>
		virtual int16_t getGamepadAxis(int gamepadIndex, uint8_t axis) = 0;

	};
}
#endif // INPUT_SYSTEM_H