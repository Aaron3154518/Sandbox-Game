#include "Button.h"

Button::Button(const std::string& id, Uint8 hoverStr) {
	normal.assetId = id; normal.useTexture = false;
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
	Asset& asset = SDL_PointInRect(&mouse, &data.dest) ? hover : normal;
	data.textureId = asset.assetId;
	data.texture = asset.texture;
	data.useTexture = asset.useTexture;
	Window::Get().assets().drawTexture(data);
}

void Button::setRect(const Rect& rect) {
	if (normal.useTexture && normal.texture) {
		mRect = Rect::getMinRect(normal.texture.get(), rect.w, rect.h);
		mRect.setCenter(rect.cX(), rect.cY());
	} else if (!normal.useTexture && !normal.assetId.empty()) {
		mRect = Window::Get().assets().getMinRect(normal.assetId,
			rect.w, rect.h);
		mRect.setCenter(rect.cX(), rect.cY());
	} else { mRect = rect; }
}

void Button::setHoverImg(const std::string& id) {
	hover.assetId = id; hover.texture.reset();
	hover.useTexture = false;
}

void Button::setHoverImg(SharedTexture tex) {
	hover.texture = tex;
	hover.useTexture = true;
}