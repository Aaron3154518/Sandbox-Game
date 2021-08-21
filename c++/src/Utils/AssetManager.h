#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

//TODO: fix long lines

//#define RENDER_DEBUG

#include <sstream>
#include <string>
#include <map>
#include <cmath>
#include <memory>
#include <sys/stat.h>

#include <SDL.h>
#include <SDL_ttf.h>

#include "../Definitions.h"
#include "../UIs/UI.h"
#include "Rect.h"

//namespace utils {
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
    constexpr SDL_Color WHITE{ 255,255,255 };
    constexpr SDL_Color LGRAY{ 175,175,175 };
    constexpr SDL_Color GRAY{ 128,128,128 };
    constexpr SDL_Color BLACK{ 0  ,0  ,0 };
    constexpr SDL_Color RED{ 255,0  ,0 };
    constexpr SDL_Color ORANGE{ 255,165,0 };
    constexpr SDL_Color YELLOW{ 255,255,0 };
    constexpr SDL_Color GREEN{ 0  ,255,0 };
    constexpr SDL_Color CYAN{ 0  ,255,255 };
    constexpr SDL_Color BLUE{ 0  ,0  ,255 };
    constexpr SDL_Color MAGENTA{ 255,0  ,255 };
    constexpr SDL_Color PURPLE{ 128,0  ,128 };

    static Uint32 toUint(SDL_Color c) {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        Uint8 arr[4] = { c.r, c.g, c.b, c.a };
        return *(Uint32*)arr;
#else
        Uint8 arr[4] = { c.a, c.b, c.g, c.r };
        return *(Uint32*)arr;
#endif
    }

    typedef std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> Texture;
    typedef std::shared_ptr<SDL_Texture> SharedTexture;
    Texture makeTexture(SDL_Texture* tex = NULL);
    SharedTexture makeSharedTexture(SDL_Texture* tex = NULL);

    struct TextData {
        enum PosType : uint8_t {
            topleft = 0, center, botright
        };

        TTF_Font* font = NULL;
        std::string fontId = "", text = "";
        SDL_Color color = BLACK;
        double x = 0., y = 0.;
        PosType xMode = PosType::topleft, yMode = PosType::topleft;
        int w = 0, h = 0; // <= 0 for unbounded

        void setRectPos(Rect& r);
        void getPosFromRect(const Rect& r);
        void constrainToRect(const Rect& r);
        void deleteFont();
    };

    class AssetManager {
    public:
        AssetManager();
        ~AssetManager();

        void clean();

        static void getFontSize(std::string fileName, int size, int* w, int* h);

        SharedTexture loadAsset(std::string fileName);
        TTF_Font* loadFont(std::string id, std::string fileName, int w, int h);
        // Doesn't store the resulting TTF_Font*
        // caller is responsible for deallocating
        TTF_Font* loadFont(std::string fileName, int w, int h);

        SharedTexture getAsset(std::string fileName);
        TTF_Font* getFont(std::string id) const;
        TTF_Font* getFont(std::string id, std::string fileName, int w, int h);

        Texture createTexture(int w, int h) const;
        Texture renderText(TextData& data, Rect& rect) const;
        Texture renderTextWrapped(TextData& data, Rect& rect, Uint32 bkgrnd = -1) const;
        void drawTexture(SDL_Texture* tex, const Rect& destRect, Rect* boundary = NULL) const;
        void drawTexture(std::string fileName, const Rect& destRect, Rect* boundary = NULL);
        void drawText(TextData& data, Rect* boundary) const;
        void drawTextWrapped(TextData& data, Rect* boundary, Uint32 bkgrnd = -1) const;

        enum BorderType : uint8_t { outside = 0, middle, inside };
        void rect(Rect* r, const SDL_Color& color) const;
        void thickRect(Rect r, int thickness, BorderType border, const SDL_Color& color) const;

        //void drawProgressBar(Number amnt, Number cap, Rect& rect, SDL_Color color, SDL_Color bkgrnd) const;
        //void drawProgressBarLog(Number amnt, Number cap, Rect& rect, SDL_Color color, SDL_Color bkgrnd) const;
    private:
        std::map<std::string, SharedTexture> assets;
        std::map<std::string, TTF_Font*> fonts;
    };
//}

#endif /* Asset Manager */
