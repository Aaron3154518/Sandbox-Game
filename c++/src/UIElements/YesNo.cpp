#include "YesNo.h"

const std::string YesNo::YES_IMG = gameVals::images() + "confirm.png";
const std::string YesNo::NO_IMG = gameVals::images() + "cancel.png";

constexpr int TEXT_OUTLINE_W = 3;

YesNo::YesNo() {
	yesButton = Button(YES_IMG, 200);
	noButton = Button(NO_IMG, 200);
}

bool YesNo::handleEvents(Event& e) {
	if (any8(e[Event::Mouse::LEFT], Event::Button::M_CLICKED)) {
		if (SDL_PointInRect(&e.mouse, &mRect)) {
			if (yesButton.clicked(e.mouse)) {
				answer = true;
				mActive = false;
				return true;
			} else if (noButton.clicked(e.mouse)) {
				answer = mActive = false;
				return true;
			}
		} else {
			answer = mActive = false;
			return true;
		}
	}
	if (SDL_PointInRect(&e.mouse, &promptTex.boundary) && e.checkScroll()) {
		scroll = std::max(0,
			std::min(maxScroll, scroll + e.scroll * scrollAmnt));
	}
	if (any8(e[SDLK_ESCAPE], Event::Button::RELEASED)) {
		answer = mActive = false;
		e.handleKey(SDLK_ESCAPE);
		return true;
	}
	return false;
}

void YesNo::draw() {
	Window& w = Window::Get();
	w.assets().rect(&mRect, GRAY);
	w.assets().thickRect(promptTex.boundary, TEXT_OUTLINE_W,
		AssetManager::BorderType::outside, BLACK);
	yesButton.draw();
	noButton.draw();
	Rect r = promptTex.dest;
	promptTex.dest += promptTex.boundary.topLeft() - SDL_Point{ 0,scroll };
	w.assets().drawTexture(promptTex);
	promptTex.dest = r;
}

void YesNo::setRect(const Rect& rect) {
	mRect = rect;

	// Calculate UI variables
	int lineW = (int)(mRect.w * 4 / 5);
	int marginX = (int)((mRect.w - lineW) / 2);
	int lineH = (int)(mRect.h / 5);
	int marginY = (int)(lineH / 3);
	scrollAmnt = (int)(lineH / 3);

	Rect boundary = Rect(marginX, marginY, lineW, lineH * 3) + mRect.topLeft();
	yesButton.setRect(Rect(boundary.x + marginX,
		promptTex.boundary.y2() + marginY, lineH, lineH));
	noButton.setRect(Rect(boundary.x2() - lineH - marginX,
		yesButton.getRect().y, lineH, lineH));

	// Render the prompt
	AssetManager& assets = Window::Get().assets();
	TextData td;
	td.x = lineW / 2;
	td.xMode = TextData::PosType::center;
	td.w = lineW;
	td.text = prompt;
	td.setFont(assets.createFont(FontData{ -1, lineH / 2 }));
	promptTex = assets.renderTextWrapped(td);
	promptTex.boundary = boundary;

	// Update scrolling
	int oldMax = maxScroll;
	maxScroll = std::max(0, promptTex.dest.h - promptTex.boundary.h);
	scroll = oldMax == 0 ? 0 : (int)(scroll * maxScroll / oldMax);
}