#ifndef SELECTOR_H
#define SELECTOR_H

#include <SDL.h>

#include "UI.h"
#include "../Definitions.h"
#include "../UIElements/Button.h"
#include "../UIElements/TextInput.h"
#include "../UIElements/YesNo.h"
#include "../Utils/AssetManager.h"
#include "../Utils/Rect.h"
#include "../Utils/Utils.h"
#include "../Window.h"

class Selector : public UI {
public:
	Selector();
	~Selector();

	void resize(Rect* rect = nullptr);

	void handleEvents(Event& e);
	void draw();
	void drawScroll();

	Rect getRect() { return mRect; }

	void setTitle(const std::string& newTitle);
protected:
	void initUI();
	void tickUI(Event& e);

	void drawItem(int idx, const Rect& space);
	bool allowNewItems(bool val);

	void onNewItem();
	virtual bool exit() { return true; }
	virtual bool newItem() { return false; }
	virtual bool deleteItem(int idx) { return false; }
	virtual void selectItem(int idx) = 0;
	virtual void loadFiles() = 0;

	std::vector<std::string> files;

	int scroll = 0, maxScroll = 0;
	Rect mRect, scrollRect, itemRect;

	Button newButton, playButton, deleteButton, exitButton;
	RenderData title;

	// Used to render each item
	TextData itemText, titleText;
	// Loaded item textures for files[sIdx : sIdx + items.size()]
	int sIdx = 0;
	std::vector<RenderData> items;

	Rect inputName;
	TextInput input;

	int deleteIdx = 0;
	YesNo deletePrompt;

	// Dimensions
	int itemW = (int)(gameVals::MIN_W()/ 2);
	int itemH = (int)(gameVals::MIN_W() / 10);
	int scrollAmnt = (int)(itemH / 3);
	// Font resources
	const std::string ITEM_FONT;
	static const SDL_Color BKGRND, SCROLL_BKGRND;
	static const std::string PLAY_IMG, DELETE_IMG, ADD_IMG, EXIT_IMG;
};

#endif