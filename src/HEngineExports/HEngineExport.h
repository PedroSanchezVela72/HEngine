#ifndef HENGINE_API
#ifdef HENGINE_EXPORTS
#define HENGINE_API __declspec(dllexport)
#else
#define HENGINE_API __declspec(dllimport)
#endif
#endif

namespace H {
	class Manager;
	class ILoadLua;
	class IException;
}

extern "C" HENGINE_API void InitManager(H::Manager* mngr, H::IException* excp);

#ifndef _DEBUG
extern "C" HENGINE_API bool InitJuego(H::Manager* mngr);
#endif // DEBUG

extern "C" HENGINE_API void InitScene(H::ILoadLua* mngr);

extern "C" HENGINE_API void FlushMessages(H::IManager* mngr);