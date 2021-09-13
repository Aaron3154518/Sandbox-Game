#include "Selector.h"

constexpr SDL_Color Selector::BKGRND{ 128,128,128,255 };
constexpr SDL_Color Selector::SCROLL_BKGRND{ 0,0,128,255 };

const std::string Selector::PLAY_IMG = gameVals::images() + "play.png";
const std::string Selector::DELETE_IMG = gameVals::images() + "delete.png";
const std::string Selector::ADD_IMG = gameVals::images() + "add.png";
const std::string Selector::EXIT_IMG = gameVals::images() + "exit.png";

Selector::Selector() :
	ITEM_FONT(Window::Get().assets().addFont(FontData{})) {
	textInput = true;

	itemText.setFontId(ITEM_FONT);
	itemText.color = WHITE;

	titleText.text = "Select <Item>";
	titleText.setFontId(ITEM_FONT);
	titleText.color = SCROLL_BKGRND;

	input.setFont(ITEM_FONT);
	input.setCharConstraint([](const char& ch) {
		return isalnum(ch) || ch == ' ';
		});

	// Setup textures
	AssetManager& assets = Window::Get().assets();
	newButton = Button(ADD_IMG, 200);
	playButton = Button(PLAY_IMG, 200);
	deleteButton = Button(DELETE_IMG, 200);
	exitButton = Button(EXIT_IMG, 100);
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

	title.dest = title.boundary = Rect((int)((mRect.w - itemW) / 2),
		mRect.y, itemW, itemH);

	scrollRect = Rect(title.dest.x, mRect.y + itemH,
		itemW, mRect.h - 2 * itemH);

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
	exitButton.setRect(Rect(0, 0, half + quarter, half + quarter));

	itemRect = Rect(scrollRect.x, scrollRect.y,
		title.dest.w - playRect.w, itemH);
	items.clear();

	maxScroll = std::max(0, (int)files.size() * itemH - scrollRect.h);

	Window::Get().assets().updateFont(ITEM_FONT, FontData{ -1, half + eighth });
	setTitle(titleText.text);
}

void Selector::setTitle(const std::string& newTitle) {
	AssetManager& assets = Window::Get().assets();
	titleText.text = newTitle;
	title.asset.setTexture(assets.renderText(titleText));
	title.fitToAsset(assets, title.dest.w, title.dest.h);
	title.dest.setCenter(title.boundary.cX(), title.boundary.cY());
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
			items.clear();
		}
		return;
	}

	bool leftClicked = any8(e[Event::Mouse::LEFT], Event::Button::M_CLICKED);

	if (leftClicked && exitButton.clicked(e.mouse)) {
		if (exit()) {
			running = false;
			return;
		}
	} else if (SDL_PointInRect(&e.mouse, &scrollRect)) {
		scroll = std::max(0,
			std::min(maxScroll, scroll + e.scroll * scrollAmnt));
		// Left click in scroll window
		if (leftClicked) {
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
	} else if (input.active() && leftClicked
		&& newButton.clicked(e.mouse) && newItem()) {
		onNewItem();
	}
	if (input.active()) {
		if (any8(e[SDLK_RETURN], Event::Button::RELEASED) && newItem()) {
			onNewItem();
		}
		input.handleEvents(e);
	}
}

void Selector::draw() {
	AssetManager& assets = Window::Get().assets();
	assets.rect(&mRect, BKGRND);

	// Draw title
	assets.drawTexture(title);

	drawScroll();
	if (input.active()) {
		// Draw current text input
		input.draw();

		// Draw add button
		newButton.draw(mRect.topLeft(), mRect);
	}
	if (deletePrompt.active()) {
		deletePrompt.draw();
	}
	exitButton.draw(mRect.topLeft(), mRect);
}

void Selector::drawScroll() {
	AssetManager& assets = Window::Get().assets();

	assets.rect(&scrollRect, SCROLL_BKGRND);
	int lb = scroll / itemH, ub = std::ceil((double)(scroll + scrollRect.h) / itemH);
	if (ub > files.size()) { ub = files.size(); }
	if (lb != sIdx || ub != sIdx + items.size()) {
		std::vector<RenderData> oldItems = items;
		items.resize(ub - lb);
		for (int i = lb; i < ub; i++) {
			// New item
			if (i < sIdx || i >= sIdx + oldItems.size()) {
				RenderData& data = items[i - lb];
				itemText.text = files[i];
				data.asset.setTexture(assets.renderText(itemText));
				data.boundary = scrollRect;
				data.fitToAsset(assets, itemRect.w, itemRect.h * 3 / 4);
			// Old item
			} else {
				items[i - lb] = oldItems[i - sIdx];
			}
		}
		sIdx = lb;
	}
	
	Rect r = itemRect - SDL_Point{ 0,scroll % itemH };
	for (int i = lb; i < ub; i++) {
		drawItem(i, r);
		r.y += itemH;
	}
}

void Selector::drawItem(int idx, const Rect& space) {
	idx -= sIdx;
	if (idx < 0 || idx >= items.size()) { return; }

	items[idx].dest.setCX(space.cX());
	items[idx].dest.setCY(space.cY());
	items[idx].boundary = scrollRect;
	Window::Get().assets().drawTexture(items[idx]);

	playButton.draw(space.topLeft(), scrollRect);
	deleteButton.draw(space.topLeft(), scrollRect);
}

void Selector::onNewItem() {
	maxScroll = std::max(0,
		(int)files.size() * itemH - scrollRect.h);
	scroll = maxScroll;
	input.clearInput();
	items.clear();
}

bool Selector::allowNewItems(bool val) {
	if (val != input.active()) {
		input.setActive(val);
		if (running) { resize(&mRect); }
	}
}