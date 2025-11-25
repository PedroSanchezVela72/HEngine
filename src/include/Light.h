#pragma once
#ifndef LIGHT_H
#define LIGHT_H
#include <Component.h>

namespace Ogre {
	class SceneNode;
	class Light;
}
namespace H {
	class Light : public Component {

	public:
		static constexpr cmpId_type id = _cmp_LIGHT;
		Light() = default;
		~Light() override {}
		void initComponent() override {};
		Ogre::SceneNode* sceneNode = nullptr;
		Ogre::Light* light = nullptr;
		std::string nameLight;
	};
}
#endif	// LIGHT_H