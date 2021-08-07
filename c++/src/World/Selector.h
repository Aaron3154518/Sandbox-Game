#ifndef SELECTOR_H
#define SELECTOR_H

#include <dirent.h>
#include <SDL.h>

#include "../Definitions.h"
#include "../Utils/UI.h"
#include "../Utils/Utils.h"
#include "../Utils/Rect.h"
#include "../Utils/AssetManager.h"

class Selector : public UI {
public:
	Selector();
	~Selector() = default;

	virtual void runUI();
	virtual void tick(Event& e);

	virtual void resize(Rect* rect = nullptr);

	virtual void handleEvents(Event& e);
	virtual void draw() { drawScroll(); }
	virtual void drawScroll();

	Rect getRect() { return mRect; }
protected:
	virtual SDL_Texture* drawItem(int idx) { return NULL; }

	int scroll = 0, maxScroll = 0;
	Rect mRect, scrollRect;
	TextData itemText;

	// Constants
	static const SDL_Color BKGRND;
	static const SDL_Color SCROLL_BKGRND;
	enum SType { player = 0, universe };
	// Dimensions
	int itemW = (int)(gameVals::MIN_W / 2);
	int itemH = (int)(gameVals::MIN_W / 10);
	//int buttonW = (int)(ITEM_H / 2);
	int scrollAmnt = (int)(itemH / 3);
	// Font resources
	const std::string ITEM_FONT;
	// Image resources
	static const std::string PLAY_IMG;
	static const std::string DELETE_IMG;
	static const std::string ADD_IMG;
};

#endif