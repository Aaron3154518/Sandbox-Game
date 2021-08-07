#include "AssetManager.h"

//namespace utils {
    void TextData::setRectPos(Rect& r) {
        switch (xMode) {
        case PosType::topleft: r.x = x; break;
        case PosType::center: r.setCenterX(x); break;
        case PosType::botright: r.setX2(x); break;
        default: break;
        }
        switch (yMode) {
        case PosType::topleft: r.y = y; break;
        case PosType::center: r.setCenterY(y); break;
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

    AssetManager::AssetManager() {}
    AssetManager::~AssetManager() { clean(); }

    void AssetManager::clean() {
        for (auto pair : assets) { SDL_DestroyTexture(pair.second); }
        assets.clear();
        for (auto pair : fonts) { TTF_CloseFont(pair.second); }
        fonts.clear();
    }

    SDL_Texture* AssetManager::getAsset(std::string id) {
        auto it = assets.find(id);
        return it == assets.end() ? loadAsset(id, id) : it->second;
    }

    TTF_Font* AssetManager::getFont(std::string id) const {
        auto it = fonts.find(id);
        return it == fonts.end() ? NULL : it->second;
    }

    void AssetManager::getFontSize(std::string fileName, int size, int* w, int* h) {
        TTF_Font* font = TTF_OpenFont(fileName.c_str(), size);
        TTF_SizeText(font, "|", w, NULL);
        *h = TTF_FontHeight(font);
        TTF_CloseFont(font);
    }

    SDL_Texture* AssetManager::loadAsset(std::string id, std::string fileName) {
        struct stat buffer;
        if (stat(fileName.c_str(), &buffer) != 0) {
            std::cout << "Could not find " << fileName << std::endl;
            return NULL;
        }
        SDL_Surface* tmpSurface = IMG_Load(fileName.c_str());
        SDL_Texture* tex = SDL_CreateTextureFromSurface(UI::renderer(), tmpSurface);
        SDL_FreeSurface(tmpSurface);
        if (tex) {
            auto it = assets.find(id);
            if (it == assets.end()) { assets[id] = tex; }
            else { SDL_DestroyTexture(it->second); it->second = tex; }
#ifdef RENDER_DEBUG
            std::cout << "Successfully loaded asset: " << id << std::endl;
#endif
        }
        return tex;
    }

    TTF_Font* AssetManager::loadFont(std::string id, std::string fileName, int maxW, int maxH) {
        int minSize = 1, maxSize = 10;
        int w, h;
        getFontSize(fileName, 1, &w, &h);
        while ((maxW != -1 && w < maxW) || (maxH != -1 && h < maxH)) {
            minSize = maxSize;
            maxSize *= 2;
            getFontSize(fileName, maxSize, &w, &h);
        }
        TTF_Font* font;
        while (true) {
            int size = (int)((maxSize + minSize) / 2);
            getFontSize(fileName, size, &w, &h);
            if ((maxW != -1 && w <= maxW) || (maxH != -1 && h <= maxH)) {
                getFontSize(fileName, size + 1, &w, &h);
                if ((maxW == -1 || w > maxW) && (maxH == -1 || h > maxH)) {
                    font = TTF_OpenFont(fileName.c_str(), size + 1);
                    break;
                }
                else {
                    minSize = size + 1;
                }
            }
            else {
                getFontSize(fileName, size - 1, &w, &h);
                if ((maxW == -1 || w > maxW) && (maxH == -1 || h > maxH)) {
                    maxSize = size - 1;
                }
                else {
                    font = TTF_OpenFont(fileName.c_str(), size - 1);
                    break;
                }
            }
        }
        if (font) {
            auto it = fonts.find(id);
            if (it == fonts.end()) { fonts[id] = font; }
            else { TTF_CloseFont(it->second); it->second = font; }
#ifdef RENDER_DEBUG
            std::cerr << "Successfully loaded font: " << id << std::endl;
#endif
        }
        return font;
    }

    SDL_Texture* AssetManager::renderText(TextData& data, Rect& rect) const {
        TTF_Font* font = getFont(data.fontId);
        if (!font) {
            std::cout << "Font: " << data.fontId << " not loaded" << std::endl;
            return NULL;
        }
        SDL_Surface* surface = TTF_RenderText_Blended(font, data.text.c_str(), data.color);
        SDL_Texture* tex = SDL_CreateTextureFromSurface(UI::renderer(), surface);
        SDL_FreeSurface(surface);
        if (tex) {
            rect = Rect::getMinRect(tex, data.w, data.h);
            data.setRectPos(rect);
        }
        return tex;
    }

    SDL_Texture* AssetManager::renderTextWrapped(TextData& data, Rect& rect, Uint32 bkgrnd) const {
        TTF_Font* font = getFont(data.fontId);
        if (font == nullptr) {
            std::cout << "Font: " << data.fontId << " not loaded" << std::endl;
            return nullptr;
        }
        if (data.w == 0) { return nullptr; }

        std::vector<std::string> lines;
        std::stringstream line_ss, word_ss;
        int spaceW;
        TTF_SizeText(font, " ", &spaceW, NULL);
        int width = 0;
        int maxW = data.w;
        bool addSpace = false;
        for (char ch : data.text + '\n') {
            if (ch == ' ' || ch == '\n' || ch == '\b') {
                std::string word = word_ss.str();
                word_ss.str("");
                int wordW;
                TTF_SizeText(font, word.c_str(), &wordW, NULL);
                if (width + wordW < data.w) {
                    if (addSpace) { line_ss << ' '; }
                    line_ss << word;
                    width += wordW;
                    if (ch == ' ') { addSpace = true; width += spaceW; }
                    else { addSpace = false; }
                }
                else {
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
            }
            else {
                word_ss << ch;
            }
        }

        // TODO: Switch to all SDL_Texture* (faster)
        int lineH;
        TTF_SizeText(font, "|", NULL, &lineH);
        rect = Rect(0, 0, maxW, (int)(lineH * lines.size()));
        data.setRectPos(rect);
        SDL_Surface* surf = SDL_CreateRGBSurface(0, rect.w, rect.h, 32, rmask, gmask, bmask, amask);
        if (bkgrnd != -1) { SDL_FillRect(surf, NULL, bkgrnd); }
        int x = maxW / 2, y = lineH / 2;
        for (std::string line : lines) {
            if (line == "") { y += lineH; continue; }
            SDL_Surface* lineSurf = TTF_RenderText_Blended(font, line.c_str(), data.color);
            if (lineSurf != nullptr) {
                Rect lineRect = Rect(0, 0, lineSurf->w, lineSurf->h);
                lineRect.setCenter(x, y);
                SDL_BlitSurface(lineSurf, NULL, surf, &lineRect);
                SDL_FreeSurface(lineSurf);
            }
            else {
                std::cout << "line '" << line << "' produced a null surface" << std::endl;
            }
            y += lineH;
        }
        lines.clear();
        SDL_Texture* tex = SDL_CreateTextureFromSurface(UI::renderer(), surf);
        SDL_FreeSurface(surf);
        return tex;
    }

    void AssetManager::drawTexture(SDL_Texture* tex, Rect& destRect, Rect* boundary) const {
        if (tex == NULL) {
            std::cout << "Invalid Texture" << std::endl;
            return;
        }
        Rect screen = Rect(0, 0, UI::width(), UI::height());
        if (boundary == NULL) {
            boundary = &screen;
        }
        else {
            SDL_IntersectRect(boundary, &screen, boundary);
        }

        double leftFrac = fmax(boundary->x - destRect.x, 0) / destRect.w;
        double topFrac = fmax(boundary->y - destRect.y, 0) / destRect.h;
        double rightFrac = fmax(destRect.x2() - boundary->x2(), 0) / destRect.w;
        double botFrac = fmax(destRect.y2() - boundary->y2(), 0) / destRect.h;
        // Make sure the rect is actually in the boundary
        if (leftFrac + rightFrac >= 1 || topFrac + botFrac >= 1) {
#ifdef RENDER_DEBUG
            std::cout << "Rect " << destRect << " was out side the boundary " << *boundary << std::endl;
#endif
            return;
        }

        Rect drawRect = Rect(destRect.x + (int)(destRect.w * leftFrac), destRect.y + (int)(destRect.h * topFrac),
            (int)(destRect.w * (1 - leftFrac - rightFrac)), (int)(destRect.h * (1 - topFrac - botFrac)));
        int w, h;
        if (SDL_QueryTexture(tex, NULL, NULL, &w, &h) != 0) {
            std::cout << "Unable to query texture size: "
                << SDL_GetError() << std::endl;
            return;
        }
        Rect texRect = Rect((int)(w * leftFrac), (int)(h * topFrac),
            (int)(w * (1 - leftFrac - rightFrac)), (int)(h * (1 - topFrac - botFrac)));
        // Make sure at least one pixel will be drawn
        if (drawRect.w == 0 || drawRect.h == 0 || texRect.w == 0 || texRect.h == 0) {
#ifdef RENDER_DEBUG
            std::cout << "Can't draw from " << texRect << " to " << drawRect << std::endl;
#endif
            return;
        }

        SDL_RenderCopy(UI::renderer(), tex, &texRect, &drawRect);
    }

    void AssetManager::drawText(TextData& data, Rect* boundary) const {
        Rect r;
        SDL_Texture* tex = renderText(data, r);
        if (tex != NULL) {
            drawTexture(tex, r, boundary);
            SDL_DestroyTexture(tex);
        }
    }

    void AssetManager::drawTextWrapped(TextData& data, Rect* boundary, Uint32 bkgrnd) const {
        Rect r;
        SDL_Texture* tex = renderTextWrapped(data, r, bkgrnd);
        if (tex != nullptr) {
            drawTexture(tex, r, boundary);
            SDL_DestroyTexture(tex);
        }
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
//}
