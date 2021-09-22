#include "Spinner.h"

constexpr double MIN_SCROLL_V = .005;
constexpr int KEY_LEN = 4;

constexpr SDL_Color CIRCLE_COLOR{ 0,255,0,128 };
constexpr SDL_Color SELECT_COLOR{ 0,0,255,128 };

int Spinner::mouseOnItem() {
	SDL_Point mouse = mousePos();
	if (SDL_PointInRect(&mouse, &mRect)) {
		mouse -= mRect.topLeft();
		for (int i = 0; i < rects.size(); i++) {
			if (SDL_PointInRect(&mouse, &rects[i])) {
				return i;
			}
		}
	}
	return -1;
}

bool Spinner::handleEvents(Event& e) {
	if (scrollV != 0.) {
		scroll += scrollV * e.dt.milliseconds();
		scrollV /= std::pow(2, e.dt.seconds());
		if (std::abs(scrollV) < MIN_SCROLL_V) { scrollV = 0; }
		redraw();
	}
	
	if (dragging) {
		if (any8(e[Event::Mouse::LEFT], Event::Button::RELEASED)) {
			scrollV = -e.mouseDx / e.dt.milliseconds();
			dragging = false;
		} else if (e.mouseDx != 0) {
			scroll -= e.mouseDx;
			moved = true;
			redraw();
		}
		return moved;
	}

	if (SDL_PointInRect(&e.mouse, &mRect)
		&& any8(e[Event::Mouse::LEFT], Event::Button::HELD)) {
		scrollV = 0.;
		dragging = true;
		moved = false;
		return true;
	}
	return false;
}

void Spinner::draw() {
	if (rects.size() != items.size()) { redraw(); }
	AssetManager& assets = Window::Get().assets();
	if (bkgrnd != TRANSPARENT) {
		RectData({ bkgrnd }).set(mRect).render(assets);
	}
	RenderData rData;
	rData.boundary = mRect;
	for (int i = 0; i < rects.size(); i++) {
		rData.asset = items[i];
		rData.fitToAsset(assets, rects[i].w, rects[i].h);
		rData.dest.setCenter(rects[i].cX(), rects[i].cY());
		rData.dest += mRect.topLeft();
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

	double scrollFrac = scroll / itemW;
	if (scrollFrac < 0 || scrollFrac > size - 1) {
		scrollFrac = scrollFrac < 0 ? 0 : size - 1;
		scroll = scrollFrac * itemW;
		scrollV = 0.;
	}
	// This is the starting item
	int idx = (int)(scrollFrac + .5);

	const double xRad = mRect.w / 2;
	const double dw = -itemW / 2;
	const double cX = mRect.w / 2, cY = mRect.h / 2;
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
	redraw();
}

void Spinner::clear() {
	items.clear();
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
	mRect = r;
	itemW = mRect.h * 7 / 8;
}

void Spinner::setBackground(const SDL_Color& color) {
	bkgrnd = color;
}
