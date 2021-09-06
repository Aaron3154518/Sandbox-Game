#ifndef BUTTON_H
#define BUTTON_H

#include <SDL.h>

#include "UIElement.h"
#include "../Definitions.h"
#include "../Utils/AssetManager.h"
#include "../Utils/Rect.h"
#include "../Utils/Utils.h"
#include "../Window.h"

class Button : public UIElement {
public:
	Button() = default;
	Button(const std::string& id, Uint8 hoverStr = 120);
	Button(SharedTexture tex, Uint8 hoverStr = 120);
	~Button() = default;

	bool clicked(SDL_Point mouse);
	void draw(SDL_Point parentPos = SDL_Point{ 0,0 },
		Rect boundary = TextureData::NO_RECT());

	void setRect(const Rect& rect);
	void setHoverImg(const std::string& id);
	void setHoverImg(SharedTexture tex);

private:
	Asset normal, hover;
	TextureData data;
};

#endif