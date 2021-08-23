#include "Button.h"

Button::Button(std::string id, Uint8 hoverStr) : imgId(id), hoverId(id + "hover") {
	AssetManager& assets = UI::assets();
	int i = 0;
	std::string tmp = hoverId;
	while (assets.assetExists(tmp)) { tmp = hoverId + std::to_string(i++); }
	hoverId = tmp;

	SharedTexture imgTex = assets.getAsset(imgId);
	assets.addAsset(hoverId, AssetManager::brightenTexture(imgTex, hoverStr));
}

bool Button::clicked(SDL_Point mouse) {
	return SDL_PointInRect(&mouse, &mRect);
}

void Button::draw(SDL_Point parentPos, Rect boundary) {
	SDL_Point mouse = UI::mouse();
	data.dest = mRect + parentPos;
	data.boundary = boundary;
	data.textureId = SDL_PointInRect(&mouse, &data.dest) ? hoverId : imgId;
	UI::assets().drawTexture(data);
}