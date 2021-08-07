#ifndef TEXT_INPUT_H
#define TEXT_INPUT_H

#include <sstream>
#include <functional>

#include "../Definitions.h"
#include "../Utils/Utils.h"
#include "../Utils/UI.h"
#include "../Utils/Rect.h"
#include "../Utils/AssetManager.h"

class TextInput {
public:
	TextInput() = default;
	~TextInput() = default;

	void handleEvents(Event& e);
	void render();

	void setRect(Rect rect);
	void setBackground(const SDL_Color& c) { bkgrnd = c; }
	void setTextData(const TextData& data) { textData = data; }
	void setCharConstraint(const std::function<bool(const char& ch)>& chFunc);

	std::string getInput() { return currInput.str(); }
	void clearInput() { currInput.str(""); }
	void addInput(const std::string& str);
private:
	Rect mRect;
	TextData textData;
	SDL_Color bkgrnd = BLACK;

	Uint32 time;
	std::ostringstream currInput;
	std::function<bool(const char& ch)> validChar = [](const char& ch) { return true; };

	static const int CURSOR_DELAY;
};

#endif