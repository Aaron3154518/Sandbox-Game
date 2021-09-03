#include "Button.h"

Button::Button(std::string id, Uint8 hoverStr) : imgId(id) {
	data.texture = Window::Get().assets().brightenTexture(imgId, hoverStr);
}

bool Button::clicked(SDL_Point mouse) {
	return SDL_PointInRect(&mouse, &mRect);
}

void Button::draw(SDL_Point parentPos, Rect boundary) {
	SDL_Point mouse = mousePos();
	data.dest = mRect + parentPos;
	data.boundary = boundary;
	data.textureId = SDL_PointInRect(&mouse, &data.dest) ? "" : imgId;
	Window::Get().assets().drawTexture(data);
}