#ifndef SPINNER_H
#define SPINNER_H

#include <map>
#include <vector>

#include "UIElement.h"
#include "../Definitions.h"
#include "../Utils/AssetManager.h"
#include "../Utils/Point.h"
#include "../Utils/Rect.h"
#include "../Utils/Utils.h"
#include "../Window.h"

class Spinner : public UIElement {
public:
	Spinner() = default;
	~Spinner() = default;

	int mouseOnItem();
	bool handleEvents(Event& e);
	void draw();

	void set(const std::vector<Asset>& vec);
	void clear();
	int size() const;

	void setSelected(int idx, bool select);

	void setRect(const Rect& r);
	void setBackground(const SDL_Color& color);

private:
	Rect mRect;
	SDL_Color bkgrnd = TRANSPARENT;

	bool dragging = false, moved = false, updateMe = false;
	int itemW = 1;
	double scroll = 0., scrollV = 0.;

	std::vector<Asset> items;
	std::vector<Rect> rects;
	std::vector<bool> selected;

	void redraw();
};

#endif