#pragma once
#ifndef RENDER_MESH_H
#define RENDER_MESH_H
#include <Component.h>
#include <string>

namespace Ogre {
	class SceneNode;
	class Entity;
}

namespace H {

	class RenderMesh : public Component
	{
	public:
		static constexpr cmpId_type id = _cmp_RENDERMESH;
		RenderMesh() = default;
		RenderMesh(const std::string& mesh, const std::string& material) : nameMesh(mesh), material(material) {}
		std::string material;
		std::string nameMesh;
		Ogre::Entity* ogreEntity = nullptr;

		Ogre::SceneNode* sceneNode = nullptr;	};
}
#endif // RENDER_MESH_H
