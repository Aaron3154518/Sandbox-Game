#include "Spinner.h"

constexpr double MIN_SCROLL_V = .005;

bool Spinner::handleEvents(Event& e, SDL_Point parentPos) {
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
			redraw();
		}
		return true;
	}

	SDL_Point mouse = e.mouse - parentPos;
	if (SDL_PointInRect(&mouse, &mRect)
		&& any8(e[Event::Mouse::LEFT], Event::Button::PRESSED)) {
		scrollV = 0.;
		dragging = true;
		return true;
	}
	return false;
}

void Spinner::draw(SDL_Point parentPos) {
	if (rects.size() != items.size()) { redraw(); }
	AssetManager& assets = Window::Get().assets();
	Rect r = mRect + parentPos;
	if (bkgrnd != TRANSPARENT) { assets.rect(&r, bkgrnd); }
	TextureData texData;
	texData.boundary = r;
	for (int i = 0; i < items.size() && i < rects.size(); i++) {
		texData.useTexture = items[i].useTexture;
		texData.textureId = items[i].assetId;
		texData.texture = items[i].texture;
		texData.dest = Window::Get().assets().getMinRect(items[i].assetId,
			rects[i].w, rects[i].h);
		texData.dest.setCenter(rects[i].cX(), rects[i].cY());
		texData.dest += r.topLeft();
		assets.thickCircle(texData.dest.center(), rects[i].w / 2, 2,
			AssetManager::BorderType::middle, { 0,255,0,128 },
			SDL_BLENDMODE_BLEND);
		assets.drawTexture(texData);
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

/*std::string Spinner::insert(const Asset& asset) {
	static constexpr int ID_LEN = 4;
	std::string id;
	do {
		id = randomString(ID_LEN);
	} while (items.find(id) != items.end());
	items[id] = asset;
	updateMe = true;
	return id;
}

void Spinner::remove(const std::string& id) {
	auto it = items.find(id);
	if (it != items.end()) { items.erase(it); }
	updateMe = true;
}*/

void Spinner::set(const std::vector<Asset>& vec) {
	items = vec;
	redraw();
}

void Spinner::clear() {
	items.clear();
	updateMe = true;
}

void Spinner::setRect(const Rect& r) {
	mRect = r;
	itemW = mRect.h * 7 / 8;
}

void Spinner::setBackground(const SDL_Color& color) {
	bkgrnd = color;
}
