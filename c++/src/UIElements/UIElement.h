#ifndef UI_ELEMENT_H
#define UI_ELEMENT_H

#include "../Definitions.h"
#include "../Utils/Utils.h"
#include "../Utils/Rect.h"

class UIElement {
public:
	UIElement() = default;
	~UIElement() = default;

	virtual bool handleEvents(Event& e) { mActive = false; return true; }
	virtual void draw() {}

	virtual void setActive(bool val) { mActive = val; }
	bool active() const { return mActive; }

	virtual void setRect(Rect rect) { mRect = rect; }
	Rect getRect() const { return mRect; }

protected:
	bool mActive = false;
	Rect mRect;
};

#endif