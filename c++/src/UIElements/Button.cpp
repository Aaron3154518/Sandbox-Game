#include "Button.h"

Button::Button(std::string id, Uint8 hoverStr) {
	data.textureId = id;
	data.texture = Window::Get().assets().brightenTexture(id, hoverStr);
}

bool Button::clicked(SDL_Point mouse) {
	return SDL_PointInRect(&mouse, &mRect);
}

void Button::draw(SDL_Point parentPos, Rect boundary) {
	SDL_Point mouse = mousePos();
	data.dest = mRect + parentPos;
	data.boundary = boundary;
	data.useTexture = SDL_PointInRect(&mouse, &data.dest);
	Window::Get().assets().drawTexture(data);
}

void Button::setRect(const Rect& rect) {
	mRect = Rect::getMinRect(data.texture.get(), rect.w, rect.h);
	mRect.setCenter(rect.cX(), rect.cY());
}
