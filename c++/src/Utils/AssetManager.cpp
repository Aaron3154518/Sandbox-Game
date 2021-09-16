#include "AssetManager.h"

constexpr auto ID_LEN = 4;

//#define RENDER_DEBUG
//#define DEBUG_ALLOC

#ifdef DEBUG_ALLOC
// Unable to track pointers that start null but are reset() to a valid pointer
int surfCreateCnt = 0, surfDestroyCnt = 0;
void destroySurface(SDL_Surface* surf) {
	surfDestroyCnt++;
	SDL_FreeSurface(surf);
}
Surface makeSurface(SDL_Surface* surf) {
	if (surf) { surfCreateCnt++; }
	return Surface(surf, destroySurface);
}
SharedSurface makeSharedSurface(SDL_Surface* surf) {
	if (surf) { surfCreateCnt++; }
	return SharedSurface(surf, destroySurface);
}

int texCreateCnt = 0, texDestroyCnt = 0;
void destroyTexture(SDL_Texture* tex) {
	texDestroyCnt++;
	SDL_DestroyTexture(tex);
}
Texture makeTexture(SDL_Texture* tex) {
	if (tex) { texCreateCnt++; }
	return Texture(tex, destroyTexture);
}
SharedTexture makeSharedTexture(SDL_Texture* tex) {
	if (tex) { texCreateCnt++; }
	return SharedTexture(tex, destroyTexture);
}

int fontCreateCnt = 0, fontDestroyCnt = 0;
void destroyFont(TTF_Font* font) {
	fontDestroyCnt++;
	TTF_CloseFont(font);
#ifdef DEBUG_FONTS
	std::cerr << "-F " << (bool)font << " " << font << std::endl;
#endif
}
Font makeFont(TTF_Font* font) {
	if (font) { fontCreateCnt++; }
#ifdef DEBUG_FONTS
	std::cerr << "+F " << (bool)font << " " << font << std::endl;
#endif
	return Font(font, destroyFont);
}
SharedFont makeSharedFont(TTF_Font* font) {
	if (font) { fontCreateCnt++; }
#ifdef DEBUG_FONTS
	std::cerr << "+F " << (bool)font << " " << font << std::endl;
#endif
	return SharedFont(font, destroyFont);
}

void printMemoryStats() {
	std::cerr << "Allocation and Deallocation Statistics:" << std::endl;

	std::cerr << "Surfaces:" << std::endl;
	std::cerr << "\tCreated: " << surfCreateCnt << ", Destroyed: "
		<< surfDestroyCnt << std::endl;

	std::cerr << "Textures:" << std::endl;
	std::cerr << "\tCreated: " << texCreateCnt << ", Destroyed: "
		<< texDestroyCnt << std::endl;

	std::cerr << "Fonts:" << std::endl;
	std::cerr << "\tCreated: " << fontCreateCnt << ", Destroyed: "
		<< fontDestroyCnt << std::endl;
}
#else
Surface makeSurface(SDL_Surface* surf) {
	return Surface(surf, SDL_FreeSurface);
}
SharedSurface makeSharedSurface(SDL_Surface* surf) {
	return SharedSurface(surf, SDL_FreeSurface);
}

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

void printMemoryStats() {}
#endif

bool operator==(const SDL_Color& lhs, const SDL_Color& rhs) {
	return lhs.a == rhs.a && lhs.r == rhs.r
		&& lhs.g == rhs.g && lhs.b == rhs.b;
}
bool operator!=(const SDL_Color& lhs, const SDL_Color& rhs) {
	return !(lhs == rhs);
}


// TextData
void TextData::setFontId(const std::string& id) {
	fontId = id;
	useFont = false;
}

void TextData::setFont(const SharedFont& newFont) {
	font = newFont;
	useFont = true;
}

SharedFont TextData::getFont(AssetManager& assets) const {
	return useFont ? font : assets.getFont(fontId);
}

// Asset
void Asset::setAssetId(const std::string& id) {
	assetId = id;
	useTexture = false;
}

void Asset::setTexture(const SharedTexture& tex) {
	texture = tex;
	useTexture = true;
}

SharedTexture Asset::getTexture(AssetManager& assets) const {
	return useTexture ? texture : assets.getAsset(assetId);
}

Rect Asset::getMinRect(AssetManager& assets, int maxW, int maxH) const {
	return Rect::getMinRect(getTexture(assets).get(), maxW, maxH);
}

// RenderData
const Rect& RenderData::NO_RECT() {
	const static Rect _NO_RECT(0, 0, -1, -1);
	return _NO_RECT;
}

void RenderData::fitToAsset(AssetManager& assets) {
	dest = asset.getMinRect(assets, dest.w, dest.h);
}

void RenderData::fitToAsset(AssetManager& assets, int maxW, int maxH) {
	dest = asset.getMinRect(assets, maxW, maxH);
}

void RenderData::addBoundary(const Rect& bounds) {
	SDL_IntersectRect(&boundary, &bounds, &boundary);
}

// RectData
RectData::RectData(const ShapeData& data) {
	color = data.color;
	blendMode = data.blendMode;
	boundary = data.boundary;
}

RectData& RectData::set() {
	r1 = r2 = Rect(0, 0, 0, 0);
	return *this;
}

RectData& RectData::set(const Rect& r) {
	r1 = Rect(0, 0, 0, 0);
	r2 = r; r2.normalize();
	return *this;
}

RectData& RectData::set(Rect r, int thickness, bool center) {
	r.normalize();
	r1 = r2 = r;
	if (center) {
		thickness = std::abs(thickness);
		r1.resize(r1.w - thickness, r1.h - thickness, true);
		r2.resize(r1.w + thickness, r1.h + thickness, true);
	} else {
		int dw = 2 * thickness;
		if (dw > 0) { r2.resize(r2.w + dw, r2.h + dw, true); }
		else { r1.resize(r1.w + dw, r1.h + dw, true); }
	}
	if (r1.invalid()) { r1 = Rect(0, 0, 0, 0); }
	return *this;
}

void RectData::render(AssetManager& assets) {
	assets.setBlendMode(blendMode);
	assets.setDrawColor(color);
	// Start with bounds as the target dimensions
	Rect bounds; bounds.resize(assets.getTargetSize(), false);
	// Make sure part of the boundary is inside the target
	// bounds is now intersected with boundary
	if (!boundary.invalid()
		&& SDL_IntersectRect(&boundary, &bounds, &bounds) == SDL_FALSE) {
		return;
	}
	// Fill entire render target
	if (r2.empty()) {
		SDL_RenderFillRect(assets.renderer(), &bounds);
	// Intersect r2 and bounds
	} else if (SDL_IntersectRect(&r2, &bounds, &bounds) == SDL_TRUE) {
		// Start at r1
		Rect r = r1;
		// Fill r2 if r is empty or not within bounds
		// Intersect r with bounds
		if (r.empty() || SDL_IntersectRect(&bounds, &r, &r) == SDL_FALSE) {
			SDL_RenderFillRect(assets.renderer(), &bounds);
		// Fill bounds except for r
		} else {
			// bounds is inclusive so draw once more when r = bounds
			do {
				// Expand rect
				if (r.x > bounds.x) { r.x--; r.w++; }
				if (r.y > bounds.y) { r.y--; r.h++; }
				if (r.x < bounds.x2()) { r.w++; }
				if (r.y < bounds.y2()) { r.h++; }
				SDL_RenderDrawRect(assets.renderer(), &r);
			} while (r != bounds);
		}
	}
	assets.resetDrawColor();
	assets.resetBlendMode();
}

// CircleData
CircleData::CircleData(const ShapeData& data) {
	color = data.color;
	blendMode = data.blendMode;
	boundary = data.boundary;
}

CircleData& CircleData::set(SDL_Point _c, int r) {
	c = _c;
	r1 = 0;
	r2 = std::abs(r);
	return *this;
}

CircleData& CircleData::set(SDL_Point _c, int r, int thickness, bool center) {
	c = _c;
	r = std::abs(r);
	if (center) { r2 = r + std::abs(thickness) / 2; }
	else { r2 = thickness < 0 ? r : r + thickness; }
	r1 = std::max(r2 - std::abs(thickness), 0);
	return *this;
}

void CircleData::render(AssetManager& assets) {
	assets.setBlendMode(blendMode);
	assets.setDrawColor(color);
	// Get boundary rect
	Rect bounds; bounds.resize(assets.getTargetSize(), false);
	if (!boundary.invalid()
		&& SDL_IntersectRect(&boundary, &bounds, &bounds) == SDL_FALSE) {
		return;
	}
	// Circle
	int dx = -1;
	while (++dx < r2) {
		int dy1 = dx >= r1 ? 0 : (int)(std::sqrt(r1 * r1 - dx * dx) + .5);
		int dy2 = (int)(std::sqrt(r2 * r2 - dx * dx) + .5);
		// Iterate through dx, -dx
		do {
			int x = c.x + dx;
			// Make sure x is in bounds
			if (x >= bounds.x && x <= bounds.x2()) {
				// Iterate through [dy1, dy2], [-dy2, -dy1]
				do {
					int y1 = std::max(c.y + dy1, bounds.y);
					int y2 = std::min(c.y + dy2, bounds.y2());
					// Make sure at least one y is in bounds
					if (y1 <= bounds.y2() && y2 >= bounds.y) {
						SDL_RenderDrawLine(assets.renderer(), x, y1, x, y2);
					}
					int tmp = -dy1; dy1 = -dy2; dy2 = tmp;
				} while (dy1 < 0);
			}
		} while ((dx *= -1) < 0);
	}

	assets.resetDrawColor();
	assets.resetBlendMode();
}

// AssetManager
void AssetManager::initAssets(SDL_Window* w) {
	if (!w) {
		std::cerr << "Invalid SDL_Window for initialization" << std::endl;
		return;
	}
	mRenderer.reset(SDL_CreateRenderer(w, -1, 0));
	resetDrawColor();
	resetRenderTarget();
	resetBlendMode();
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
void AssetManager::quit() {
	assets.clear();
	fonts.clear();
	mRenderer.reset();

	printMemoryStats();
}

SDL_Renderer* AssetManager::renderer() {
	return mRenderer.get();
}
SDL_Point AssetManager::getTargetSize() const {
	SDL_Point size;
	getTargetSize(&size.x, &size.y);
	return size;
}
void AssetManager::getTargetSize(int* w, int* h) const {
	SDL_GetRendererOutputSize(mRenderer.get(), w, h);
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
void AssetManager::setBlendMode(SDL_BlendMode mode) {
	SDL_SetRenderDrawBlendMode(mRenderer.get(), mode);
}
void AssetManager::resetBlendMode() {
	SDL_SetRenderDrawBlendMode(mRenderer.get(), SDL_BLENDMODE_BLEND);
}

bool AssetManager::getTextureSize(SDL_Texture* tex, int* w, int* h) {
	return SDL_QueryTexture(tex, NULL, NULL, w, h) == 0;
}
void AssetManager::getFontSize(std::string fileName, int size, int* w, int* h) {
	Font font = makeFont(TTF_OpenFont(fileName.c_str(), size));
	TTF_SizeText(font.get(), "_", w, NULL);
	if (h) { *h = TTF_FontHeight(font.get()); }
}
void AssetManager::getTextSize(std::string fileName, int size,
	std::string sampleText, int* w, int* h) {
	if (sampleText.empty()) { getFontSize(fileName, size, w, h); } 	else {
		Font font = makeFont(TTF_OpenFont(fileName.c_str(), size));
		TTF_SizeText(font.get(), sampleText.c_str(), w, h);
	}
}

// Create font/asset
Font AssetManager::createFont(const FontData& data) {
	Font font = makeFont(TTF_OpenFont(data.fileName.c_str(), 1));
	if (data.w <= 0 && data.h <= 0) {
		return font;
	}
	// Min is always too small or just right, max is too big
	int minSize = 1, maxSize = 1;
	int w, h;
	getTextSize(data.fileName, minSize, data.sampleText, &w, &h);
	// While too small
	while ((data.w <= 0 || w <= data.w) && (data.h <= 0 || h <= data.h)) {
		minSize = maxSize;
		maxSize *= 2;
		getTextSize(data.fileName, maxSize, data.sampleText, &w, &h);
	}
	// Terminate when maxSize (too big) is right after minSize (too small)
	while (maxSize - minSize > 1) {
		int size = (maxSize + minSize) / 2;
		getTextSize(data.fileName, size, data.sampleText, &w, &h);
		// Too big
		if ((data.w > 0 && w > data.w) || (data.h > 0 && h > data.h)) {
			maxSize = size;
		// Too small or just right
		} else {
			minSize = size;
		}
	}
	getTextSize(data.fileName, minSize, data.sampleText, &w, &h);
	return makeFont(TTF_OpenFont(data.fileName.c_str(), minSize));
}
Texture AssetManager::createTexture(int w, int h, SDL_Color bkgrnd) {
	Texture tex = makeTexture(SDL_CreateTexture(mRenderer.get(),
		SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h));
	setRenderTarget(tex.get());
	SDL_SetTextureBlendMode(tex.get(), SDL_BLENDMODE_BLEND);
	RectData({ TRANSPARENT,SDL_BLENDMODE_BLEND }).set(RenderData::NO_RECT()).
		render(*this);
	resetRenderTarget();
	return tex;
}
Texture AssetManager::loadImage(std::string fileName) {
	struct stat buffer;
	if (stat(fileName.c_str(), &buffer) != 0) {
		std::cerr << "Could not find image file: " << fileName << std::endl;
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
	std::string id;
	do {
		id = randomString(ID_LEN);
	} while (assets.find(id) != assets.end());
	updateAsset(id, tex);
	return id;
}
std::string AssetManager::addFont(const FontData& data) {
	std::string id;
	do {
		id = randomString(ID_LEN);
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

SharedFont AssetManager::getFont(std::string id) {
	auto it = fonts.find(id);
	return it == fonts.end() ? makeSharedFont() : it->second;
}

std::vector<std::string> AssetManager::splitText(const std::string& text,
	SharedFont font, int maxW) {
	std::vector<std::string> lines;
	if (!font) {
		return lines;
	}
	int maxChW = 0;
	TTF_SizeText(font.get(), "_", &maxChW, NULL);
	if (maxW < maxChW) {
		std::cerr << "splitText(): Specified width is too small. Is: "
			<< maxW << " Should be at least: " << maxChW << std::endl;
		return lines;
	}

	std::stringstream line_ss, word_ss;
	int spaceW, dashW;
	TTF_SizeText(font.get(), " ", &spaceW, NULL);
	TTF_SizeText(font.get(), "-", &dashW, NULL);
	int lineW = 0;
	bool addSpace = false;
	// Go through each character in the input
	for (char ch : text + '\n') {
		// Finished this word
		if (ch == ' ' || ch == '\n' || ch == '\b') {
			std::string word = word_ss.str();
			word_ss.str("");
			int wordW = 0;
			TTF_SizeText(font.get(), word.c_str(), &wordW, NULL);
			// This word does not overflow the line, add it
			if (lineW + wordW <= maxW) {
				// Add space if necessary
				if (addSpace) { line_ss << ' '; }
				line_ss << word;
				lineW += wordW;
				if (ch == ' ') { addSpace = true; lineW += spaceW; } 				else { addSpace = false; }
			// This word overflows the line, check solutions
			} else {
				int i = 0;
				// If the word will not fit on its own line, break it up
				while (wordW > maxW && i < word.size() - 1) {
					int lb = i;
					const char* begin = word.data() + lb;
					char save = word[lb + 1];
					word[lb + 1] = '\0';
					int subW = 0;
					TTF_SizeText(font.get(), begin, &subW, NULL);
					while (lineW + subW + dashW <= maxW && i < word.size() - 2) {
						++i;
						word[i] = save;
						save = word[i + 1];
						word[i + 1] = '\0';
						TTF_SizeText(font.get(), begin, &subW, NULL);
					}
					// Get remaining word width
					word[i + 1] = save;
					TTF_SizeText(font.get(), word.data() + i,
						&wordW, NULL);
					// Make sure we actually fit some characters
					// from the word onto the line (not just the '-')
					if (i > lb) {
						// If adding to an existing line, check to add space
						if (lineW > 0 && addSpace) { line_ss << " "; }
						line_ss << word.substr(lb, i - lb) << "-";
					}
					lines.push_back(line_ss.str());
					line_ss.str("");
					lineW = 0;
				}
				// Word will fit on its own line
				if (i == 0) {
					lines.push_back(line_ss.str());
					line_ss.str("");
				}
				// Add whatever's left
				line_ss << word.substr(i);
				lineW = wordW;
				if (ch == ' ') { addSpace = true; lineW += spaceW; } 				else { addSpace = false; }
			}
			if (ch == '\n') {
				lines.push_back(line_ss.str());
				line_ss.str("");
				lineW = 0;
				addSpace = false;
			}
			// Continue building word
		} else {
			word_ss << ch;
		}
	}
	return lines;
}

// Functions to create a texture
Texture AssetManager::renderText(const TextData& data) {
	Texture tex = makeTexture();
	if (data.text.empty()) { return tex; }
	SharedFont font = data.getFont(*this);
	if (!font) {
		std::cerr << "renderText(): Invalid font" << std::endl;
		return tex;
	}
	Surface surface = makeSurface();
	if (data.bkgrnd == TRANSPARENT) {
		surface.reset(TTF_RenderText_Blended(font.get(),
			data.text.c_str(), data.color));
	} else {
		surface.reset(TTF_RenderText_Shaded(font.get(),
			data.text.c_str(), data.color, data.bkgrnd));
	}
	if (!surface) {
#ifdef DEBUG_RENDER
		std::cerr << "renderText(): Unable to render text" << std::endl;
#endif
		return tex;
	}
	tex = makeTexture(SDL_CreateTextureFromSurface(
		mRenderer.get(), surface.get()));
	return tex;
}

Texture AssetManager::renderTextWrapped(const WrappedTextData& data) {
	Texture tex = makeTexture();
	SharedFont font = data.getFont(*this);
	if (!font) {
		std::cerr << "renderTextWrapped(): Could not load font" << std::endl;
		return tex;
	}
	std::vector<std::string> lines = splitText(data.text, font, data.w);
	if (lines.empty()) { return tex; }

	// TODO: Switch to all SDL_Texture* (faster)?
	int lineH = TTF_FontHeight(font.get());
	int h = lineH * lines.size();
	Surface surf = makeSurface(SDL_CreateRGBSurface(0, data.w, h, 32,
		rmask, gmask, bmask, amask));
	if (data.bkgrnd != TRANSPARENT) {
		SDL_FillRect(surf.get(), NULL, toUint(data.bkgrnd));
	}
	Rect lineR(0, 0, data.w, lineH);
	for (std::string line : lines) {
		if (line != "") {
			Surface lineSurf = makeSurface(TTF_RenderText_Blended(font.get(),
				line.c_str(), data.color));
			if (lineSurf) {
				Rect lineRect = Rect(0, 0, lineSurf->w, lineSurf->h);
				lineRect.setPos(lineR, data.textDir, Rect::PosType::pCenter);
				SDL_BlitSurface(lineSurf.get(), NULL, surf.get(), &lineRect);
			} else {
#ifdef RENDER_DEBUG
				std::cerr << "line '" << line << "' produced a null surface"
					<< std::endl;
#endif
			}
		}
		lineR.y += lineH;
	}
	tex = makeTexture(SDL_CreateTextureFromSurface(mRenderer.get(), surf.get()));
	return tex;
}

// Functions to draw a texture
void AssetManager::drawTexture(const RenderData& data) {
	SDL_Texture* target = SDL_GetRenderTarget(mRenderer.get());
	Rect renderBounds;
	getTargetSize(&renderBounds.w, &renderBounds.h);
	// Make sure we are actually drawing something
	if (data.dest.empty() || data.dest.invalid() ||
		data.area.empty() || data.boundary.empty()) {
#ifdef RENDER_DEBUG
		std::cerr << "drawTexture(): Empty drawing or boundary rect" << std::endl;
#endif
		return;
	}
	// Get and check the texture to draw
	SharedTexture tex = data.asset.getTexture(*this);
	if (!tex) {
#ifdef RENDER_DEBUG
		std::cerr << "drawTexture(): Invalid Texture";
		if (!data.useTexture) { std::cerr << " - " << data.textureId; }
		std::cerr << std::endl;
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
		std::cerr << "drawTexture(): Boundary rect " << boundary
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
		std::cerr << "drawTexture(): Rect " << destRect << " was out side the boundary " <<
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
#ifdef RENDER_DEBUG
		std::cerr << "drawTexture(): Unable to query texture size: "
			<< SDL_GetError() << std::endl;
#endif
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
		std::cerr << "drawTexture(): Can't draw from " << texRect << " to " << destRect
			<< std::endl;
#endif
		return;
	}

	SDL_RenderCopy(mRenderer.get(), tex.get(), &texRect, &destRect);
}

Texture AssetManager::brightenTexture(SharedTexture src, Uint8 val) {
	int w, h;
	if (getTextureSize(src.get(), &w, &h)) {
		SDL_BlendMode mode;
		SDL_GetTextureBlendMode(src.get(), &mode);
		Uint8 r, g, b;
		SDL_GetTextureColorMod(src.get(), &r, &g, &b);

		Texture dest = AssetManager::createTexture(w, h);
		setRenderTarget(dest.get());
		setBlendMode(SDL_BLENDMODE_ADD);

		RenderData data;
		data.dest = Rect(0, 0, w, h);
		data.asset.setTexture(src);
		drawTexture(data);

		setDrawColor(SDL_Color{ val,val,val,255 });
		SDL_RenderFillRect(mRenderer.get(), NULL);
		resetDrawColor();

		resetBlendMode();
		resetRenderTarget();

		return dest;
	} else {
#ifndef RENDER_DEBUG
		std::cerr << "AssetManager::brightenTexture: "
			<< "could not query source texture" << std::endl;
#endif
	}
	return makeTexture();
}

Texture AssetManager::brightenTexture(std::string id, Uint8 val) {
	return brightenTexture(getAsset(id), val);
}
