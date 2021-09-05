#include "Button.h"

Button::Button(const std::string& id, Uint8 hoverStr) {
	normal.textureId = id; normal.useTexture = false;
	hover.texture = Window::Get().assets().brightenTexture(id, hoverStr);
	hover.useTexture = true;
}

Button::Button(SharedTexture tex, Uint8 hoverStr) {
	normal.texture = tex; normal.useTexture = true;
	hover.texture = Window::Get().assets().brightenTexture(tex, hoverStr);
	hover.useTexture = true;
}

bool Button::clicked(SDL_Point mouse) {
	return SDL_PointInRect(&mouse, &mRect);
}

void Button::draw(SDL_Point parentPos, Rect boundary) {
	SDL_Point mouse = mousePos();
	data.dest = mRect + parentPos;
	data.boundary = boundary;
	State& state = SDL_PointInRect(&mouse, &data.dest) ? hover : normal;
	data.textureId = state.textureId;
	data.texture = state.texture;
	data.useTexture = state.useTexture;
	Window::Get().assets().drawTexture(data);
}

void Button::setRect(const Rect& rect) {
	if (normal.useTexture && normal.texture) {
		mRect = Rect::getMinRect(normal.texture.get(), rect.w, rect.h);
		mRect.setCenter(rect.cX(), rect.cY());
	} else if (!normal.useTexture && !normal.textureId.empty()) {
		mRect = Window::Get().assets().getMinRect(normal.textureId,
			rect.w, rect.h);
		mRect.setCenter(rect.cX(), rect.cY());
	} else { mRect = rect; }
}

void Button::setHoverImg(const std::string& id) {
	hover.textureId = id; hover.texture.reset();
	hover.useTexture = false;
}

void Button::setHoverImg(SharedTexture tex) {
	hover.texture = tex;
	hover.useTexture = true;
}