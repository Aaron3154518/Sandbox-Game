#include "AssetManager.h"

constexpr auto ID_LEN = 4;

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
void TextData::setRectPos(Rect& r) const {
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
void AssetManager::initAssets(RendererPtr& r) {
	mRenderer.swap(r);
	resetDrawColor();
	resetRenderTarget();
	resetRenderBlendMode();
	for (const std::string& fName : queuedAssets) {
		loadAsset(fName);
	}
	queuedAssets.clear();
}
void AssetManager::initFonts() {
	for (const auto& pair : queuedFonts) {
		updateFont(pair.first, pair.second);
	}
	queuedFonts.clear();
}

SDL_Renderer* AssetManager::renderer() {
	return mRenderer.get();
}
SDL_Point AssetManager::getRenderSize() const {
	SDL_Point size;
	SDL_GetRendererOutputSize(mRenderer.get(), &size.x, &size.y);
	return size;
}
void AssetManager::setDrawColor(const SDL_Color& c) {
	SDL_SetRenderDrawColor(mRenderer.get(), c.r, c.g, c.b, c.a);
}
void AssetManager::resetDrawColor() {
	SDL_SetRenderDrawColor(mRenderer.get(), 255, 255, 255, 255);
}
void AssetManager::setRenderTarget(SDL_Texture* tex) {
	if (SDL_SetRenderTarget(mRenderer.get(), tex) != 0) {
		std::cerr << "Unable to set render target" << std::endl;
	}
}
void AssetManager::resetRenderTarget() {
	SDL_SetRenderTarget(mRenderer.get(), NULL);
	resetDrawColor();
}
void AssetManager::setRenderBlendMode(SDL_BlendMode mode) {
	SDL_SetRenderDrawBlendMode(mRenderer.get(), mode);
}
void AssetManager::resetRenderBlendMode() {
	SDL_SetRenderDrawBlendMode(mRenderer.get(), SDL_BLENDMODE_BLEND);
}

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
Font AssetManager::createFont(const FontData& data) {
	if (data.w == -1 && data.h == -1) {
		return makeFont(TTF_OpenFont(data.fileName.c_str(), 1));
	}
	int minSize = 1, maxSize = 10;
	int w, h;
	getFontSize(data.fileName, 1, &w, &h);
	// While too small
	while ((data.w != -1 && w < data.w) || (data.h != -1 && h < data.h)) {
		minSize = maxSize;
		maxSize *= 2;
		getFontSize(data.fileName, maxSize, &w, &h);
	}
	Font font = makeFont();
	while (true) {
		int size = (maxSize + minSize) / 2;
		getFontSize(data.fileName, size, &w, &h);
		// If too small
		if ((data.w != -1 && w <= data.w) || (data.h != -1 && h <= data.h)) {
			getFontSize(data.fileName, size + 1, &w, &h);
			if ((data.w == -1 || w > data.w) && (data.h == -1 || h > data.h)) {
				font = makeFont(TTF_OpenFont(data.fileName.c_str(), size + 1));
				break;
			} else {
				minSize = size + 1;
			}
			// If too big
		} else {
			getFontSize(data.fileName, size - 1, &w, &h);
			if ((data.w == -1 || w > data.w) && (data.h == -1 || h > data.h)) {
				maxSize = size - 1;
			} else {
				font = makeFont(TTF_OpenFont(data.fileName.c_str(), size - 1));
				break;
			}
		}
	}
	return font;
}
Texture AssetManager::createTexture(int w, int h, SDL_Color bkgrnd) {
	Texture tex = makeTexture(SDL_CreateTexture(mRenderer.get(),
		SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h));
	setRenderTarget(tex.get());
	SDL_SetTextureBlendMode(tex.get(), SDL_BLENDMODE_BLEND);
	rect(NULL, TRANSPARENT);
	resetRenderTarget();
	return tex;
}
Texture AssetManager::loadImage(std::string fileName) {
	struct stat buffer;
	if (stat(fileName.c_str(), &buffer) != 0) {
		std::cerr << "Could not find " << fileName << std::endl;
		return makeTexture();
	}
	return makeTexture(IMG_LoadTexture(mRenderer.get(), fileName.c_str()));
}

// Check if font/asset exists
bool AssetManager::assetExists(std::string id) const {
	return assets.find(id) != assets.end();
}
bool AssetManager::fontExists(std::string id) const {
	return fonts.find(id) != fonts.end();
}

// Load asset
bool AssetManager::loadAsset(std::string fileName) {
	if (SDL_WasInit(SDL_INIT_VIDEO) != 0) {
		assets[fileName] = loadImage(fileName);
#ifdef RENDER_DEBUG
		if (assets[fileName]) {
			std::cerr << "Successfully loaded asset: " << fileName << std::endl;
		}
#endif
		return true;
	} else {
		queuedAssets.push_back(fileName);
	}
	return false;
}

// Add font/asset
std::string AssetManager::addAsset(const SharedTexture& tex) {
	std::string id(ID_LEN, ' ');
	do {
		for (int i = 0; i < id.size(); i++) { id[i] = rand(); }
	} while (assets.find(id) != assets.end());
	updateAsset(id, tex);
	return id;
}
std::string AssetManager::addFont(const FontData& data) {
	std::string id(ID_LEN, ' ');
	do {
		for (int i = 0; i < id.size(); i++) { id[i] = rand(); }
	} while (fonts.find(id) != fonts.end());
	updateFont(id, data);
	return id;
}

// Update font/asset
void AssetManager::updateAsset(std::string id, const SharedTexture& tex) {
	assets[id] = tex;
}
void AssetManager::updateFont(std::string id, const FontData& data) {
	if (TTF_WasInit()) {
		fonts[id] = createFont(data);
	} else {
		fonts[id] = makeSharedFont();
		queuedFonts.push_back(std::make_pair(id, data));
	}
#ifdef RENDER_DEBUG
	if (fonts[id]) {
		std::cerr << "Successfully loaded font: " << id << std::endl;
	}
#endif
}

// Remove font/asset
void AssetManager::removeAsset(std::string id) {
	auto it = assets.find(id);
	if (it != assets.end()) { assets.erase(it); }
}
void AssetManager::removeFont(std::string id) {
	auto it = fonts.find(id);
	if (it != fonts.end()) { fonts.erase(it); }
}

// Get font/asset
SharedTexture AssetManager::getAsset(std::string id) {
	return !assetExists(id) && !loadAsset(id) ?
		makeSharedTexture() : assets[id];
}
SharedTexture AssetManager::getAsset(const TextureData& data) {
	return data.texture ? data.texture : getAsset(data.textureId);
}

SharedFont AssetManager::getFont(std::string id) const {
	auto it = fonts.find(id);
	return it == fonts.end() ? makeSharedFont() : it->second;
}
SharedFont AssetManager::getFont(const TextData& data) const {
	return data.font ? data.font : getFont(data.fontId);
}

// Functions to create a texture
TextureData AssetManager::renderText(const TextData& data) const {
	TextureData tData;
	SharedFont font = getFont(data);
	if (!font) {
		std::cerr << "Could not load font" << std::endl;
		return tData;
	}
	SDL_Surface* surface = TTF_RenderText_Blended(font.get(),
		data.text.c_str(), data.color);
	tData.texture = makeTexture(
		SDL_CreateTextureFromSurface(mRenderer.get(), surface));
	SDL_FreeSurface(surface);
	if (tData.texture) {
		tData.dest = Rect::getMinRect(tData.texture.get(), data.w, data.h);
		data.setRectPos(tData.dest);
	}
	return tData;
}

TextureData AssetManager::renderTextWrapped(const TextData& data,
	Uint32 bkgrnd) const {
	TextureData tData;
	SharedFont font = getFont(data);
	if (!font) {
		std::cerr << "Could not load font" << std::endl;
		return tData;
	}
	if (data.w == 0) {
		std::cerr << "Cannot render wrapped text with 0 length" << std::endl;
		return tData;
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
				if (ch == ' ') { addSpace = true; width += spaceW; }
				else { addSpace = false; }
			} else {
				if (width != 0) { lines.push_back(line_ss.str()); }
				line_ss.str("");
				line_ss << word;
				width = wordW;
				if (ch == ' ') { addSpace = true; width += spaceW; }
				else { addSpace = false; }
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

	// TODO: Switch to all SDL_Texture* (faster)?
	TextureData result;
	int lineH = TTF_FontHeight(font.get());
	int w = maxW, h = lineH * lines.size();
	SDL_Surface* surf = SDL_CreateRGBSurface(0, w, h, 32,
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
	tData.texture = makeTexture(
		SDL_CreateTextureFromSurface(mRenderer.get(), surf));
	SDL_FreeSurface(surf);
	tData.dest = Rect(0, 0, w, h);
	data.setRectPos(tData.dest);
	return tData;
}

// Functions to draw a texture
void AssetManager::drawTexture(const TextureData& data) {
	SDL_Texture* target = SDL_GetRenderTarget(mRenderer.get());
	Rect renderBounds;
	if (!target) {
		SDL_Point dim = getRenderSize();
		renderBounds.resize(dim.x, dim.y, false);
	} 	else { getTextureSize(target, &renderBounds.w, &renderBounds.h); }
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
	} else if (SDL_IntersectRect(&boundary, &renderBounds, &boundary)
		== SDL_FALSE) {
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

	SDL_RenderCopy(mRenderer.get(), tex.get(), &texRect, &destRect);
}
void AssetManager::drawText(const TextData& data) {
	drawTexture(renderText(data));
}
void AssetManager::drawTextWrapped(const TextData& data, Uint32 bkgrnd) {
	drawTexture(renderTextWrapped(data, bkgrnd));
}

Rect AssetManager::getMinRect(std::string id, int maxW, int maxH) {
	return Rect::getMinRect(getAsset(id).get(), maxW, maxH);
}

// Functions to draw a rectangle
void AssetManager::rect(Rect* r, const SDL_Color& color,
	SDL_BlendMode mode) {
	setRenderBlendMode(mode);
	setDrawColor(color);
	SDL_RenderFillRect(mRenderer.get(), r);
	resetDrawColor();
	resetRenderBlendMode();
}

void AssetManager::thickRect(Rect r, int thickness,
	AssetManager::BorderType border, const SDL_Color& color) {
	setDrawColor(color);
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
		SDL_RenderDrawRect(mRenderer.get(), &r);
	}
	resetDrawColor();
}

SharedTexture AssetManager::brightenTexture(SharedTexture src, Uint8 val) {
	int w, h;
	if (getTextureSize(src.get(), &w, &h)) {
		SDL_BlendMode mode;
		SDL_GetTextureBlendMode(src.get(), &mode);
		Uint8 r, g, b;
		SDL_GetTextureColorMod(src.get(), &r, &g, &b);

		SharedTexture dest = AssetManager::createTexture(w, h);
		setRenderTarget(dest.get());
		setRenderBlendMode(SDL_BLENDMODE_ADD);

		TextureData data;
		data.dest = Rect(0, 0, w, h);
		data.texture = src;
		drawTexture(data);

		setDrawColor(SDL_Color{ val,val,val,255 });
		SDL_RenderFillRect(mRenderer.get(), NULL);
		resetDrawColor();

		resetRenderBlendMode();
		resetRenderTarget();

		return dest;
	} else {
		std::cerr << "AssetManager::brightenTexture: "
			<< "could not query source texture" << std::endl;
	}
	return makeSharedTexture();
}

SharedTexture AssetManager::brightenTexture(std::string id, Uint8 val) {
	return brightenTexture(getAsset(id), val);
}

/*void AssetManager::drawProgressBar(Number amnt, Number cap, Rect& rect,
	SDL_Color color, SDL_Color bkgrnd) const {
	Number quot(cap == 0 ? 1 : amnt / cap);
	int w = fmax(0, fmin(1, quot.toDouble())) * rect.w;
	Game::get().setDrawColor(bkgrnd);
	SDL_RenderFillRect(mRenderer.get(), &rect);
	Rect r = Rect(rect.x, rect.y, w, rect.h);
	Game::get().setDrawColor(color);
	SDL_RenderFillRect(mRenderer.get(), &r);
	Game::get().setDrawColor(BLACK);
	SDL_RenderDrawRect(mRenderer.get(), &rect);
	Game::get().resetDrawColor();
}
void AssetManager::drawProgressBarLog(Number amnt, Number cap, Rect& rect,
	SDL_Color color, SDL_Color bkgrnd) const {
	Number logAmnt = amnt > 0 ? amnt.logBase(10) : 0;
	Number logCap = cap > 0 ? cap.logBase(10) : 0;
	drawProgressBar(logAmnt, logCap, rect, color, bkgrnd);
}*/
