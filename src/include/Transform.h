#pragma once
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Component.h"
#include "Quaternion.h"
#include "IManager.h"
#include "Entity.h"

namespace H {
    class IManager;
    class Entity;

    struct Transform : public Component {
    public:
        static constexpr cmpId_type id = _cmp_TRANSFORM;

        Vector3F position;
        QuaternionF rotation;
        Vector3F scale;

        Transform(const Vector3F& pos = Vector3F(0, 0, 0), const QuaternionF& rot = QuaternionF(1, 0, 0, 0), const Vector3F& scl = Vector3F(1, 1, 1))
            : position(pos), rotation(rot), scale(scl) {
        }

        inline Vector3F getGlobalPosition() {
            if (_ent->getParent() == nullptr)
                return position;

            Transform* parentTr = _mngr->getComponent<Transform>(_ent->getParent());
            if (!parentTr) return position; // por si acaso

            // Devolver la posición global resultante
            return parentTr->getGlobalPosition() + parentTr->getGlobalRotation() * (parentTr->getGlobalScale() * position);
        }

        inline Vector3F getGlobalScale() {
            if (_ent->getParent() == nullptr)
                return scale;

            Transform* parentTransform = _mngr->getComponent<Transform>(_ent->getParent());
            if (!parentTransform) return scale;

            return scale * parentTransform->getGlobalScale();
        }

        inline QuaternionF getGlobalRotation() {
            if (_ent->getParent() == nullptr)
                return rotation;

            Transform* parentTransform = _mngr->getComponent<Transform>(_ent->getParent());
            if (!parentTransform) return rotation;

            return rotation * parentTransform->getGlobalRotation();
        }

    };
}
#endif