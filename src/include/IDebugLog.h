#pragma once

#ifndef IDEBUGLOG_H 
#define IDEBUGLOG_H 

#include <fstream>
#include "H_Defs.h"

namespace H {
	class IDebugLog {
	public:
		virtual ~IDebugLog() {};

		/// <summary>
		/// Registra una excepcion dentro del archivo de logs.
		/// </summary>
		/// <param name="mesagge">Mensaje del error</param>
		virtual void throwLog(std::string message) = 0;
	};
}

#endif // !EDITOR_SYSTEM_H 