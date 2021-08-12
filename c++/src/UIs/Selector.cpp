#include "Selector.h"

const SDL_Color Selector::BKGRND{ 128,128,128 };
const SDL_Color Selector::SCROLL_BKGRND{ 0,0,128 };

const std::string Selector::PLAY_IMG = createFile(IMAGES, "play", ".png");
const std::string Selector::DELETE_IMG = createFile(IMAGES, "delete", ".png");
const std::string Selector::ADD_IMG = createFile(IMAGES, "add", ".png");

// Used to give selectors uniquely named fonts
static int selectorCnt = 0;

Selector::Selector(bool allowTextInput) :
	ITEM_FONT(std::string("selectorfont").append(std::to_string(selectorCnt++))) {
	itemText.fontId = ITEM_FONT;
	itemText.color = WHITE;
	itemText.xMode = itemText.yMode = PosType::center;

	textInput = true;

	TextData td;
	td.fontId = ITEM_FONT;
	td.color = WHITE;
	td.xMode = PosType::topleft;
	td.yMode = PosType::center;
	input.setBackground(BLACK);
	input.setTextData(td);
	input.setCharConstraint([](const char& ch) {
		return isalnum(ch) || ch == ' ';
		});
	input.setActive(allowTextInput);
}

void Selector::runUI() {
	loadFiles();
	resize(NULL);
	UI::runUI();
}

void Selector::tick(Event& e) {
	handleEvents(e);
	draw();
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
	scrollRect += mRect.topLeft();

	if (input.active()) { scrollRect.h -= itemH; }

	int half = (int)(itemH / 2);
	int quarter = (int)(itemH / 4);
	int eigth = (int)(itemH / 8);
	SDL_Point offset{ scrollRect.x, scrollRect.y2() };
	buttonNew = Rect(itemW - half - quarter, quarter, half, half) + offset;
	inputName = Rect(eigth, eigth, itemW - itemH, itemH - quarter) + offset;

	maxScroll = std::max(0, (int)files.size() * itemH - scrollRect.h);

	input.setRect(inputName);
	Rect promptRect(0, 0, (int)(mRect.w / 3), (int)(mRect.h / 3));
	promptRect.setCenter(mRect.cX(), mRect.cY());
	deletePrompt.setRect(promptRect);
	buttonPlay = Rect(itemW - half, 0, half, half);
	buttonDelete = Rect(buttonPlay.x, buttonPlay.y2(), half, half);

	UI::assets().loadFont(ITEM_FONT, TIMES_FONT, -1, half);
}

void Selector::handleEvents(Event& e) {
	if (e.quit) {
		running = false;
		return;
	}
	if (e.resize) { resize(NULL); }

	if (deletePrompt.active()) {
		if (deletePrompt.handleEvents(e) && deletePrompt.getAnswer()
			&& deleteItem(deleteIdx)) {
			maxScroll = std::max(0,
				(int)files.size() * itemH - scrollRect.h);
			scroll = std::min(maxScroll, scroll);
		}
		return;
	}

	if (SDL_PointInRect(&e.mouse, &scrollRect)) {
		scroll = std::max(0,
			std::min(maxScroll, scroll + e.scroll * scrollAmnt));
		// Left click in scroll window
		if (e.clicked(e.left)) {
			SDL_Point mouse = e.mouse - scrollRect.topLeft();
			mouse.y += scroll;
			int idx = (int)(mouse.y / itemH);
			// Clicked player or delete button
			if (idx < files.size() && mouse.x >= buttonPlay.x) {
				if (std::fmod(mouse.y, itemH) < buttonPlay.h) {
					selectItem(idx);
				} else {
					deleteIdx = idx;
					std::ostringstream ss;
					ss << "Delete " << files[idx] << "?";
					ss << " I mean, it's a really good question. Who knows what " << files[idx]
						<< " may contain. Rare shit? Nothing? I dunno. Anyways, make this decision carefully."
						<< " Deletion is permanent. (Or mabe not, Idk)";
					deletePrompt.setPrompt(ss.str());
					deletePrompt.setActive(true);
					return;
				}
			}
		}
	} else if (input.active() &&
		SDL_PointInRect(&e.mouse, &buttonNew) && e.clicked(e.left)) {
		if (newItem()) {
			maxScroll = std::max(0,
				(int)files.size() * itemH - scrollRect.h);
			scroll = maxScroll;
		}
	}
	if (input.active()) {
		if (e.keyReleased(SDLK_RETURN)) {
			if (newItem()) {
				maxScroll = std::max(0,
					(int)files.size() * itemH - scrollRect.h);
				scroll = maxScroll;
			}
		}
		input.handleEvents(e);
	}
}

void Selector::draw() {
	drawScroll();
	if (input.active()) {
		// Draw current text input
		input.draw();

		// Draw add button
		UI::assets().drawTexture(ADD_IMG, buttonNew, NULL);
	}
	if (deletePrompt.active()) {
		deletePrompt.draw();
	}
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
		SDL_DestroyTexture(tex);
	}
}

SDL_Texture* Selector::drawItem(int idx) {
	if (idx >= files.size()) { return NULL; }

	SDL_Texture* tex = SDL_CreateTexture(UI::renderer(),
		SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, itemW, itemH);
	UI::setRenderTarget(tex);

	UI::setDrawColor(BLACK);
	SDL_RenderFillRect(UI::renderer(), NULL);
	UI::resetDrawColor();

	UI::assets().drawTexture(PLAY_IMG, buttonPlay, NULL);
	UI::assets().drawTexture(DELETE_IMG, buttonDelete, NULL);

	itemText.text = files[idx];
	Rect r(0, 0, itemW - buttonPlay.w, itemH);
	UI::assets().drawText(itemText, &r);

	UI::resetRenderTarget();
	return tex;
}

bool Selector::toggleTextInput(bool val) {
	input.setActive(val);
	if (running) { resize(&mRect); }
}