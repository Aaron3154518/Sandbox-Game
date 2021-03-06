#ifndef UI_ELEMENT_H
#define UI_ELEMENT_H

#include "../Definitions.h"
#include "../Utils/Event.h"
#include "../Utils/Rect.h"

class UIElement {
public:
	UIElement() = default;
	~UIElement() = default;

	// Return true when something of note happens
	virtual bool handleEvents(Event& e) { mActive = false; return true; }
	virtual void draw() {}

	virtual void setActive(bool val) { mActive = val; }
	bool active() const { return mActive; }

	virtual void setRect(const Rect& rect) { mRect = rect; }
	Rect getRect() const { return mRect; }

protected:
	bool mActive = false;
	Rect mRect;
};

#endif