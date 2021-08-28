#include "AssetManager.h"

//#define RENDER_DEBUG
//#define DEBUG_ALLOC

#ifdef DEBUG_ALLOC
int texCreateCnt = 0, texDestroyCnt = 0;
void destroyTexture(SDL_Texture* tex) {
	SDL_DestroyTexture(tex);
	std::cerr << "Destroyed Texture: " << texDestroyCnt++ << std::endl;
}
Texture makeTexture(SDL_Texture* tex) {
	std::cerr << "Created Texture: " << texCreateCnt++ << std::endl;
	return Texture(tex, destroyTexture);
}
SharedTexture makeSharedTexture(SDL_Texture* tex) {
	std::cerr << "Created Texture: " << texCreateCnt++ << std::endl;
	return SharedTexture(tex, destroyTexture);
}

int fontCreateCnt = 0, fontDestroyCnt = 0;
void destroyFont(TTF_Font* font) {
	TTF_CloseFont(font);
	std::cerr << "Destroyed Font: " << fontDestroyCnt++ << std::endl;
}
Font makeFont(TTF_Font* font) {
	std::cerr << "Created Font: " << fontCreateCnt++ << std::endl;
	return Font(font, destroyFont);
}
SharedFont makeSharedFont(TTF_Font* font) {
	std::cerr << "Created Font: " << fontCreateCnt++ << std::endl;
	return SharedFont(font, destroyFont);
}
#else
Texture makeTexture(SDL_Texture* tex) {
	return Texture(tex, SDL_DestroyTexture);
}
SharedTexture makeSharedTexture(SDL_Texture* tex) {
	return SharedTexture(tex, SDL_DestroyTexture);
}

Font makeFont(TTF_Font* font) {
	return Font(font, TTF_CloseFont);
}
SharedFont makeSharedFont(TTF_Font* font) {
	return SharedFont(font, TTF_CloseFont);
}
#endif

// TextData
void TextData::setRectPos(Rect& r) {
	switch (xMode) {
		case PosType::topleft: r.x = x; break;
		case PosType::center: r.setCX(x); break;
		case PosType::botright: r.setX2(x); break;
		default: break;
	}
	switch (yMode) {
		case PosType::topleft: r.y = y; break;
		case PosType::center: r.setCY(y); break;
		case PosType::botright: r.setY2(y); break;
		default: break;
	}
}

void TextData::getPosFromRect(const Rect& r) {
	switch (xMode) {
		case PosType::topleft: x = r.x; break;
		case PosType::center: x = r.cX(); break;
		case PosType::botright: x = r.x2(); break;
		default: break;
	}
	switch (yMode) {
		case PosType::topleft: y = r.y; break;
		case PosType::center: y = r.cY(); break;
		case PosType::botright: y = r.y2(); break;
		default: break;
	}
}

void TextData::constrainToRect(const Rect& r) {
	getPosFromRect(r);
	w = r.w; h = r.h;
}

// TextureData
const Rect& TextureData::NO_RECT() {
	static Rect _NO_RECT(0, 0, -1, -1);
	return _NO_RECT;
}

// AssetManager
bool AssetManager::getTextureSize(SDL_Texture* tex, int* w, int* h) {
	return SDL_QueryTexture(tex, NULL, NULL, w, h) == 0;
}
void AssetManager::getFontSize(std::string fileName, int size, int* w, int* h) {
	Font font = makeFont(TTF_OpenFont(fileName.c_str(), size));
	// Don't use ' '!
	TTF_SizeText(font.get(), "_", w, NULL);
	if (h) { *h = TTF_FontHeight(font.get()); }
}

// Create font/asset
Texture AssetManager::createTexture(int w, int h, SDL_Color bkgrnd) {
	Texture tex = makeTexture(SDL_CreateTexture(UI::renderer(),
		SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h));
	UI::setRenderTarget(tex.get());
	SDL_SetTextureBlendMode(tex.get(), SDL_BLENDMODE_BLEND);
	rect(NULL, TRANSPARENT);
	UI::resetRenderTarget();
	return tex;
}
SharedTexture AssetManager::loadImage(std::string fileName) {
	struct stat buffer;
	if (stat(fileName.c_str(), &buffer) != 0) {
		std::cerr << "Could not find " << fileName << std::endl;
		return makeSharedTexture();
	}
	return makeSharedTexture(IMG_LoadTexture(UI::renderer(), fileName.c_str()));
}
Font AssetManager::createFont(std::string fileName, int maxW, int maxH) {
	int minSize = 1, maxSize = 10;
	int w, h;
	getFontSize(fileName, 1, &w, &h);
	// While too small
	while ((maxW != -1 && w < maxW) || (maxH != -1 && h < maxH)) {
		minSize = maxSize;
		maxSize *= 2;
		getFontSize(fileName, maxSize, &w, &h);
	}
	Font font = makeFont();
	while (true) {
		int size = (maxSize + minSize) / 2;
		getFontSize(fileName, size, &w, &h);
		// If too small
		if ((maxW != -1 && w <= maxW) || (maxH != -1 && h <= maxH)) {
			getFontSize(fileName, size + 1, &w, &h);
			if ((maxW == -1 || w > maxW) && (maxH == -1 || h > maxH)) {
				font = makeFont(TTF_OpenFont(fileName.c_str(), size + 1));
				break;
			} else {
				minSize = size + 1;
			}
		// If too big
		} else {
			getFontSize(fileName, size - 1, &w, &h);
			if ((maxW == -1 || w > maxW) && (maxH == -1 || h > maxH)) {
				maxSize = size - 1;
			} else {
				font = makeFont(TTF_OpenFont(fileName.c_str(), size - 1));
				break;
			}
		}
	}
	return font;
}

// Check if font/asset exists
bool AssetManager::assetExists(std::string id) const {
	return assets.find(id) != assets.end();
}
bool AssetManager::fontExists(std::string id) const {
	return fonts.find(id) != fonts.end();
}

// Add texture/font
SharedTexture AssetManager::addAsset(std::string id, const SharedTexture& tex) {
	assets[id] = tex;
	return tex;
}
SharedFont AssetManager::addFont(std::string id, const SharedFont& font) {
	fonts[id] = font;
	return font;
}

// Load font/asset
SharedTexture AssetManager::loadAsset(std::string fileName) {
	SharedTexture tex = loadImage(fileName);
	if (tex) {
		assets[fileName] = tex;
#ifdef RENDER_DEBUG
		std::cerr << "Successfully loaded asset: " << fileName << std::endl;
#endif
	}
	return tex;
}

SharedFont AssetManager::loadFont(std::string id, std::string fileName,
	int maxW, int maxH) {
	SharedFont font = createFont(fileName, maxW, maxH);
	if (font) {
		fonts[id] = font;
#ifdef RENDER_DEBUG
		std::cerr << "Successfully loaded font: " << id << std::endl;
#endif
	}
	return font;
}

// Get font/asset
SharedTexture AssetManager::getAsset(std::string fileName) {
	auto it = assets.find(fileName);
	return it == assets.end() ? loadAsset(fileName) : it->second;
}

SharedTexture AssetManager::getAsset(const TextureData& td) {
	return td.texture ? td.texture : getAsset(td.textureId);
}

SharedFont AssetManager::getFont(std::string id) const {
	auto it = fonts.find(id);
	return it == fonts.end() ? makeSharedFont() : it->second;
}

SharedFont AssetManager::getFont(std::string id, std::string fileName,
	int maxW, int maxH) {
	auto it = fonts.find(id);
	return it == fonts.end() ? loadFont(id, fileName, maxW, maxH) : it->second;
}

SharedFont AssetManager::getFont(const TextData& td) const {
	return td.font ? td.font : getFont(td.fontId);
}

// Functions to create a texture
Texture AssetManager::renderText(TextData& data, Rect& rect) const {
	SharedFont font = getFont(data);
	if (!font) {
		std::cerr << "Could not load font" << std::endl;
		return makeTexture();
	}
	SDL_Surface* surface = TTF_RenderText_Blended(font.get(),
		data.text.c_str(), data.color);
	Texture tex = makeTexture(
		SDL_CreateTextureFromSurface(UI::renderer(), surface));
	SDL_FreeSurface(surface);
	if (tex) {
		rect = Rect::getMinRect(tex.get(), data.w, data.h);
		data.setRectPos(rect);
	}
	return tex;
}

Texture AssetManager::renderTextWrapped(TextData& data, Rect& rect,
	Uint32 bkgrnd) const {
	SharedFont font = getFont(data);
	if (!font) {
		std::cerr << "Could not load font" << std::endl;
		return makeTexture();
	}
	if (data.w == 0) {
		std::cerr << "Cannot render wrapped text with 0 length" << std::endl;
		return makeTexture();
	}

	std::vector<std::string> lines;
	std::stringstream line_ss, word_ss;
	int spaceW;
	TTF_SizeText(font.get(), " ", &spaceW, NULL);
	int width = 0;
	int maxW = data.w;
	bool addSpace = false;
	for (char ch : data.text + '\n') {
		if (ch == ' ' || ch == '\n' || ch == '\b') {
			std::string word = word_ss.str();
			word_ss.str("");
			int wordW;
			TTF_SizeText(font.get(), word.c_str(), &wordW, NULL);
			if (width + wordW < data.w) {
				if (addSpace) { line_ss << ' '; }
				line_ss << word;
				width += wordW;
				if (ch == ' ') { addSpace = true; width += spaceW; } else { addSpace = false; }
			} else {
				if (width != 0) { lines.push_back(line_ss.str()); }
				line_ss.str("");
				line_ss << word;
				width = wordW;
				if (ch == ' ') { addSpace = true; width += spaceW; } else { addSpace = false; }
				if (wordW > maxW) { maxW = wordW; }
			}
			if (ch == '\n') {
				lines.push_back(line_ss.str());
				line_ss.str("");
				width = 0;
				addSpace = false;
			}
		} else {
			word_ss << ch;
		}
	}

	// TODO: Switch to all SDL_Texture* (faster)
	int lineH;
	TTF_SizeText(font.get(), "|", NULL, &lineH);
	rect = Rect(0, 0, maxW, (int)(lineH * lines.size()));
	data.setRectPos(rect);
	SDL_Surface* surf = SDL_CreateRGBSurface(0, rect.w, rect.h, 32,
		rmask, gmask, bmask, amask);
	if (bkgrnd != -1) { SDL_FillRect(surf, NULL, bkgrnd); }
	int x = maxW / 2, y = lineH / 2;
	for (std::string line : lines) {
		if (line == "") { y += lineH; continue; }
		SDL_Surface* lineSurf = TTF_RenderText_Blended(font.get(),
			line.c_str(), data.color);
		if (lineSurf != nullptr) {
			Rect lineRect = Rect(0, 0, lineSurf->w, lineSurf->h);
			lineRect.setCenter(x, y);
			SDL_BlitSurface(lineSurf, NULL, surf, &lineRect);
			SDL_FreeSurface(lineSurf);
		} else {
			std::cerr << "line '" << line << "' produced a null surface"
				<< std::endl;
		}
		y += lineH;
	}
	lines.clear();
	Texture tex = makeTexture(
		SDL_CreateTextureFromSurface(UI::renderer(), surf));
	SDL_FreeSurface(surf);
	return tex;
}

// Functions to draw a texture
void AssetManager::drawTexture(const TextureData& data) {
	SDL_Texture* target = SDL_GetRenderTarget(UI::renderer());
	Rect renderBounds;
	if (!target) { renderBounds.resize(UI::width(), UI::height(), false); }
	else { getTextureSize(target, &renderBounds.w, &renderBounds.h); }
	// Make sure we are actually drawing something
	if (data.dest.empty() || data.dest.invalid() ||
		data.area.empty() || data.boundary.empty()) {
#ifdef RENDER_DEBUG
		std::cerr << "Empty drawing or boundary rect" << std::endl;
#endif
		return;
	}
	// Get and check the texture to draw
	SharedTexture tex = getAsset(data);
	if (!tex) {
#ifdef RENDER_DEBUG
		std::cerr << "Invalid Texture" << std::endl;
		return;
#endif
	}
	// Get the boundary rect
	Rect boundary = data.boundary;
	if (boundary.invalid()) {
		boundary = renderBounds;
	} else if (SDL_IntersectRect(&boundary, &renderBounds, &boundary) == SDL_FALSE) {
#ifdef RENDER_DEBUG
		std::cerr << "Boundary rect " << boundary
			<< " was out side the screen: " << renderBounds << std::endl;
#endif
		return;
	}

	Rect destRect = data.dest;
	// Get fraction of item to draw
	double leftFrac = fmax(boundary.x - destRect.x, 0) / destRect.w;
	double topFrac = fmax(boundary.y - destRect.y, 0) / destRect.h;
	double rightFrac = fmax(destRect.x2() - boundary.x2(), 0) / destRect.w;
	double botFrac = fmax(destRect.y2() - boundary.y2(), 0) / destRect.h;
	// Make sure the rect is actually in the boundary
	if (leftFrac + rightFrac >= 1 || topFrac + botFrac >= 1) {
#ifdef RENDER_DEBUG
		std::cerr << "Rect " << destRect << " was out side the boundary " <<
			boundary << std::endl;
#endif
		return;
	}

	destRect = Rect(destRect.x + destRect.w * leftFrac,
		destRect.y + destRect.h * topFrac,
		destRect.w * (1. - leftFrac - rightFrac),
		destRect.h * (1. - topFrac - botFrac));
	int w, h;
	if (!getTextureSize(tex.get(), &w, &h)) {
		std::cerr << "Unable to query texture size: "
			<< SDL_GetError() << std::endl;
		return;
	}
	Rect areaRect = data.area.invalid() ? Rect(0, 0, w, h) : data.area;
	Rect texRect = Rect(areaRect.x + areaRect.w * leftFrac,
		areaRect.y + areaRect.h * topFrac,
		areaRect.w * (1. - leftFrac - rightFrac),
		areaRect.h * (1. - topFrac - botFrac));
	// Make sure at least one pixel will be drawn
	if (texRect.empty()) {
#ifdef RENDER_DEBUG
		std::cerr << "Can't draw from " << texRect << " to " << destRect
			<< std::endl;
#endif
		return;
	}

	SDL_RenderCopy(UI::renderer(), tex.get(), &texRect, &destRect);
}

void AssetManager::drawText(TextData& data, const Rect& boundary) {
	TextureData tData;
	tData.texture = renderText(data, tData.dest);
	tData.boundary = boundary;
	if (tData.texture) { drawTexture(tData); }
}

void AssetManager::drawTextWrapped(TextData& data, const Rect& boundary,
	Uint32 bkgrnd) {
	TextureData tData;
	tData.texture = renderTextWrapped(data, tData.dest, bkgrnd);
	tData.boundary = boundary;
	if (tData.texture) { drawTexture(tData); }
}

// Functions to draw a rectangle
void AssetManager::rect(Rect* r, const SDL_Color& color,
	SDL_BlendMode mode) {
	UI::setRenderBlendMode(mode);
	UI::setDrawColor(color);
	SDL_RenderFillRect(UI::renderer(), r);
	UI::resetDrawColor();
	UI::resetRenderBlendMode();
}

void AssetManager::thickRect(Rect r, int thickness,
	AssetManager::BorderType border, const SDL_Color& color) {
	UI::setDrawColor(color);
	int lb, ub;
	switch (border) {
		case BorderType::inside:
			lb = -thickness; ub = 0; break;
		case BorderType::outside:
			lb = 0; ub = thickness; break;
		case BorderType::middle:
			lb = -(int)(thickness / 2);
			ub = (int)(((double)thickness / 2) + .5);
			break;
		default:
			lb = ub = 0; break;
	}
	// Set inital rect
	r.x -= lb; r.y -= lb; r.w += lb * 2; r.h += lb * 2;
	while (lb++ < ub) {
		// Expand rect each time
		r.x--; r.y--; r.w += 2; r.h += 2;
		if (r.w <= 0 || r.h <= 0) { break; }
		SDL_RenderDrawRect(UI::renderer(), &r);
	}
	UI::resetDrawColor();
}


Rect AssetManager::getMinRect(std::string fileName, int maxW, int maxH) {
	return Rect::getMinRect(getAsset(fileName).get(), maxW, maxH);
}

SharedTexture AssetManager::brightenTexture(SharedTexture src, Uint8 val) {
	TextureData data;
	data.texture = src;
	int w, h;
	if (getTextureSize(src.get(), &w, &h)) {
		data.dest = Rect(0, 0, w, h);
		SharedTexture dest = AssetManager::createTexture(w, h);

		SDL_BlendMode mode;
		SDL_GetTextureBlendMode(src.get(), &mode);
		Uint8 r, g, b;
		SDL_GetTextureColorMod(src.get(), &r, &g, &b);

		UI::setRenderTarget(dest.get());
		UI::setRenderBlendMode(SDL_BLENDMODE_ADD);

		UI::assets().drawTexture(data);
		UI::setDrawColor(SDL_Color{ val,val,val,255 });
		SDL_RenderFillRect(UI::renderer(), NULL);
		UI::resetDrawColor();

		UI::resetRenderBlendMode();
		UI::resetRenderTarget();

		return dest;
	} else {
		std::cerr << "AssetManager::brightenTexture: "
			<< "could not query source texture" << std::endl;
	}
	return makeSharedTexture();
}

/*void AssetManager::drawProgressBar(Number amnt, Number cap, Rect& rect,
	SDL_Color color, SDL_Color bkgrnd) const {
	Number quot(cap == 0 ? 1 : amnt / cap);
	int w = fmax(0, fmin(1, quot.toDouble())) * rect.w;
	Game::get().setDrawColor(bkgrnd);
	SDL_RenderFillRect(UI::renderer(), &rect);
	Rect r = Rect(rect.x, rect.y, w, rect.h);
	Game::get().setDrawColor(color);
	SDL_RenderFillRect(UI::renderer(), &r);
	Game::get().setDrawColor(BLACK);
	SDL_RenderDrawRect(UI::renderer(), &rect);
	Game::get().resetDrawColor();
}
void AssetManager::drawProgressBarLog(Number amnt, Number cap, Rect& rect,
	SDL_Color color, SDL_Color bkgrnd) const {
	Number logAmnt = amnt > 0 ? amnt.logBase(10) : 0;
	Number logCap = cap > 0 ? cap.logBase(10) : 0;
	drawProgressBar(logAmnt, logCap, rect, color, bkgrnd);
}*/