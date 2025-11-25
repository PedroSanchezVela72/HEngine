#pragma once
#ifndef UI_IMAGE_COMPONENT_H 
#define UI_IMAGE_COMPONENT_H

#include "UIElement.h"

typedef unsigned long long  ImU64;
typedef ImU64 ImTextureID;

namespace H {

	class UIImageComponent : public UIElement
	{
	public:
		static constexpr cmpId_type id = _cmp_UI_IMAGE;
		UIImageComponent(std::string texture, Vector2F size = Vector2F(H_SIZE_WIND_WIDTH, H_SIZE_WIND_HEIGHT), Color color = Color::white(), bool tiled = false) :
			texture(texture), color(color), tiled(tiled){
			sizeWind = size;
		}
		~UIImageComponent() {}
		std::string texture;
		Color color; // Color con el que se mezcla la imagen
		bool tiled = false;   // Indica si la imagen debe repetirse en mosaico
		ImTextureID textureID;
	};
}
#endif // UI_IMAGE_COMPONENT_H

