#include "Spinner.h"

constexpr int KEY_LEN = 4;

constexpr SDL_Color CIRCLE_COLOR{ 0,255,0,255 };
constexpr SDL_Color SELECT_COLOR{ 0,0,255,255 };

int Spinner::mouseOnItem() {
	SDL_Point mouse = mousePos();
	if (SDL_PointInRect(&mouse, &dragData.mRect)) {
		mouse -= dragData.mRect.topLeft();
		for (int i = 0; i < rects.size(); i++) {
			if (SDL_PointInRect(&mouse, &rects[i])) {
				return i;
			}
		}
	}
	return -1;
}

bool Spinner::handleEvents(Event& e) {
	if (dragData.dragV(e)) { redraw(); }
	return dragData.isDragging();
}

void Spinner::draw() {
	if (rects.size() != items.size()) { redraw(); }
	AssetManager& assets = Window::Get().assets();
	if (bkgrnd != TRANSPARENT) {
		RectData({ bkgrnd }).set(dragData.mRect).render(assets);
	}
	RenderData rData;
	rData.boundary = dragData.mRect;
	for (int i = 0; i < rects.size(); i++) {
		rData.asset = items[i];
		rData.fitToAsset(assets, rects[i].w, rects[i].h);
		rData.dest.setCenter(rects[i].cX(), rects[i].cY());
		rData.dest += dragData.mRect.topLeft();
		CircleData({ selected[i] ? SELECT_COLOR : CIRCLE_COLOR,
			SDL_BLENDMODE_BLEND, rData.boundary }).
			set(rData.dest.center(), rects[i].w / 2, 2, true).render(assets);
		assets.drawTexture(rData);
	}
}

void Spinner::redraw() {
	int size = items.size();
	rects.resize(size);

	if (itemW == 0 || size == 0) { return; }

	double scrollFrac = dragData.scroll / itemW;
	// This is the starting item
	int idx = (int)(scrollFrac + .5);
	if (idx < 0 || idx > size) { return; }

	const double xRad = dragData.mRect.w / 2;
	const double dw = -itemW / 2;
	const double cX = dragData.mRect.w / 2, cY = dragData.mRect.h / 2;
	const int w = itemW;
	auto wAt = [w, dw, xRad](double x) {
		return w + dw * std::abs(x) / xRad;
	};
	auto nextX = [w, dw, xRad](double x, int sign) {
		sign = std::copysign(1, sign);
		double top = 2 * x + sign * w, bot = 2 - sign * dw / xRad;
		// If they are opposite signs, adjust the denominator
		if (top * bot < 0) { bot = 2 + sign * dw / xRad; }
		return top / bot;
	};

	for (int sign : {1, -1}) {
		double x = (idx - scrollFrac) * itemW;
		double currW = wAt(x);
		int i = idx;
		while (i >= 0 && i < size
			&& x - currW < xRad && x + currW > -xRad) {
			rects[i] = Rect(0, 0, (int)currW, (int)currW);
			rects[i].setCenter(cX + x, cY);
			// Add half our width plus 1/10 for margin
			x = nextX(x + sign * currW * .6, sign);
			currW = wAt(x);
			i += sign;
		}
	}
}

/*void Spinner::add(const Asset& asset) {
	items.push_back(asset);
	updateMe = true;
}

void Spinner::remove(const std::string& id) {
	auto it = items.find(id);
	if (it != items.end()) { items.erase(it); }
	updateMe = true;
}*/

void Spinner::set(const std::vector<Asset>& vec) {
	items = vec;
	selected = std::vector<bool>(items.size(), false);
	dragData.maxScroll = itemW * (items.size() - 1);
	redraw();
}

void Spinner::clear() {
	items.clear();
	selected.clear();
	dragData.reset();
	updateMe = true;
}

int Spinner::size() const {
	return items.size();
}

void Spinner::setSelected(int idx, bool select) {
	if (idx >= 0 && idx < selected.size()) {
		selected[idx] = select;
	}
}

void Spinner::setRect(const Rect& r) {
	dragData.mRect = r;
	itemW = dragData.mRect.h * 7 / 8;
}

void Spinner::setBackground(const SDL_Color& color) {
	bkgrnd = color;
}
