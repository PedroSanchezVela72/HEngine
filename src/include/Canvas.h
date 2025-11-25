#pragma once
#ifndef CANVAS_H
#define CANVAS_H

#include "Component.h"
#include <functional>
#include "Vector2.h"

namespace H {
    class Canvas : public Component {
    public:
        static constexpr cmpId_type id = _cmp_UI_CANVAS;

        Canvas(Vector2F size, bool resizable, bool movable) : size(size), resizable(resizable), movable(movable) {}
        ~Canvas() {}
        bool resizable;
        bool movable;
        bool input = false;
        std::string name;
        Vector2F size;
    };
}
#endif // CANVAS_H