#include "Selector.h"

void Selector::tick(Event& e) {
	handleEvents(e)
	draw();
}

void Selector::resize(Rect* rect) {
	mRect = rect ? *rect : Rect(0, 0, UI::width(), UI::height());
	itemW = (int)(min(mRect.w, MIN_W) / 2);
	itemH = (int)(min(mRect.h, MIN_H) / 10);
	buttonW = (int)(itemH / 2);
	scrollAmnt = (int)(itemH / 3);
}

void Selector::draw() {
	UI::setDrawColor(BKGRND);
	SDL_RenderFillRect(UI::renderer(), mRect);
	UI::setDrawColor(SCROLL_BKGRND);
	SDL_RenderFillRect(UI::renderer(), scrollRect);
	UI::resetDrawColor();
	for (int i = (int)(scroll / itemH);
		i < (int)((scroll + scrollRect.h) / itemH); i++) {
		SDL_Texture* tex = drawItem(i);
		if (!tex) { continue; }
		int w, h;
		SDL_QueryTexture(tex, NULL, NULL, &w, &h);
		int y = i * itemH - scroll;
		Rect src(0, 0, w, h), dest(scrollRect.x, scrollRect.y + y, itemW, itemH);
		// Check upper bouunds
		if (y < 0) {
			int dh = (int)(h * y / itemH);
			src.y -= dh;
			src.h += dh;
			dest.y -= y;
			dest.h += y;
		}
		y += h - scrollRect.h;
		// Check lower bounds
		if (y > 0) {
			int dh = (int)(h * y / itemH);
			src.h -= dh;
			dest.h -= y;
		}
		SDL_RenderCopy(UI::renderer(), tex, src, dest);
		SDL_DestroyTexture(tex);
	}
}