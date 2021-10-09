#ifndef UI_UTILS_H
#define UI_UTILS_H

#include <limits>

#include <SDL.h>

#include "Event.h"
#include "Rect.h"
#include "Utils.h"

struct DragData {
	Rect mRect;
	bool dragging = false, moved = false;
	double scroll = 0., scrollV = 0., maxScroll = 0.;

	// Resets the data structure
	void reset();
	// Returns true if currently dragging
	bool isDragging() const;
	// Returns true if scroll changes
	bool drag(Event& e);
	bool dragV(Event& e);
};

#endif