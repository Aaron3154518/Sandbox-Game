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
	void setFont(std::string id);
	void setFont(SharedFont font);
	void setXMode(TextData::PosType mode);
	void setYMode(TextData::PosType mode);
	void setCharConstraint(const std::function<bool(const char& ch)>& chFunc);

	std::string getInput() { return currInput.str(); }
	void clearInput() { currInput.str(""); }
	void addInput(const std::string& str);
private:
	TextData textData;
	SDL_Color bkgrnd = BLACK;

	Uint32 time;
	std::ostringstream currInput;
	std::function<bool(const char& ch)> validChar = [](const char& ch) { return true; };
};

#endif