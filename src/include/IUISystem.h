#ifndef IUI_SYSTEM_H
#define IUI_SYSTEM_H

#include "System.h"
namespace Ogre {
#ifdef _DEBUG
	namespace DEBUG_BUILD_REQUIRED {
		class Root;
	}
	using Root = DEBUG_BUILD_REQUIRED::Root;  // Define Ogre::Root como alias en Debug
#else
	class Root;
#endif

	class SceneManager;
	class RenderWindow;
	class ImguiManager;
	class SceneNode;
}

struct ImFont;

namespace H {

	class Manager;
	struct Message;

	class IUISystem : public System {
	public:
		static constexpr sysId_type id = _sys_UIGAME;

		virtual ~IUISystem() {};

		virtual void update(double deltaTime) = 0;
		virtual void receive(const Message* m) = 0;
		virtual void initSystem() = 0;
	};
}
#endif // UI_SYSTEM_H

