#ifndef SELECTOR_H
#define SELECTOR_H

#include <dirent.h>
#include <SDL.h>

#include "../Definitions.h"
#include "../Utils/UI.h"
#include "../Utils/Rect.h"

class Selector : public UI {
public:
	Selector() = default;
	~Selector() { if (scrollTex) { SDL_DestroyTexture(scrollTex); } }

	void tick(Event& e);

	void resize(Rect* rect = nullptr);

	void draw();
	bool handleEvents(Event& e) { return true; }

	Rect getRect() { return mRect; }
private:
	void loadFiles() {}
	SDL_Texture* drawItem(int idx) { return NULL; }

	int scroll = 0, maxScroll = 0;
	Rect mRect, scrollRect;

	// Constants
	static const SDL_Color BKGRND{ 128,128,128 };
	static const SDL_Color SCROLL_BKGRND{ 0,0,128 };
	enum SType { player = 0, universe };
	// Dimensions
	int itemW = (int)(gameVals::MIN_W / 2);
	int itemH = (int)(gameVals::MIN_W / 10);
	int buttonW = (int)(ITEM_H / 2);
	int scrollAmnt = (int)(ITEM_H / 3);
	// Image resources

};

#endif