#pragma once
#ifndef LOAD_LUA_H
#define LOAD_LUA_H

#include <ILoadLua.h>

namespace H {

    class LuaSingleton
    {
    public:
        static ILoadLua*& GetInstance() {
            static ILoadLua* instance = nullptr;
            return instance;
        }

        static void Init(ILoadLua* impl) {
            GetInstance() = impl;
        }
    };
}
#endif // LOAD_LUA_H