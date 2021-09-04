#include "Selector.h"

const SDL_Color Selector::BKGRND{ 128,128,128,255 };
const SDL_Color Selector::SCROLL_BKGRND{ 0,0,128 };

const std::string Selector::PLAY_IMG = gameVals::images() + "play.png";
const std::string Selector::DELETE_IMG = gameVals::images() + "delete.png";
const std::string Selector::ADD_IMG = gameVals::images() + "add.png";

Selector::Selector() :
	ITEM_FONT(Window::Get().assets().addFont(FontData{})) {
	textInput = true;

	itemText.fontId = ITEM_FONT;
	itemText.color = WHITE;
	itemText.xMode = itemText.yMode = TextData::PosType::center;

	input.setFont(ITEM_FONT);
	input.setCharConstraint([](const char& ch) {
		return isalnum(ch) || ch == ' ';
		});

	// Setup textures
	AssetManager& assets = Window::Get().assets();
	newButton = Button(ADD_IMG, 200);
	playButton = Button(PLAY_IMG, 200);
	deleteButton = Button(DELETE_IMG, 200);
}

Selector::~Selector() {
	Window::Get().assets().removeFont(ITEM_FONT);
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
	SDL_Point dim = Window::Get().screenDim();
	mRect = rect ? *rect : Rect(0, 0, dim.x, dim.y);
	itemW = (int)(mRect.w / 2);
	itemH = (int)(mRect.h / 10);
	scrollAmnt = (int)(itemH / 3);

	int half = (int)(itemH / 2);
	int quarter = (int)(itemH / 4);
	int eighth = (int)(itemH / 8);

	titleRect = Rect((int)((mRect.w - itemW) / 2), 0, itemW, itemH);

	scrollRect = Rect(titleRect.x, titleRect.y2(),
		itemW, mRect.h - 2 * itemH);
	scrollRect += mRect.topLeft();

	if (input.active()) { scrollRect.h -= itemH; }

	SDL_Point offset{ scrollRect.x, scrollRect.y2() };

	newButton.setRect(
		Rect(itemW - half - quarter, quarter, half, half) + offset);
	input.setRect(
		Rect(eighth, eighth, itemW - itemH, itemH - quarter) + offset);

	Rect promptRect(0, 0, (int)(mRect.w / 3), (int)(mRect.h / 3));
	promptRect.setCenter(mRect.cX(), mRect.cY());
	deletePrompt.setRect(promptRect);
	Rect playRect = Rect(itemW - half, 0, half, half);
	playButton.setRect(playRect);
	deleteButton.setRect(Rect(playRect.x, playRect.y2(), half, half));

	itemRect = Rect(0, 0, titleRect.w - playRect.w, titleRect.h);

	maxScroll = std::max(0, (int)files.size() * itemH - scrollRect.h);

	itemText.h = half + eighth;
	Window::Get().assets().updateFont(ITEM_FONT, FontData{ -1, itemText.h });
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
						<< " Deletion is permanent. (Or maybe not, Idk)";
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
	AssetManager& assets = Window::Get().assets();
	assets.rect(&mRect, BKGRND);

	// Draw title
	itemText.text = title;
	itemText.setPos(titleRect);
	itemText.color = SCROLL_BKGRND;
	TextureData texData = assets.renderText(itemText);
	texData.boundary = titleRect;
	assets.drawTexture(texData);
	itemText.color = WHITE;

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
	// Set item text location
	itemText.setPos(itemRect);

	Window::Get().assets().rect(&scrollRect, SCROLL_BKGRND);
	int lb = scroll / itemH, ub = (scroll + scrollRect.h) / itemH;
	SDL_Point topLeft = mRect.topLeft() + scrollRect.topLeft()
		+ SDL_Point{ 0,-scroll + lb * itemH };
	for (int i = lb; i <= ub; i++) {
		TextureData data;
		data.setTexture(drawItem(i));
		if (!data.texture) { continue; }
		int y = i * itemH - scroll;
		data.dest = Rect(scrollRect.x, scrollRect.y + y, itemW, itemH);
		data.boundary = scrollRect;
		Window::Get().assets().drawTexture(data);

		playButton.draw(topLeft, scrollRect);
		deleteButton.draw(topLeft, scrollRect);
		topLeft.y += itemH;
	}
}

Texture Selector::drawItem(int idx) {
	if (idx >= files.size()) { return makeTexture(); }

	AssetManager& assets = Window::Get().assets();
	Texture tex = assets.createTexture(itemW, itemH);
	assets.setRenderTarget(tex.get());
	assets.rect(NULL, BLACK);

	itemText.text = files[idx];
	TextureData texData = assets.renderText(itemText);
	texData.boundary = itemRect;
	assets.drawTexture(texData);

	assets.resetRenderTarget();
	return tex;
}

void Selector::onNewItem() {
	maxScroll = std::max(0,
		(int)files.size() * itemH - scrollRect.h);
	scroll = maxScroll;
	input.clearInput();
}

bool Selector::allowNewItems(bool val) {
	if (val != input.active()) {
		input.setActive(val);
		if (running) { resize(&mRect); }
	}
}