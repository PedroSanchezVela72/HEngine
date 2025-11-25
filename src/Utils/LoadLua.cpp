#include "LoadLua.h"
#include <Transform.h>
#include <RenderMesh.h>
#include <Manager.h>
#include <Debuglog.h>
#include <windows.h>
#include <filesystem>
extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

using namespace H;

LoadLua::LoadLua(Manager* mngr): _mngr(mngr)
{
    initLua();
}

LoadLua::~LoadLua()
{
    if(L !=nullptr)
        luaClose();
}

void LoadLua::initLua()
{
    // 1) Crear el estado de Lua
    L = luaL_newstate();
    if (!L) {
        DebugLog::instance()->throwLog("Failed to create the Lua state!");
        return;
    }
  
    // 2) Abrir las librerías estándar de Lua
    luaL_openlibs(L);
}

void LoadLua::luaClose()
{
    lua_close(L);
}


void LoadLua::loadScene(const std::string scenePath) {
    DebugLog::instance()->throwLog("Cargando escena " + scenePath);
    if (luaL_dofile(L, scenePath.c_str()) != LUA_OK) {
        sceneLoaded = false;
        std::string error = "Error al cargar : ";
        error += scenePath;
        DebugLog::instance()->throwLog(error);
        lua_pop(L, 1);
        MessageBoxA(NULL, error.c_str(), "Excepción", MB_ICONERROR | MB_OK);
        _mngr->exit();
    }
    _mngr->setSceneName(scenePath);

    lua_getglobal(L, "entities");
    if (!lua_istable(L, -1)) {
        std::string error = "No 'entities' table found in entities";
        DebugLog::instance()->throwLog(error);
        lua_pop(L, 1);
        return;
    }

    DebugLog::instance()->throwLog("Cargando entidades");
    lua_pushnil(L);
    std::unordered_map<std::string, Entity*> entities;
    std::vector<std::pair<std::string, std::string>> parenthood;
    while (lua_next(L, -2) != 0) {
        std::string name = lua_tostring(L, -1);
        sceneEntitiesNames.push_back(name);
        DebugLog::instance()->throwLog("Cargando la entidad " + name);

        if (hasProperty(name, { "Parent" })) {
            entities[name] = _mngr->addEntity(grpId::_grp_ENTITIES);
            parenthood.push_back({ name, loadStrValue(name, {"Parent"}) }); // child / parent
        }
        else {
            entities[name] = _mngr->addEntity(grpId::_grp_PARENTENTS);
        }
        if (hasProperty(name, { "Active" })) {
            entities[name]->setActive(loadBoolValue(name, { "Active" }));
        }
        else
            entities[name]->setActive(true);


        #ifdef _DEBUG
        Message* m = new Message;
        m->id = _m_INIT_ENTITY;
        sceneEntities[name] = entities[name];
        m->entity.entityPtr = sceneEntities[name];
        m->entity.entityName = name;
        _mngr->send(m, true);
        #else
        Message* m = new Message;
        m->id = _m_INIT_ENTITY;
        m->entity.entityPtr = entities[name];
        m->entity.entityName = name;
        _mngr->send(m,true);
        #endif

        lua_pop(L, 1);
    }
    for (std::pair<std::string, std::string> e : parenthood) {
        _mngr->addEntityParent(entities[e.first], entities[e.second]); 
    }
}

void LoadLua::closeLuaSceneFile() {
    int top = lua_gettop(L);
    lua_pop(L, top);
    if (sceneLoaded) {
        if (!_imguiInitiated) {
            Message* m = new Message;
            m->id = _m_INIT_IMGUIMANAGER;
            _mngr->send(m, true);
            _imguiInitiated = true; //para evitar que se inicialize 2 veces
        }
        Message* m = new Message;
        m->id = _m_ADD_FONT_TEXTURE;
        m->scene.incScene++;
        _mngr->send(m);
    }
    
}

bool LoadLua::hasComponent(const std::string entity, const std::string component) {
    lua_getglobal(L, entity.c_str());
    if (!lua_istable(L,-1)) {
        std::string error = "No se ha encontrado la entidad: ";
        error += entity;
        DebugLog::instance()->throwLog(error);
        lua_pop(L, 1);
        return false;
    }

    lua_getfield(L, -1, component.c_str());
    if (!lua_istable(L,-1)) {
        lua_pop(L, 2);
        return false;
    }
    lua_pop(L, 2);
    return true;
}

bool LoadLua::hasProperty(std::string entityName, std::vector<std::string> args) {
    int contador = 1;

    lua_getglobal(L, entityName.c_str());
    if (!lua_istable(L, -1)) {
        std::string error = "No se ha encontrado la entidad: ";
        error += entityName;
        DebugLog::instance()->throwLog(error);
        lua_pop(L, 1);
        return false;
    }

    for (int i = 0; i < args.size() - 1; i++) {
        std::string s = args[i];
        contador++;
        lua_getfield(L, -1, s.c_str());
        if (!lua_istable(L, -1)) {
            lua_pop(L, contador);
            return false;
        }
    }

    contador++;
    lua_getfield(L, -1, args[args.size() - 1].c_str());
    if (lua_isnil(L, -1)) {
        lua_pop(L, contador);
        return false;
    }

    lua_pop(L, contador);
    return true;
}

std::string LoadLua::loadStrValue(std::string entityName, std::vector<std::string> args) {
    int contador = 1;

    lua_getglobal(L, entityName.c_str());
    if (!lua_istable(L,-1)) {
        std::string error = "No se ha encontrado la entidad: ";
        error += entityName;
        DebugLog::instance()->throwLog(error);
    }

    for (int i = 0; i < args.size() - 1; i++) {
        std::string s = args[i];
        contador++;
        lua_getfield(L, -1, s.c_str());
        if (!lua_istable(L, -1)) {
            std::string error = "No se ha encontrado el valor: ";
            error += s;
            error += " en la entidad: ";
            error += entityName;
            DebugLog::instance()->throwLog(error);
            lua_pop(L, contador);
            return "";
        }
    }

    contador++;
    lua_getfield(L, -1, args[args.size() - 1].c_str());
    if (!lua_isstring(L, -1)) {
        std::string error = "No se ha encontrado el valor: ";
        error += args[args.size() - 1];
        error += " en la entidad: ";
        error += entityName;
        DebugLog::instance()->throwLog(error);
        lua_pop(L, contador);
        return "";
    }

    std::string value = lua_tostring(L, -1);

    lua_pop(L,contador);

    return value;
}

int LoadLua::loadIntValue(std::string entityName, std::vector<std::string> args) {
    int contador = 1;

    lua_getglobal(L, entityName.c_str());
    if (!lua_istable(L, -1)) {
        std::string error = "No se ha encontrado la entidad: ";
        error += entityName;
        DebugLog::instance()->throwLog(error);
    }

    for (int i = 0; i < args.size() - 1; i++) {
        std::string s = args[i];
        contador++;
        lua_getfield(L, -1, s.c_str());
        if (!lua_istable(L, -1)) {
            std::string error = "No se ha encontrado el valor: ";
            error += s;
            error += " en la entidad: ";
            error += entityName;
            DebugLog::instance()->throwLog(error);
            lua_pop(L, contador);
            return 0;
        }
    }

    contador++;
    lua_getfield(L, -1, args[args.size() - 1].c_str());
    if (!lua_isinteger(L, -1)) {
        std::string error = "No se ha encontrado el valor: ";
        error += args[args.size() - 1];
        error += " en la entidad: ";
        error += entityName;
        DebugLog::instance()->throwLog(error);
        lua_pop(L, contador);
        return 0;
    }

    int value = (int)lua_tointeger(L, -1);

    lua_pop(L, contador);

    return value;
}

float LoadLua::loadFloatValue(std::string entityName, std::vector<std::string> args) {
    int contador = 1;

    lua_getglobal(L, entityName.c_str());
    if (!lua_istable(L, -1)) {
        std::string error = "No se ha encontrado la entidad: ";
        error += entityName;
        DebugLog::instance()->throwLog(error);
    }

    for (int i = 0; i < args.size() - 1; i++) {
        std::string s = args[i];
        contador++;
        lua_getfield(L, -1, s.c_str());
        if (!lua_istable(L, -1)) {
            std::string error = "No se ha encontrado el valor: ";
            error += s;
            error += " en la entidad: ";
            error += entityName;
            DebugLog::instance()->throwLog(error);
            lua_pop(L, contador);
            return 0;
        }
    }

    contador++;
    lua_getfield(L, -1, args[args.size() - 1].c_str());
    if (!lua_isnumber(L, -1)) {
        std::string error = "No se ha encontrado el valor: ";
        error += args[args.size() - 1];
        error += " en la entidad: ";
        error += entityName;
        DebugLog::instance()->throwLog(error);
        lua_pop(L, contador);
        return 0;
    }

    float value = (float)lua_tonumber(L, -1);

    lua_pop(L, contador);

    return value;
}

bool LoadLua::loadBoolValue(std::string entityName, std::vector<std::string> args) {
    int contador = 1;

    lua_getglobal(L, entityName.c_str());
    if (!lua_istable(L, -1)) {
        std::string error = "No se ha encontrado la entidad: ";
        error += entityName;
        DebugLog::instance()->throwLog(error);
    }

    for (int i = 0; i < args.size() - 1; i++) {
        std::string s = args[i];
        contador++;
        lua_getfield(L, -1, s.c_str());
        if (!lua_istable(L, -1)) {
            std::string error = "No se ha encontrado el valor: ";
            error += s;
            error += " en la entidad: ";
            error += entityName;
            DebugLog::instance()->throwLog(error);
            lua_pop(L, contador);
            return 0;
        }
    }

    contador++;
    lua_getfield(L, -1, args[args.size() - 1].c_str());
    if (!lua_isboolean(L, -1)) {
        std::string error = "No se ha encontrado el valor: ";
        error += args[args.size() - 1];
        error += " en la entidad: ";
        error += entityName;
        DebugLog::instance()->throwLog(error);
        lua_pop(L, contador);
        return 0;
    }
    bool value = lua_toboolean(L, -1);

    lua_pop(L, contador);

    return value;
}

bool LoadLua::saveStrValue(std::string entityName, std::vector<std::string> args, std::string value) {
    int contador = 1;

    lua_getglobal(L, entityName.c_str());
    if (!lua_istable(L, -1)) {
        std::string error = "No se ha encontrado la entidad: ";
        error += entityName;
        DebugLog::instance()->throwLog(error);
    }

    for (int i = 1; i < args.size() - 1; i++) {
        std::string s = args[i];
        contador++;
        lua_getfield(L, -1, s.c_str());
        if (!lua_istable(L, -1)) {
            std::string error = "No se ha encontrado el valor: ";
            error += s;
            error += " en la entidad: ";
            error += entityName;
            DebugLog::instance()->throwLog(error);
            lua_pop(L, contador);
            return false;
        }
    }

    lua_pushstring(L, value.c_str());
    if (!lua_isstring(L, -1)) {
        std::string error = "No se ha encontrado el valor: ";
        error += args[args.size() - 1];
        error += " en la entidad: ";
        error += entityName;
        DebugLog::instance()->throwLog(error);
        lua_pop(L, contador);
        return false;
    }
    lua_setfield(L, -2, args[args.size() - 1].c_str());
    

    lua_pop(L, contador);

    return true;
}

bool LoadLua::saveIntValue(std::string entityName, std::vector<std::string> args, int value) {
    int contador = 1;

    lua_getglobal(L, entityName.c_str());
    if (!lua_istable(L, -1)) {
        std::string error = "No se ha encontrado la entidad: ";
        error += entityName;
        DebugLog::instance()->throwLog(error);
    }

    for (int i = 1; i < args.size() - 1; i++) {
        std::string s = args[i];
        contador++;
        lua_getfield(L, -1, s.c_str());
        if (!lua_istable(L, -1)) {
            std::string error = "No se ha encontrado el valor: ";
            error += s;
            error += " en la entidad: ";
            error += entityName;
            DebugLog::instance()->throwLog(error);
            lua_pop(L, contador);
            return false;
        }
    }

    lua_pushinteger(L, value);

    if (!lua_isinteger(L, -1)) {
        std::string error = "No se ha encontrado el valor: ";
        error += args[args.size() - 1];
        error += " en la entidad: ";
        error += entityName;
        DebugLog::instance()->throwLog(error);
        lua_pop(L, contador);
        return false;
    }

    lua_setfield(L, -2, args[args.size() - 1].c_str());

    lua_pop(L, contador);

    return true;
}

bool LoadLua::saveFloatValue(std::string entityName, std::vector<std::string> args, float value) {
    int contador = 1;

    lua_getglobal(L, entityName.c_str());
    if (!lua_istable(L, -1)) {
        std::string error = "No se ha encontrado la entidad: ";
        error += entityName;
        DebugLog::instance()->throwLog(error);
    }

    for (int i = 1; i < args.size() - 1; i++) {
        std::string s = args[i];
        contador++;
        lua_getfield(L, -1, s.c_str());
        if (!lua_istable(L, -1)) {
            std::string error = "No se ha encontrado el valor: ";
            error += s;
            error += " en la entidad: ";
            error += entityName;
            DebugLog::instance()->throwLog(error);
            lua_pop(L, contador);
            return false;
        }
    }

    lua_pushnumber(L, value);
    if (!lua_isnumber(L, -1)) {
        std::string error = "No se ha encontrado el valor: ";
        error += args[args.size() - 1];
        error += " en la entidad: ";
        error += entityName;
        DebugLog::instance()->throwLog(error);
        lua_pop(L, contador);
        return false;
    }
    lua_setfield(L, -2, args[args.size() - 1].c_str());
    

    lua_pop(L, contador);

    return true;
}

bool LoadLua::saveBoolValue(std::string entityName, std::vector<std::string> args, bool value) {
    int contador = 1;

    lua_getglobal(L, entityName.c_str());
    if (!lua_istable(L, -1)) {
        std::string error = "No se ha encontrado la entidad: ";
        error += entityName;
        DebugLog::instance()->throwLog(error);
    }

    for (int i = 1; i < args.size() - 1; i++) {
        contador++;
        std::string s = args[i];
        lua_getfield(L, -1, s.c_str());
        if (!lua_istable(L, -1)) {
            std::string error = "No se ha encontrado el valor: ";
            error += s;
            error += " en la entidad: ";
            error += entityName;
            DebugLog::instance()->throwLog(error);
            lua_pop(L, contador);
            return false;
        }
    }

    lua_pushboolean(L, value);  
    if (!lua_isboolean(L, -1)) {
        std::string error = "No se ha encontrado el valor: ";
        error += args[args.size() - 1];
        error += " en la entidad: ";
        error += entityName;
        DebugLog::instance()->throwLog(error);
        lua_pop(L, contador);
        return false;
    }
    lua_setfield(L, -2, args[args.size() - 1].c_str());
    lua_pop(L, contador);

    return true;
}

std::string LoadLua::loadHierarchy(std::list<PropertyData>& hierarchy) {
    hierarchy.clear();
    std::list<PropertyData> entityProperties;
    lua_getglobal(L, "entities");
    if (!lua_istable(L, -1)) {
        std::string error = "No se encontró la tabla 'entities' en Lua";
        DebugLog::instance()->throwLog(error);
        lua_pop(L, 1);
        return "Error: No se encontró 'entities' en Lua";
    }

    lua_pushnil(L);  // Poner el primer índice de la tabla de entidades
    while (lua_next(L, -2)) {
        const char* entityNameRaw = lua_tostring(L, -1);
        if (!entityNameRaw) {
            lua_pop(L, 1);
            continue;
        }
        std::string entityName(entityNameRaw);
        PropertyData entityData;
        entityData.profundidad = 0;
        entityData.args.push_back(entityName);
        entityProperties.push_back(entityData);
        lua_pop(L, 1);  // Limpiar el valor de la pila

        lua_getglobal(L, entityName.c_str());  // Obtener la tabla de la entidad por su nombre
        if (lua_isnil(L, -1)) {
            lua_pop(L, 1);
            std::string error = "Advertencia: No se encontró la entidad '";
            error += entityName;
            error += "' en Lua";
            DebugLog::instance()->throwLog(error);
            continue;
        }
        else if (!lua_istable(L, -1)) {
            lua_pop(L, 1);
            continue;
        }

        

        lua_pushnil(L);  // Poner el primer índice de la tabla de componentes
        while (lua_next(L, -2)) {
            const char* componentNameRaw = lua_tostring(L, -2);
            if (!componentNameRaw) {
                lua_pop(L, 1);
                continue;
            }
            std::string componentName(componentNameRaw);
            PropertyData componentData;
            componentData.profundidad = 1;
            componentData.args.push_back(entityName);
            componentData.args.push_back(componentName);
            entityProperties.push_back(componentData);
            if (!lua_istable(L, -1)) {
                lua_pop(L, 1);
                continue;
            }
         

            lua_pushnil(L);  // Poner el primer índice de las propiedades del componente
            while (lua_next(L, -2)) {
                const char* propertyNameRaw = lua_tostring(L, -2);
                if (!propertyNameRaw) {
                    lua_pop(L, 1);
                    continue;
                }
                std::string propertyName(propertyNameRaw);
                std::vector<std::string> args;
                args.push_back(entityName);
                args.push_back(componentName);
                args.push_back(propertyName);
                int profundidad = 2;
                searchTable(entityProperties, args, profundidad); 
                
                lua_pop(L, 1);
            }        
            lua_pop(L, 1);  // Limpiar el valor de la pila
        }

        hierarchy = entityProperties;  // Asignar los componentes de la entidad al jerarquía
        lua_pop(L, 1);  // Limpiar la tabla de la entidad
    }

    lua_pop(L, 1);  // Limpiar la tabla 'entities'
    return "Hierarchy loaded successfully";
}

void H::LoadLua::saveHierarchy(std::list<PropertyData>& hierarchy)
{
    for (const auto& property : hierarchy) { 
        switch (property.tipo) {
        case INT:
            saveIntValue(property.args[0], property.args, property.iValue);
            break;
        case BOOL:
            saveBoolValue(property.args[0], property.args, property.bValue);
            break;
        case FLOAT:
            saveFloatValue(property.args[0], property.args, property.fValue);
            break;
        case STRING:
            saveStrValue(property.args[0], property.args, property.sValue);
            break;
        }
    }
}

void H::LoadLua::searchTable(std::list<PropertyData>& properties, std::vector<std::string>& args, int& profundidad)
{
    PropertyData data;

    //Si es una tabla, guarda la clave, y hace lo mismo con los elementos de la tabla
    if (lua_istable(L, -1)) {
        profundidad++;
        data.tipo = TABLE;
        lua_pushnil(L);
        while (lua_next(L, -2)) {
            std::string key = lua_tostring(L, -2);
            args.push_back(key);          
            searchTable(properties, args, profundidad);         
            args.pop_back();
            lua_pop(L, 1);
        } 
        profundidad--;
    }
    else if (lua_isnumber(L, -1)) {
        if (lua_isinteger(L, -1) && args[1] != "Transform") {
            data.tipo = INT;
            data.iValue = (int)lua_tointeger(L, -1);
        }
        else {
            data.tipo = FLOAT;
            data.fValue = (float)lua_tonumber(L, -1);
        }
    }
    else if (lua_isstring(L, -1)) {
        data.tipo = STRING;
        data.sValue = lua_tostring(L, -1);
    }
    else if (lua_isboolean(L, -1)) {
        data.tipo = BOOL;
        data.bValue = lua_toboolean(L, -1);
    }
    data.args = args;
    data.profundidad = profundidad;
    properties.push_back(data);
}

void H::LoadLua::writeEntities(const std::vector<std::string>& entityNames)
{
    std::string utilsPath = H_LUA_UTILS;

    if (luaL_dofile(L, utilsPath.c_str()) != LUA_OK) {
        std::string error = "Error ejecutando " + utilsPath + ": " + lua_tostring(L, -1);
        DebugLog::instance()->throwLog(error);
        lua_pop(L, 1);
        return;
    }
    std::string scene = _mngr->getSceneName();
    std::ofstream file(scene, std::ios::trunc);
    DebugLog::instance()->throwLog("Escribiendo entidades en " + scene );
    file << "entities = { " << std::quoted(entityNames[0]);
    for (int i = 1; i < entityNames.size(); i++)
        file << "," << std::quoted(entityNames[i]);
    file << "}\n\n";
    file.close();
    for (const auto& name : entityNames) {
        lua_getglobal(L, "saveEntityToFile");
        lua_pushstring(L, name.c_str());
        lua_pushstring(L, scene.c_str());
        if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
            std::cerr << "Error al guardar: " << lua_tostring(L, -1) << std::endl;
        }
    }    
}



void LoadLua::loadMapInScene(const std::string& mapPath, const std::string& scenePath) {
    std::string scriptPath = H_MAP_EXPORTER;

    if (luaL_dofile(L, scriptPath.c_str()) != LUA_OK) {
        std::string error =  "Error ejecutando " + scriptPath + ": " + lua_tostring(L, -1);
        DebugLog::instance()->throwLog(error);
        lua_pop(L, 1);
        return;
    }

    lua_getglobal(L, "loadMap"); // Obtiene la función de Lua
    if (!lua_isfunction(L, -1)) {
        std::string error = "No se encontro la funcion loadMap en " + scriptPath;
        DebugLog::instance()->throwLog(error);
        lua_pop(L, 1);
        return;
    }

    lua_pushstring(L, mapPath.c_str());  // Primer argumento
    lua_pushstring(L, scenePath.c_str()); // Segundo argumento

    if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
        std::string error = std::string("Error llamando a loadMap: ") + lua_tostring(L, -1);
        DebugLog::instance()->throwLog(error);
        lua_pop(L, 1);
    }   
}



