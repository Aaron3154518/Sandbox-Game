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
protected:
	void initUI();
	void tickUI(Event& e);

	Texture drawItem(int idx);
	bool allowNewItems(bool val);

	void onNewItem();
	virtual bool exit() { return true; }
	virtual bool newItem() { return false; }
	virtual bool deleteItem(int idx) { return false; }
	virtual void selectItem(int idx) = 0;
	virtual void loadFiles() = 0;

	std::vector<std::string> files;

	std::string title = "Select <item>";
	int scroll = 0, maxScroll = 0;
	Rect mRect, scrollRect, itemRect, titleRect;
	Button newButton, playButton, deleteButton, exitButton;
	TextData itemText;

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
};

#endif