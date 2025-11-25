#pragma once
#ifndef UI_ELEMENT_H
#define UI_ELEMENT_H

#include <string>
#include "Component.h"
#include "Color.h"
#include "Vector2.h"

namespace H {

    /// <summary>Clase base para elementos de UI</summary>
    class UIElement : public Component {
    public:
        static constexpr cmpId_type id = _cmp_UI_ELEMENT;
        UIElement() : interactive(true), sizeWind(Vector2F(H_SIZE_WIND_WIDTH, H_SIZE_WIND_HEIGHT)) {}
        virtual ~UIElement() {}
        std::string name;
        bool interactive;   // Definir si puede recibir eventos
        Vector2F sizeWind;
    };
}
#endif // UI_ELEMENT_H