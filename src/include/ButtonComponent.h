#pragma once
#include <functional>

#ifndef BUTTON_COMPONENT_H
#define BUTTON_COMPONENT_H

#include "UIElement.h"
namespace H {

	class ButtonComponent : public UIElement
	{
	public:
		static constexpr cmpId_type id = _cmp_UI_BUTTON;
		ButtonComponent(bool _interactive, std::string label, Color color = Color::white(), Color colorHovered = Color(H_UI_WHITEHOVERED), Color colorActive = Color(H_UI_WHITEACTIVE), Vector2F size = Vector2F(H_SIZE_WIND_WIDTH, H_SIZE_WIND_HEIGHT)) :
			label(label), color(color), colorHovered(colorHovered),colorActive(colorActive) {
			interactive = _interactive;
			sizeWind = size;
		}
		~ButtonComponent() {}
		std::string label;
		std::function<void()> OnClick;
		Color color;
		Color colorHovered;
		Color colorActive;
		inline void AddCallback(std::function<void()> callback) {
			OnClick = std::move(callback);
		}
	};
}
#endif // BUTTON_COMPONENT_H
