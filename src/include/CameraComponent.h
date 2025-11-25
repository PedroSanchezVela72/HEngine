#pragma once
#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H
#include <Component.h>

namespace Ogre {
	class SceneNode;
	class Camera;
    class Viewport;
}

namespace H {
    class CameraComponent : public Component
    {
    public:
        static constexpr cmpId_type id = _cmp_CAMERA;  

        CameraComponent(std::string cameraName, Vector3F backgroundColor = H_CAM_CMP_BG_COLOR ,
            float nearClipDistance = H_CAM_CMP_NEAR_CDIST, float farClipDistance = H_CAM_CMP_FAR_CDIST, bool cursorOnScreen = true, bool lookAt = false, Vector3F target = Vector3F::zero())
            : nearClipDistance(nearClipDistance), farClipDistance(farClipDistance), cursorOnScreen(cursorOnScreen), lookAt(lookAt), target(target),
            cameraName(cameraName), backgroundColor(backgroundColor) {}

        float nearClipDistance;
        float farClipDistance;
        bool lookAt;
        bool cursorOnScreen;
        Vector3F target;
        Vector3F backgroundColor;
        std::string cameraName;

        Ogre::SceneNode* cameraNode = nullptr;
        Ogre::SceneNode* yawNode = nullptr;
        Ogre::SceneNode* pitchNode = nullptr;
        Ogre::Camera* camera = nullptr;
        Ogre::Viewport* viewport = nullptr;
    };
}
#endif // CAMERA_COMPONENT_H

