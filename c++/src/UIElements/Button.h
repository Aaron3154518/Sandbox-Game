#ifndef BUTTON_H
#define BUTTON_H

#include <SDL.h>

#include "../Definitions.h"
#include "UIElement.h"
#include "../UIs/UI.h"
#include "../Utils/Rect.h"
#include "../Utils/AssetManager.h"

class Button : public UIElement {
public:
	Button() = default;
	Button(std::string id, Uint8 hoverStr = 120);
	~Button() = default;

	bool clicked(SDL_Point mouse);
	void draw(SDL_Point parentPos, Rect boundary = TextureData::NO_RECT());

private:
	std::string imgId, hoverId;

	TextureData data;
};

#endif