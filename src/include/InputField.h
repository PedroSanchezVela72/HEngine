#pragma once
#ifndef INPUT_FIELD_H
#define INPUT_FIELD_H

#include "UIElement.h"
struct ImFont;

namespace H {

    class InputField : public UIElement
    {
    public:
        static constexpr cmpId_type id = _cmp_UI_INPUTFIELD;
        InputField(bool _interactive, std::string text = "",
            std::string placeholder = H_PLACE_HOLDER_TXT_DEF,
            Color textColor = Color(H_UI_ELEM_DEF_COLOR),
            Color backgroundColor = Color(H_UI_ELEM_DEF_BG_COLOR), 
            ImFont* font = nullptr, Vector2F alignment = Vector2F(0.5f, 0.5f), Vector2F size = Vector2F(H_SIZE_WIND_WIDTH, H_SIZE_WIND_HEIGHT)
        ) : text(text), placeholder(placeholder), onSubmit(nullptr),
            textColor(textColor), backgroundColor(backgroundColor), font(font),alignment(alignment) {
            interactive = _interactive;
            sizeWind = size;
            buffer[0] = '\0';
            clicked = false;
        }
        ~InputField() {}

        std::string text = "";                                  // Texto actual en el input field
        std::string placeholder;              // Texto de marcador de posición (placeholder)

        bool clicked;
        std::function<void(const std::string&)> onSubmit = nullptr; // Callback cuando se confirma la entrada
        Color textColor; // Color del texto
        Color backgroundColor; // Color de fondo 
        ImFont* font = nullptr;    // Fuente personalizada
        Vector2F alignment; // Alineación del texto, 0.0 a 1.0 en X y Y
        char buffer[H_INPUT_FIELD_MAX_LEN];  // Buffer para almacenar el texto ingresado

        inline void AddOnSubmitCallback(std::function<void(const std::string&)> callback) {
            onSubmit = callback;
        }
    };
}
#endif // INPUT_FIELD_H
