#include "DebugLog.h"
#include <string>
#include <iostream>
#include <ctime>

using namespace H;

DebugLog::DebugLog() {
    _file.open(H_LOG_FILE, std::ios::trunc);
}

DebugLog::~DebugLog() {
    _file.close();
}

void DebugLog::throwLog(std::string message){
    std::time_t tiempoActual = std::time(nullptr); // Obtener tiempo en segundos desde 1970 (Epoch)
    std::tm tiempoLocal;
    localtime_s(&tiempoLocal, &tiempoActual); // Convertir a formato local

    _file << "[EXCEPTION] -> Hora actual: "
        << tiempoLocal.tm_hour << ":"
        << tiempoLocal.tm_min << ":"
        << tiempoLocal.tm_sec << ": "
        << message << std::endl;
}