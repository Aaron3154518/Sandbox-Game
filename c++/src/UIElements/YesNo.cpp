#include "YesNo.h"

const std::string YesNo::YES_IMG = gameVals::images() + "confirm.png";
const std::string YesNo::NO_IMG = gameVals::images() + "cancel.png";

bool YesNo::handleEvents(Event& e) {
	SDL_Point pos = e.mouse - mRect.topLeft();
	if (e.clicked(e.left)) {
		if (SDL_PointInRect(&e.mouse, &mRect)) {
			if (SDL_PointInRect(&pos, &yesRect)) {
				answer = true;
				mActive = false;
				return true;
			} else if (SDL_PointInRect(&pos, &noRect)) {
				answer = mActive = false;
				return true;
			}
		} else {
			answer = mActive = false;
			return true;
		}
	}
	if (e.scroll != 0 && SDL_PointInRect(&e.mouse, &promptRect)) {
		scroll = std::max(0, std::min(maxScroll, scroll + e.scroll * scrollAmnt));
	}
	if (e.keyReleased(SDLK_ESCAPE)) {
		answer = mActive = false;
		return true;
	}
	return false;
}

void YesNo::draw() {
	UI::assets().drawTexture(mTex.get(), mRect);
	Rect r = promptRectFull - SDL_Point{ 0, scroll };
	UI::assets().drawTexture(promptTex.get(), r, &promptRect);
}

void YesNo::setRect(Rect rect) {
	mRect = rect;

	// Calculate UI variables
	int lineW = (int)(mRect.w * 4 / 5);
	int marginX = (int)((mRect.w - lineW) / 2);
	int lineH = (int)(mRect.h / 5);
	int marginY = (int)(lineH / 3);
	scrollAmnt = (int)(lineH / 3);

	promptRect = Rect(marginX, marginY, lineW, lineH * 3);
	yesRect = Rect(marginX, promptRect.y2() + marginY, lineH, lineH);
	noRect = Rect(mRect.w - lineH - marginX, yesRect.y, lineH, lineH);
	// Render the main texture
	mTex = UI::assets().createTexture(mRect.w, mRect.h);
	UI::setRenderTarget(mTex.get());
	UI::assets().rect(NULL, GRAY);
	UI::assets().thickRect(promptRect, 3, AssetManager::BorderType::outside, BLACK);
	UI::assets().drawTexture(YES_IMG, yesRect);
	UI::assets().drawTexture(NO_IMG, noRect);
	UI::resetRenderTarget();

	// Render the prompt
	TextData td;
	td.w = lineW;
	td.text = prompt;
	td.font = UI::assets().loadFont(gameVals::fontFile(), -1, (int)(lineH / 2));
	promptTex = UI::assets().renderTextWrapped(td, promptRectFull);
	promptRect += mRect.topLeft();
	promptRectFull += promptRect.topLeft();
	td.deleteFont();

	// Update scrolling
	int oldMax = maxScroll;
	maxScroll = std::max(0, promptRectFull.h - promptRect.h);
	scroll = oldMax == 0 ? 0 : (int)(scroll * maxScroll / oldMax);
}
