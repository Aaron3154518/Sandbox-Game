#include "Selector.h"

const SDL_Color Selector::BKGRND{ 128,128,128,255 };
const SDL_Color Selector::SCROLL_BKGRND{ 0,0,128 };

const std::string Selector::PLAY_IMG = gameVals::images() + "play.png";
const std::string Selector::DELETE_IMG = gameVals::images() + "delete.png";
const std::string Selector::ADD_IMG = gameVals::images() + "add.png";
// Used to give selectors uniquely named fonts
static int selectorCnt = 0;

Selector::Selector(bool allowTextInput) :
	ITEM_FONT(std::string("selectorfont").append(std::to_string(selectorCnt++))) {
	itemText.fontId = ITEM_FONT;
	itemText.color = WHITE;
	itemText.xMode = itemText.yMode = TextData::PosType::center;

	textInput = true;

	TextData td;
	td.fontId = ITEM_FONT;
	td.color = WHITE;
	td.xMode = TextData::PosType::topleft;
	td.yMode = TextData::PosType::center;
	input.setBackground(BLACK);
	input.setTextData(td);
	input.setCharConstraint([](const char& ch) {
		return isalnum(ch) || ch == ' ';
		});
	input.setActive(allowTextInput);

	// Setup textures
	AssetManager& assets = UI::assets();
	newButton = Button(ADD_IMG, 200);
	playButton = Button(PLAY_IMG, 200);
	deleteButton = Button(DELETE_IMG, 200);
}

void Selector::initUI() {
	loadFiles();
	resize(NULL);
}

void Selector::tickUI(Event& e) {
	handleEvents(e);
	draw();
}

void Selector::resize(Rect* rect) {
	mRect = rect ? *rect : Rect(0, 0, UI::width(), UI::height());
	//itemW = (int)(std::max(mRect.w, gameVals::MIN_W()) / 2);
	itemW = (int)(mRect.w / 2);
	//itemH = (int)(std::max(mRect.h, gameVals::MIN_H()) / 10);
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
	newButton.setRect(
		Rect(itemW - half - quarter, quarter, half, half) + offset);
	inputName = Rect(eigth, eigth, itemW - itemH, itemH - quarter) + offset;

	maxScroll = std::max(0, (int)files.size() * itemH - scrollRect.h);

	input.setRect(inputName);
	Rect promptRect(0, 0, (int)(mRect.w / 3), (int)(mRect.h / 3));
	promptRect.setCenter(mRect.cX(), mRect.cY());
	deletePrompt.setRect(promptRect);
	Rect playRect = Rect(itemW - half, 0, half, half);
	playButton.setRect(playRect);
	deleteButton.setRect(Rect(playRect.x, playRect.y2(), half, half));

	UI::assets().loadFont(ITEM_FONT, gameVals::fontFile(), -1, half);
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
		if (e.checkMouse(Event::Mouse::LEFT, Event::ButtonStatus::CLICKED)) { 
			SDL_Point mouse = e.mouse - scrollRect.topLeft();
			mouse.y += scroll;
			int idx = (int)(mouse.y / itemH);
			mouse.y -= idx * itemH;
			// Clicked player or delete button
			if (idx >= 0 && idx < files.size()) {
				if (playButton.clicked(mouse)) {
					selectItem(idx);
				} else if (deleteButton.clicked(mouse)) {
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
	} else if (input.active()
		&& e.checkMouse(Event::Mouse::LEFT, Event::ButtonStatus::CLICKED)
		&& newButton.clicked(e.mouse) && newItem()) {
		onNewItem();
	}
	if (input.active()) {
		if (e.checkKey(SDLK_RETURN, Event::ButtonStatus::RELEASED)
			&& newItem()) {
			onNewItem();
		}
		input.handleEvents(e);
	}
}

void Selector::draw() {
	UI::assets().rect(&mRect, BKGRND);
	drawScroll();
	if (input.active()) {
		// Draw current text input
		input.draw();

		// Draw add button
		newButton.draw(mRect.topLeft());
	}
	if (deletePrompt.active()) {
		deletePrompt.draw();
	}
}

void Selector::drawScroll() {
	UI::assets().rect(&scrollRect, SCROLL_BKGRND);
	int lb = scroll / itemH, ub = (scroll + scrollRect.h) / itemH;
	SDL_Point topLeft = mRect.topLeft() + scrollRect.topLeft()
		+ SDL_Point{ 0,-scroll + lb * itemH };
	for (int i = lb; i <= ub; i++) {
		TextureData data;
		data.texture = drawItem(i);
		if (!data.texture) { continue; }
		int y = i * itemH - scroll;
		data.dest = Rect(scrollRect.x, scrollRect.y + y, itemW, itemH);
		data.boundary = scrollRect;
		UI::assets().drawTexture(data);

		playButton.draw(topLeft, scrollRect);
		deleteButton.draw(topLeft, scrollRect);
		topLeft.y += itemH;
	}
}

Texture Selector::drawItem(int idx) {
	if (idx >= files.size()) { return makeTexture(); }

	Texture tex = UI::assets().createTexture(itemW, itemH);
	UI::setRenderTarget(tex.get());
	UI::assets().rect(NULL, BLACK);

	itemText.text = files[idx];
	UI::assets().drawText(itemText,
		Rect(0, 0, itemW - playButton.getRect().w, itemH));

	UI::resetRenderTarget();
	return tex;
}

void Selector::onNewItem() {
	maxScroll = std::max(0,
		(int)files.size() * itemH - scrollRect.h);
	scroll = maxScroll;
	input.clearInput();
}

bool Selector::toggleTextInput(bool val) {
	input.setActive(val);
	if (running) { resize(&mRect); }
}