#ifndef SELECTOR_H
#define SELECTOR_H

#include <dirent.h>
#include <SDL.h>

#include "../Definitions.h"
#include "UI.h"
#include "TextInput.h"
#include "../UIElements/YesNo.h"
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
	virtual void draw();
	virtual void drawScroll();

	Rect getRect() { return mRect; }
protected:
	virtual SDL_Texture* drawItem(int idx);
	bool setAllowNewItems(bool val);

	virtual bool newItem() { return false; }
	virtual bool deleteItem(int idx) { return false; }
	virtual void selectItem(int idx) = 0;
	virtual void loadFiles() = 0;

	std::vector<std::string> files;

	int scroll = 0, maxScroll = 0;
	Rect mRect, scrollRect;
	Rect buttonPlay, buttonDelete;
	TextData itemText;

	Rect buttonNew, inputName;
	TextInput input;

	int deleteIdx = 0;
	YesNo deletePrompt;

	// Constants
	static const SDL_Color BKGRND;
	static const SDL_Color SCROLL_BKGRND;
	// Dimensions
	int itemW = (int)(gameVals::MIN_W / 2);
	int itemH = (int)(gameVals::MIN_W / 10);
	int scrollAmnt = (int)(itemH / 3);
	// Font resources
	const std::string ITEM_FONT;
	// Image resources
	static const std::string PLAY_IMG;
	static const std::string DELETE_IMG;
	static const std::string ADD_IMG;
private:
	bool allowNewItems = false;
};

#endif