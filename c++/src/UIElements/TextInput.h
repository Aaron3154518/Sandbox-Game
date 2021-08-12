#ifndef TEXT_INPUT_H
#define TEXT_INPUT_H

#include <sstream>
#include <functional>

#include "../Definitions.h"
#include "UIElement.h"
#include "../Utils/Utils.h"
#include "../Utils/Rect.h"
#include "../Utils/AssetManager.h"

class TextInput : public UIElement {
public:
	TextInput() = default;
	~TextInput() = default;

	bool handleEvents(Event& e);
	void draw();

	void setRect(Rect rect);
	void setBackground(const SDL_Color& c) { bkgrnd = c; }
	void setTextData(const TextData& data) { textData = data; }
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

	static const int CURSOR_DELAY;
};

#endif