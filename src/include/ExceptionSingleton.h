#pragma once

#ifndef DEBUGLOG_H 
#define DEBUGLOG_H 

#include "IDebugLog.h"

namespace H {
    class ExceptionSingleton {
    public:
        static IDebugLog*& GetInstance() {
            static IDebugLog* instance = nullptr;
            return instance;
        }

        static void Init(IDebugLog* impl) {
            GetInstance() = impl;
        }
    };
}

#endif // !EDITOR_SYSTEM_H 