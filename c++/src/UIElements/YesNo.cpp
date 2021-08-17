#include "YesNo.h"

const std::string YesNo::YES_IMG = createFile(IMAGES, "confirm", ".png");
const std::string YesNo::NO_IMG = createFile(IMAGES, "cancel", ".png");

YesNo::YesNo(std::string _prompt) : prompt(_prompt) {}

YesNo::~YesNo() {
	if (mTex) { SDL_DestroyTexture(mTex); }
	if (promptTex) { SDL_DestroyTexture(promptTex); }
}

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
	UI::assets().drawTexture(mTex, mRect);
	Rect r = promptRectFull - SDL_Point{ 0, scroll };
	UI::assets().drawTexture(promptTex, r, &promptRect);
}

void YesNo::setRect(Rect rect) {
	if (mTex) { SDL_DestroyTexture(mTex); }
	if (promptTex) { SDL_DestroyTexture(promptTex); }
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
	mTex = SDL_CreateTexture(UI::renderer(), SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_TARGET, mRect.w, mRect.h);
	UI::setRenderTarget(mTex);
	UI::assets().rect(NULL, GRAY);
	UI::assets().thickRect(promptRect, 3, BLACK);
	UI::assets().drawTexture(YES_IMG, yesRect);
	UI::assets().drawTexture(NO_IMG, noRect);
	UI::resetRenderTarget();

	// Render the prompt
	TextData td;
	td.w = lineW;
	td.text = prompt;
	td.font = UI::assets().loadFont(TIMES_FONT, -1, (int)(lineH / 2));
	promptTex = UI::assets().renderTextWrapped(td, promptRectFull);
	promptRect += mRect.topLeft();
	promptRectFull += promptRect.topLeft();
	td.deleteFont();

	// Update scrolling
	int oldMax = maxScroll;
	maxScroll = std::max(0, promptRectFull.h - promptRect.h);
	scroll = oldMax == 0 ? 0 : (int)(scroll * maxScroll / oldMax);
}
