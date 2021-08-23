#include "YesNo.h"

const std::string YesNo::YES_IMG = gameVals::images() + "confirm.png";
const std::string YesNo::NO_IMG = gameVals::images() + "cancel.png";

YesNo::YesNo() {
	yesButton = Button(YES_IMG, 200);
	noButton = Button(NO_IMG, 200);
}

bool YesNo::handleEvents(Event& e) {
	if (e.clicked(e.left)) {
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
	if (e.scroll != 0 && SDL_PointInRect(&e.mouse, &promptTex.dest)) {
		scroll = std::max(0, std::min(maxScroll, scroll + e.scroll * scrollAmnt));
	}
	if (e.keyReleased(SDLK_ESCAPE)) {
		answer = mActive = false;
		return true;
	}
	return false;
}

void YesNo::draw() {
	UI::assets().rect(&mRect, GRAY);
	UI::assets().thickRect(promptTex.boundary, 3,
		AssetManager::BorderType::outside, BLACK);
	yesButton.draw(mRect.topLeft());
	noButton.draw(mRect.topLeft());
	promptTex.dest.setTopLeft(
		promptTex.boundary.topLeft() - SDL_Point{ 0,scroll });
	UI::assets().drawTexture(promptTex);
}

void YesNo::setRect(Rect rect) {
	mRect = rect;

	// Calculate UI variables
	int lineW = (int)(mRect.w * 4 / 5);
	int marginX = (int)((mRect.w - lineW) / 2);
	int lineH = (int)(mRect.h / 5);
	int marginY = (int)(lineH / 3);
	scrollAmnt = (int)(lineH / 3);

	promptTex.boundary = Rect(marginX, marginY, lineW, lineH * 3);
	yesButton.setRect(Rect(marginX, promptTex.boundary.y2() + marginY,
		lineH, lineH));
	noButton.setRect(Rect(mRect.w - lineH - marginX, yesButton.getRect().y,
		lineH, lineH));

	// Render the prompt
	TextData td;
	td.w = lineW;
	td.text = prompt;
	td.font = UI::assets().createFont(gameVals::fontFile(),
		-1, (int)(lineH / 2));
	promptTex.texture = UI::assets().renderTextWrapped(td, promptTex.dest);
	promptTex.boundary += mRect.topLeft();

	// Update scrolling
	int oldMax = maxScroll;
	maxScroll = std::max(0, promptTex.dest.h - promptTex.boundary.h);
	scroll = oldMax == 0 ? 0 : (int)(scroll * maxScroll / oldMax);
}
