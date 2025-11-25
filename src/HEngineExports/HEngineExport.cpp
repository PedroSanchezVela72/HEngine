#include "HEngineExport.h"
#include "Manager.h"
#include "LuaSingleton.h"
#include "ExceptionSingleton.h"

using namespace H;

void InitManager(H::Manager* mngr, H::IException* excp) {
    ExceptionSingleton::Init(excp);
    mngr->init(_LAST_GRP_ID, _LAST_HDLR_ID, _LAST_SYS_ID, _LAST_CMP_ID);
}

#ifndef _DEBUG
bool InitJuego(H::Manager* mngr)
{
    bool init = true;
    //Añadir sistemas nuevos


    return init;
}
#endif // !_DEBUG

void InitScene(H::ILoadLua* lua) {
    LuaSingleton::Init(lua);

    //Cargar mapa

    //Cargar escenas
    LuaSingleton::GetInstance()->loadScene("../../../Assets/ScriptsLua/Scene.lua");
}

void FlushMessages(H::IManager* mngr) {
    mngr->flushMessages();
}