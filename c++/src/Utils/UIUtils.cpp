#include "UIUtils.h"

constexpr double MIN_SCROLL_V = .005;

void DragData::reset() {
	dragging = moved = false;
	scroll = scrollV = maxScroll = 0.;
}

bool DragData::isDragging() const {
	return dragging || moved;
}

bool DragData::drag(Event& e) {
	int prevS = scroll;
	if (dragging) {
		if (any8(e[Event::Mouse::LEFT], Event::Button::RELEASED)) {
			dragging = false;
		} else if (e.mouseDx != 0) {
			scroll -= e.mouseDx;
			if (scroll < 0) {
				scroll = 0;
			}
			if (maxScroll != -1 && scroll > maxScroll) {
				scroll = maxScroll;
			}
			moved = true;
		}
	} else if (moved) { moved = false; }

	if (SDL_PointInRect(&e.mouse, &mRect)
		&& any8(e[Event::Mouse::LEFT], Event::Button::PRESSED)) {
		dragging = true;
		moved = false;
	}
	return prevS != scroll;
}

bool DragData::dragV(Event& e) {
	int prevS = scroll;
	if (scrollV != 0.) {
		scroll += scrollV * e.dt.milliseconds();
		if (scroll < 0) {
			scroll = scrollV = 0;
		}
		if (maxScroll != -1 && scroll > maxScroll) {
			scroll = maxScroll;
			scrollV = 0;
		}
		scrollV /= std::pow(2, e.dt.seconds());
		if (std::abs(scrollV) < MIN_SCROLL_V) { scrollV = 0; }
	}

	bool prev = dragging;
	drag(e);
	// Stopped dragging
	if (prev && !dragging) {
		scrollV = -e.mouseDx / e.dt.milliseconds();
	// Started dragging
	} else if (!prev && dragging) {
		scrollV = 0.;
	}
	return prevS != scroll;
}