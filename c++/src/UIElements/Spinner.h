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

	bool handleEvents(Event& e, SDL_Point parentPos = { 0,0 });
	void draw(SDL_Point parentPos = { 0,0 });

	void set(const std::vector<Asset>& vec);
	void clear();

	void setRect(const Rect& r);
	void setBackground(const SDL_Color& color);

private:
	Rect mRect;
	SDL_Color bkgrnd = TRANSPARENT;

	bool dragging = false, updateMe = false;
	int selected = -1, itemW = 1;
	double scroll = 0., scrollV = 0.;

	std::vector<Asset> items;
	std::vector<Rect> rects;

	void redraw();
};

#endif