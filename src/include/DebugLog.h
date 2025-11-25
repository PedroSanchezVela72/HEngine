#pragma once

#ifndef DEBUGLOG_H 
#define DEBUGLOG_H 

#include <Singleton.h>
#include <IDebugLog.h>

namespace H {
	class DebugLog : public Singleton<DebugLog>, public IDebugLog {
	public:
		DebugLog();
		~DebugLog() override;

		/// <summary>
		/// Registra una excepcion dentro del archivo de logs.
		/// </summary>
		/// <param name="mesagge">Mensaje del error</param>
		void throwLog(std::string message) override;
	private:
		std::ofstream _file;
	};
}

#endif // !EDITOR_SYSTEM_H 