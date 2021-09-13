#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include <cmath>
#include <cstdlib>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <vector>

#include <SDL.h>
#include <SDL_ttf.h>

#include "Rect.h"
#include "Utils.h"
#include "../Definitions.h"

// ARGB masks for creating surfaces and colors
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
constexpr Uint32 rmask = 0xff000000;
constexpr Uint32 gmask = 0x00ff0000;
constexpr Uint32 bmask = 0x0000ff00;
constexpr Uint32 amask = 0x000000ff;
#else
constexpr Uint32 rmask = 0x000000ff;
constexpr Uint32 gmask = 0x0000ff00;
constexpr Uint32 bmask = 0x00ff0000;
constexpr Uint32 amask = 0xff000000;
#endif

// Colors
constexpr SDL_Color TRANSPARENT{ 0,0,0,0 };
constexpr SDL_Color WHITE{ 255,255,255,255 };
constexpr SDL_Color LGRAY{ 175,175,175,255 };
constexpr SDL_Color GRAY{ 128,128,128,255 };
constexpr SDL_Color BLACK{ 0,0,0,255 };
constexpr SDL_Color RED{ 255,0,0,255 };
constexpr SDL_Color ORANGE{ 255,165,0,255 };
constexpr SDL_Color YELLOW{ 255,255,0,255 };
constexpr SDL_Color GREEN{ 0,255,0,255 };
constexpr SDL_Color CYAN{ 0,255,255,255 };
constexpr SDL_Color BLUE{ 0,0,255,255 };
constexpr SDL_Color MAGENTA{ 255,0,255,255 };
constexpr SDL_Color PURPLE{ 128,0,128,255 };

bool operator==(const SDL_Color& lhs, const SDL_Color& rhs);
bool operator!=(const SDL_Color& lhs, const SDL_Color& rhs);

static Uint32 toUint(SDL_Color c) {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	Uint8 arr[4] = { c.r, c.g, c.b, c.a };
	return *(Uint32*)arr;
#else
	Uint8 arr[4] = { c.a, c.b, c.g, c.r };
	return *(Uint32*)arr;
#endif
}

// Memory management for textures and fonts
typedef std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> Texture;
typedef std::shared_ptr<SDL_Texture> SharedTexture;
Texture makeTexture(SDL_Texture* tex = NULL);
SharedTexture makeSharedTexture(SDL_Texture* tex = NULL);

typedef std::unique_ptr<TTF_Font, void(*)(TTF_Font*)> Font;
typedef std::shared_ptr<TTF_Font> SharedFont;
Font makeFont(TTF_Font* font = NULL);
SharedFont makeSharedFont(TTF_Font* font = NULL);

// Forward Declaration
class AssetManager;

struct FontData {
	int w = -1, h = -1;
	std::string sampleText = "";
	std::string fileName = gameVals::fontFile();
};

struct TextData {
	std::string text = "";
	SDL_Color color = BLACK;
	SDL_Color bkgrnd = TRANSPARENT;

	// True - use font, false - use fontId
	bool useFont = false;
	std::string fontId = "";
	SharedFont font = makeSharedFont();

	void setFontId(const std::string& id);
	void setFont(const SharedFont& newFont);
	SharedFont getFont(AssetManager& assets) const;
};

// TODO: Set h, variable w
struct WrappedTextData : public TextData {
	int w = 0;
	Rect::PosType textDir = Rect::PosType::pCenter;
};

struct Asset {
	// True - use texture, false - use textureId
	bool useTexture = false;
	std::string assetId = "";
	SharedTexture texture;

	void setAssetId(const std::string& id);
	void setTexture(const SharedTexture& tex);
	SharedTexture getTexture(AssetManager& assets) const;

	Rect getMinRect(AssetManager& assets, int maxW, int maxH) const;
};

struct RenderData {
	static const Rect& NO_RECT();

	Asset asset;
	Rect dest, area = NO_RECT(), boundary = NO_RECT();

	void fitToAsset(AssetManager& assets);
	void fitToAsset(AssetManager& assets, int maxW, int maxH);

	void addBoundary(const Rect& bounds);
};

class AssetManager {
public:
	typedef std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)> RendererPtr;

	AssetManager() = default;
	~AssetManager() = default;

	// TODO: Make private to window
	// Init functions
	void initAssets(SDL_Window* w);
	void initFonts();
	void quit();

	// Renderer functions
	SDL_Renderer* renderer();
	SDL_Point getRenderSize() const;
	void setDrawColor(const SDL_Color& c);
	void resetDrawColor();
	void setRenderTarget(SDL_Texture* tex);
	void resetRenderTarget();
	void setBlendMode(SDL_BlendMode mode);
	void resetBlendMode();

	// Texture/font size
	static bool getTextureSize(SDL_Texture* tex, int* w, int* h);
	static void getFontSize(std::string fileName, int size, int* w, int* h);
	static void getTextSize(std::string fileName, int size,
		std::string sampleText, int* w, int* h);
	// Create texture/font
	static Font createFont(const FontData& data);
	Texture createTexture(int w, int h, SDL_Color bkgrnd = TRANSPARENT);
	Texture loadImage(std::string fileName);

	bool assetExists(std::string id) const;
	bool fontExists(std::string id) const;

	bool loadAsset(std::string fileName);

	std::string addAsset(const SharedTexture& tex);
	std::string addFont(const FontData& data);

	void updateAsset(std::string id, const SharedTexture& tex);
	void updateFont(std::string id, const FontData& data);

	void removeAsset(std::string id);
	void removeFont(std::string id);

	SharedTexture getAsset(std::string id);

	SharedFont getFont(std::string id);

	// Split text
	static std::vector<std::string> splitText(const std::string& text,
		SharedFont font, int maxW);

	// Render text
	Texture renderText(const TextData& data);
	Texture renderTextWrapped(const WrappedTextData& data);

	// Draw textures/text
	void drawTexture(const RenderData& data);

	//Rect getMinRect(std::string id, int maxW, int maxH);

	// Draw rectangles
	enum BorderType : uint8_t { outside = 0, middle, inside };
	void rect(Rect* r, const SDL_Color& color,
		SDL_BlendMode mode = SDL_BLENDMODE_NONE);
	void thickRect(Rect r, int thickness, BorderType border,
		const SDL_Color& color);
	void circle(SDL_Point c, int r, const SDL_Color& color,
		SDL_BlendMode mode = SDL_BLENDMODE_NONE);
	void thickCircle(SDL_Point c, int r, int thickness, BorderType border,
		const SDL_Color& color, SDL_BlendMode mode = SDL_BLENDMODE_NONE);

	// Brighten a texture
	Texture brightenTexture(SharedTexture src, Uint8 val);
	Texture brightenTexture(std::string id, Uint8 val);

private:
	RendererPtr mRenderer = RendererPtr(NULL, SDL_DestroyRenderer);

	std::map<std::string, SharedTexture> assets;
	std::map<std::string, SharedFont> fonts;

	// Used for static assets/fonts that need to wait until init to be created
	std::vector<std::string> queuedAssets;
	std::vector<std::pair<std::string, FontData>> queuedFonts;
};

#endif /* Asset Manager */
