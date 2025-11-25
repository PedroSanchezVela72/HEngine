#pragma once
#ifndef UI_SLIDER_COMPONENT_H
#define UI_SLIDER_COMPONENT_H

#include "UIElement.h"
struct ImFont;
namespace H {

    class UISliderComponent : public UIElement
    {
    public:
        static constexpr cmpId_type id = _cmp_UI_SLIDER;

        UISliderComponent(bool _interactive, 
            float minValue = H_SLIDER_MIN_DEF_VALUE, float maxValue = H_SLIDER_MAX_DEF_VALUE, 
            float value = H_SLIDER_MAX_DEF_VALUE / 2.f, bool wholeNumbers = false, Vector2F size = Vector2F(H_SIZE_WIND_WIDTH, H_SIZE_WIND_HEIGHT),
            Color barColor = Color::white(), Color barColorHover = Color(H_UI_WHITEHOVERED), Color barColorActive = Color(H_UI_WHITEACTIVE),
            Color handleColor = Color::black(), Color handleColorActive = Color(H_UI_BLACKHOVERED),
            std::string text="", bool vertical = false, ImFont* font = nullptr) : minValue(minValue), maxValue(maxValue), value(value),
            wholeNumbers(wholeNumbers), barColor(barColor), barColorHover(barColorHover), barColorActive(barColorActive),
            handleColor(handleColor), handleColorActive(handleColorActive), text(text),
            vertical(vertical), font(font)
        {
            interactive = _interactive;
            sizeWind = size;
        }
        ~UISliderComponent() {}

        // Rango del slider (minValue, maxValue)
        float minValue;
        float maxValue;

        // Valor actual del slider
        float value;

        // Paso de incremento (si es entero o flotante)
        bool wholeNumbers;  // Si es un slider con valores enteros o flotantes

        // Color de la barra y el control deslizante
        Color barColor;
        Color barColorHover;
        Color barColorActive;
        Color handleColor;
        Color handleColorActive;
        ImFont* font = nullptr;    // Fuente personalizada

        // Dirección del slider (horizontal o vertical)
        bool vertical;

        std::string text;
    };
}
#endif // UI_SLIDER_COMPONENT_H 