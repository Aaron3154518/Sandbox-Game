#ifndef TEXT_INPUT_H
#define TEXT_INPUT_H

#include <sstream>
#include <functional>

#include "UIElement.h"
#include "../Definitions.h"
#include "../Utils/AssetManager.h"
#include "../Utils/Event.h"
#include "../Utils/Rect.h"
#include "../Utils/Utils.h"
#include "../Window.h"

class TextInput : public UIElement {
public:
	TextInput();
	~TextInput() = default;

	bool handleEvents(Event& e);
	void draw();

	void setRect(Rect rect);
	void setBackgroundColor(const SDL_Color& c);
	void setTextColor(const SDL_Color& c);
	void setHint(const std::string& hint);
	void setFont(const std::string& id);
	void setFont(SharedFont font);
	void setXMode(Rect::PosType mode);
	void setYMode(Rect::PosType mode);
	void setCharConstraint(const std::function<bool(const char& ch)>& chFunc);

	std::string getInput() { return currInput.str(); }
	void clearInput() { currInput.str(""); updateMe = true; }
	void addInput(const std::string& str);
private:
	std::string hintText = "";
	TextData textData;
	RenderData rData;
	Rect::PosType xMode = Rect::PosType::pTopLeft,
		yMode = Rect::PosType::pCenter;
	SDL_Color bkgrnd = BLACK;
	bool updateMe = true;

	Uint32 time;
	std::ostringstream currInput;
	std::function<bool(const char& ch)> validChar =
		[](const char& ch) { return true; };
};

#endif