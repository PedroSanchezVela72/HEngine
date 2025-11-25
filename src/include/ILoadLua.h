#pragma once
#ifndef ILOAD_LUA_H
#define ILOAD_LUA_H
#include <string>
#include <iostream>
#include <unordered_map>
#include <list>
#include <utility>
#include "Vector3.h"

class lua_State;
namespace H {
    class Manager;
    class Transform;
    class Entity;
    struct PropertyData;
    

    class ILoadLua
    {
    public:

        static ILoadLua* LuaSingleton;

        virtual ~ILoadLua() {};

        virtual std::list<std::string> getEntitiesNames() = 0;
#ifdef _DEBUG
        virtual Entity* getEntity(std::string name) = 0;
#endif

        virtual void luaClose() = 0;
        virtual void loadScene(const std::string scenePath) = 0;
        
        virtual void closeLuaSceneFile() = 0;
        virtual bool hasComponent(const std::string entity, const std::string component) = 0;
        virtual bool hasProperty(std::string entityName, std::vector<std::string> args) = 0;
        virtual std::string loadStrValue(std::string entityName, std::vector<std::string> args) = 0;
        virtual int loadIntValue(std::string entityName, std::vector<std::string> args) = 0;
        virtual float loadFloatValue(std::string entityName, std::vector<std::string> args) = 0;
        virtual bool loadBoolValue(std::string entityName, std::vector<std::string> args) = 0;
        virtual bool saveStrValue(std::string entityName, std::vector<std::string> args, std::string value) = 0;
        virtual bool saveIntValue(std::string entityName, std::vector<std::string> args, int value) = 0;
        virtual bool saveFloatValue(std::string entityName, std::vector<std::string> args, float value) = 0;
        virtual bool saveBoolValue(std::string entityName, std::vector<std::string> args, bool value) = 0;
        virtual void loadMapInScene(const std::string& mapPath, const std::string& scenePath) = 0;
        virtual std::string loadHierarchy(std::list<PropertyData>& hierarchy) = 0;
        virtual void saveHierarchy(std::list<PropertyData>& hierarchy) = 0;
        virtual void searchTable(std::list<PropertyData>& properties, std::vector<std::string>& args, int& profundidad) = 0;
        virtual void writeEntities(const std::vector<std::string>& entityNames) = 0;
    };
}
#endif // LOAD_LUA_H