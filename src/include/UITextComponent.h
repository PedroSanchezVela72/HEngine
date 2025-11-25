#pragma once
#ifndef UI_TEXT_COMPONENT_H
#define UI_TEXT_COMPONENT_H

#include "UIElement.h"
struct ImFont;

namespace H {

	class UITextComponent : public UIElement
	{
	public:
		static constexpr cmpId_type id = _cmp_UI_TEXT;
		UITextComponent(std::string text, Color color, ImFont* font = nullptr, Vector2F alignment = Vector2F(0.5, 0.5), Vector2F size = Vector2F(H_SIZE_WIND_WIDTH, H_SIZE_WIND_HEIGHT)) :
			text(text), color(color), font(font), alignment(alignment) {
			sizeWind = size;
		}
		~UITextComponent() {}
		std::string text;
		Color color; // Color del texto (RGBA)
		ImFont* font = nullptr;    // Fuente personalizada
		Vector2F alignment; // Alineación del texto, 0.0 a 1.0 en X y Y
	};
}
#endif // UI_TEXT_COMPONENT_H