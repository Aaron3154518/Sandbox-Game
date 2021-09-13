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
	rect(NULL, TRANSPARENT);
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
	SDL_Surface* surface = NULL;
	if (data.bkgrnd == TRANSPARENT) {
		surface = TTF_RenderText_Blended(font.get(),
			data.text.c_str(), data.color);
	} else {
		surface = TTF_RenderText_Shaded(font.get(),
			data.text.c_str(), data.color, data.bkgrnd);
	}
	if (!surface) {
#ifdef DEBUG_RENDER
		std::cerr << "renderText(): Unable to render text" << std::endl;
#endif
		return tex;
	}
	tex = makeTexture(SDL_CreateTextureFromSurface(mRenderer.get(), surface));
	SDL_FreeSurface(surface);
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
	SDL_Surface* surf = SDL_CreateRGBSurface(0, data.w, h, 32,
		rmask, gmask, bmask, amask);
	if (data.bkgrnd != TRANSPARENT) {
		SDL_FillRect(surf, NULL, toUint(data.bkgrnd));
	}
	Rect lineR(0, 0, data.w, lineH);
	for (std::string line : lines) {
		if (line != "") {
			SDL_Surface* lineSurf = TTF_RenderText_Blended(font.get(),
				line.c_str(), data.color);
			if (lineSurf) {
				Rect lineRect = Rect(0, 0, lineSurf->w, lineSurf->h);
				lineRect.setPos(lineR, data.textDir, Rect::PosType::pCenter);
				SDL_BlitSurface(lineSurf, NULL, surf, &lineRect);
				SDL_FreeSurface(lineSurf);
			} else {
#ifdef RENDER_DEBUG
				std::cerr << "line '" << line << "' produced a null surface"
					<< std::endl;
#endif
			}
		}
		lineR.y += lineH;
	}
	tex = makeTexture(SDL_CreateTextureFromSurface(mRenderer.get(), surf));
	SDL_FreeSurface(surf);
	return tex;
}

// Functions to draw a texture
void AssetManager::drawTexture(const RenderData& data) {
	SDL_Texture* target = SDL_GetRenderTarget(mRenderer.get());
	Rect renderBounds;
	if (!target) {
		SDL_Point dim = getRenderSize();
		renderBounds.resize(dim.x, dim.y, false);
	} else { getTextureSize(target, &renderBounds.w, &renderBounds.h); }
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

// TODO: negatic thickness (SDL_gfx)
// TODO: boundary
// Functions to draw a rectangle
void AssetManager::rect(Rect* r, const SDL_Color& color,
	SDL_BlendMode mode) {
	setBlendMode(mode);
	setDrawColor(color);
	SDL_RenderFillRect(mRenderer.get(), r);
	resetDrawColor();
	resetBlendMode();
}

void AssetManager::thickRect(Rect r, int thickness,
	AssetManager::BorderType border, const SDL_Color& color) {
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
	setDrawColor(color);
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

void AssetManager::circle(SDL_Point c, int r, const SDL_Color& color,
	SDL_BlendMode mode) {
	thickCircle(c, r, r, BorderType::inside, color, mode);
}

void AssetManager::thickCircle(SDL_Point c, int r, int thickness,
	BorderType border, const SDL_Color& color, SDL_BlendMode mode) {
	if (r <= 0) { return; }
	switch (border) {
		case BorderType::inside:
		{
			setBlendMode(mode);
			setDrawColor(color);
			int r2 = r - thickness;
			int dx = 0;
			while (dx < r) {
				int ub = (int)(std::sqrt(r * r - dx * dx) + .5);
				int lb = dx >= r2 ? 0 : (int)(std::sqrt(r2 * r2 - dx * dx) + .5);
				double x = c.x + dx;
				SDL_RenderDrawLine(mRenderer.get(), x, c.y + lb, x, c.y + ub);
				SDL_RenderDrawLine(mRenderer.get(), x, c.y - lb, x, c.y - ub);
				x = c.x - dx;
				SDL_RenderDrawLine(mRenderer.get(), x, c.y + lb, x, c.y + ub);
				SDL_RenderDrawLine(mRenderer.get(), x, c.y - lb, x, c.y - ub);
				dx++;
			}
			resetDrawColor();
			resetBlendMode();
			break;
		}
		case BorderType::outside:
			thickCircle(c, r + thickness, thickness, BorderType::inside,
				color, mode);
			break;
		case BorderType::middle:
		{
			int lb = -(int)(thickness / 2);
			int ub = (int)(((double)thickness / 2) + .5);
			thickCircle(c, r + ub, ub - lb, BorderType::inside, color, mode);
			break;
		}
		default:
			break;
	}
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