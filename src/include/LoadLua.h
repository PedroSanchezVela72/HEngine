#pragma once
#ifndef LOAD_LUA_H
#define LOAD_LUA_H
#include <ILoadLua.h>
#include <Singleton.h>

namespace H {
    class LoadLua : public Singleton<LoadLua>, public ILoadLua
    {
    public:
        LoadLua() {};
        LoadLua(Manager* mngr);
        ~LoadLua() override;

        std::list<std::string> getEntitiesNames() override { return sceneEntitiesNames; };
#ifdef _DEBUG
        Entity* getEntity(std::string name) override { return sceneEntities[name]; };
#endif

        void luaClose()override;
        void loadScene(const std::string scenePath)override; 
        
        void closeLuaSceneFile();
        bool hasComponent(const std::string entity, const std::string component) override;
        bool hasProperty(std::string entityName, std::vector<std::string> args)override;
        std::string loadStrValue(std::string entityName, std::vector<std::string> args)override;
        int loadIntValue(std::string entityName, std::vector<std::string> args)override;
        float loadFloatValue(std::string entityName, std::vector<std::string> args)override;
        bool loadBoolValue(std::string entityName, std::vector<std::string> args)override;
        bool saveStrValue(std::string entityName, std::vector<std::string> args, std::string value)override;
        bool saveIntValue(std::string entityName, std::vector<std::string> args, int value)override;
        bool saveFloatValue(std::string entityName, std::vector<std::string> args, float value)override;
        bool saveBoolValue(std::string entityName, std::vector<std::string> args, bool value)override;
        void loadMapInScene(const std::string& mapPath, const std::string& scenePath)override;
        std::string loadHierarchy(std::list<PropertyData>& hierarchy)override;
        void saveHierarchy(std::list<PropertyData>& hierarchy)override;
        void searchTable(std::list<PropertyData>& properties, std::vector<std::string>& args, int& profundidad)override;
        void writeEntities(const std::vector<std::string>& entityNames)override;

        void setImGUINoInitialized() { _imguiInitiated = false; };
    private:
        lua_State* L = nullptr;
        Manager* _mngr = nullptr;
        std::list<std::string> sceneEntitiesNames;

        bool _imguiInitiated = false;
        bool sceneLoaded = true;

#ifdef _DEBUG
        std::unordered_map<std::string, Entity*> sceneEntities;
#endif

        void initLua();
    };
}
#endif // LOAD_LUA_H