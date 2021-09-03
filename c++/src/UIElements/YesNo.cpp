#include "YesNo.h"

const std::string YesNo::YES_IMG = gameVals::images() + "confirm.png";
const std::string YesNo::NO_IMG = gameVals::images() + "cancel.png";

YesNo::YesNo() {
	yesButton = Button(YES_IMG, 200);
	noButton = Button(NO_IMG, 200);
}

bool YesNo::handleEvents(Event& e) {
	if (e.checkMouse(Event::Mouse::LEFT, Event::ButtonStatus::CLICKED)) {
		if (SDL_PointInRect(&e.mouse, &mRect)) {
			SDL_Point mouse = e.mouse - mRect.topLeft();
			if (yesButton.clicked(mouse)) {
				answer = true;
				mActive = false;
				return true;
			} else if (noButton.clicked(mouse)) {
				answer = mActive = false;
				return true;
			}
		} else {
			answer = mActive = false;
			return true;
		}
	}
	if (SDL_PointInRect(&e.mouse, &promptTex.dest) && e.checkScroll()) {
		scroll = std::max(0,
			std::min(maxScroll, scroll + e.scroll * scrollAmnt));
	}
	if (e.checkAndHandleKey(SDLK_ESCAPE, Event::ButtonStatus::RELEASED)) {
		answer = mActive = false;
		return true;
	}
	return false;
}

void YesNo::draw() {
	Window& w = Window::Get();
	w.assets().rect(&mRect, GRAY);
	w.assets().thickRect(promptTex.boundary, 3,
		AssetManager::BorderType::outside, BLACK);
	yesButton.draw(mRect.topLeft());
	noButton.draw(mRect.topLeft());
	promptTex.dest.setTopLeft(
		promptTex.boundary.topLeft() - SDL_Point{ 0,scroll });
	w.assets().drawTexture(promptTex);
}

void YesNo::setRect(Rect rect) {
	mRect = rect;

	// Calculate UI variables
	int lineW = (int)(mRect.w * 4 / 5);
	int marginX = (int)((mRect.w - lineW) / 2);
	int lineH = (int)(mRect.h / 5);
	int marginY = (int)(lineH / 3);
	scrollAmnt = (int)(lineH / 3);

	promptTex.boundary = Rect(marginX, marginY, lineW, lineH * 3)
		+ mRect.topLeft();
	yesButton.setRect(Rect(marginX, promptTex.boundary.y2() + marginY,
		lineH, lineH));
	noButton.setRect(Rect(mRect.w - lineH - marginX, yesButton.getRect().y,
		lineH, lineH));

	// Render the prompt
	AssetManager& assets = Window::Get().assets();
	TextData td;
	td.w = lineW;
	td.text = prompt;
	td.font = assets.createFont(FontData{ -1, lineH / 2 });
	promptTex = assets.renderTextWrapped(td);

	// Update scrolling
	int oldMax = maxScroll;
	maxScroll = std::max(0, promptTex.dest.h - promptTex.boundary.h);
	scroll = oldMax == 0 ? 0 : (int)(scroll * maxScroll / oldMax);
}