#include "Selector.h"

const SDL_Color Selector::BKGRND{ 128,128,128 };
const SDL_Color Selector::SCROLL_BKGRND{ 0,0,128 };

const std::string Selector::PLAY_IMG = createFile(IMAGES, "play", ".png");
const std::string Selector::DELETE_IMG = createFile(IMAGES, "delete", ".png");
const std::string Selector::ADD_IMG = createFile(IMAGES, "add", ".png");

// Used to give selectors uniquely named fonts
static int selectorCnt = 0;

Selector::Selector() :
	ITEM_FONT(std::string("selectorfont").append(std::to_string(selectorCnt++))) {
	itemText.fontId = ITEM_FONT;
	itemText.color = WHITE;
	itemText.xMode = itemText.yMode = PosType::center;
}

void Selector::runUI() {
	resize(NULL);
	UI::runUI();
}

void Selector::tick(Event& e) {
	handleEvents(e);
	draw();
}

void Selector::handleEvents(Event& e) {
	if (e.quit) {
		running = false;
		return;
	} else if (e.resize) {
		resize(NULL);
	}
}

void Selector::resize(Rect* rect) {
	mRect = rect ? *rect : Rect(0, 0, UI::width(), UI::height());
	//itemW = (int)(std::max(mRect.w, gameVals::MIN_W) / 2);
	itemW = (int)(mRect.w / 2);
	//itemH = (int)(std::max(mRect.h, gameVals::MIN_H) / 10);
	itemH = (int)(mRect.h / 10);
	//buttonW = (int)(itemH / 2);
	scrollAmnt = (int)(itemH / 3);

	// Set item text location
	itemText.y = (int)(itemH / 2);
	itemText.x = (int)((itemW - itemText.y) / 2);

	scrollRect = Rect((int)((mRect.w - itemW) / 2), itemH,
		itemW, mRect.h - 2 * itemH);

	UI::assets().loadFont(ITEM_FONT, createFile(FONTS, "times", ".ttf"),
		-1, (int)(itemH / 2));
}

void Selector::drawScroll() {
	UI::setDrawColor(BKGRND);
	SDL_RenderFillRect(UI::renderer(), &mRect);
	UI::setDrawColor(SCROLL_BKGRND);
	SDL_RenderFillRect(UI::renderer(), &scrollRect);
	UI::resetDrawColor();
	for (int i = (int)(scroll / itemH);
		i <= (int)((scroll + scrollRect.h) / itemH); i++) {
		SDL_Texture* tex = drawItem(i);
		if (!tex) { continue; }
		int y = i * itemH - scroll;
		Rect dest(scrollRect.x, scrollRect.y + y, itemW, itemH);
		UI::assets().drawTexture(tex, dest, &scrollRect);
		//SDL_RenderCopy(UI::renderer(), tex, src, dest);
		SDL_DestroyTexture(tex);
	}
}