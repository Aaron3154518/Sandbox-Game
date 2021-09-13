#include "Button.h"

Button::Button(const std::string& id, Uint8 hoverStr) {
	img.setAssetId(id);
	hover.setTexture(Window::Get().assets().brightenTexture(id, hoverStr));
}

Button::Button(SharedTexture tex, Uint8 hoverStr) {
	img.setTexture(tex);
	hover.setTexture(Window::Get().assets().brightenTexture(tex, hoverStr));
}

bool Button::clicked(SDL_Point mouse) {
	return SDL_PointInRect(&mouse, &mRect);
}

void Button::draw(SDL_Point parentPos, Rect boundary) {
	SDL_Point mouse = mousePos();
	data.dest = mRect + parentPos;
	data.boundary = boundary;
	data.asset = SDL_PointInRect(&mouse, &data.dest) ? hover : img;
	Window::Get().assets().drawTexture(data);
}

void Button::setRect(const Rect& rect) {
	mRect = img.getMinRect(Window::Get().assets(), rect.w, rect.h);
	mRect.setCenter(rect.cX(), rect.cY());
}

void Button::setHoverImg(const std::string& id) {
	hover.setAssetId(id);
	hover.texture.reset();
}

void Button::setHoverImg(SharedTexture tex) {
	hover.setTexture(tex);
}