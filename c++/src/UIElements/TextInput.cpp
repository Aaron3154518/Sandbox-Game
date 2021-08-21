#include "TextInput.h"

const int TextInput::CURSOR_DELAY = 500;

bool TextInput::handleEvents(Event& e) {
	time += e.dt.milliseconds();
	time = time % (2 * CURSOR_DELAY);
	if (e.keyPressed(SDLK_BACKSPACE)) {
		std::string str = currInput.str();
		currInput.str(str.substr(0, str.length() - 1));
		currInput.seekp(0, std::ios_base::end);
	} else {
		addInput(e.inputText);
	}
	return false;
}

void TextInput::draw() {
	UI::setDrawColor(bkgrnd);
	SDL_RenderFillRect(UI::renderer(), &mRect);
	UI::resetDrawColor();

	SharedFont font = UI::assets().getFont(textData);
	if (!font) { return; }
	bool cursor = time >= CURSOR_DELAY;
	std::string text = currInput.str();
	if (!cursor) { textData.text = text; }
	// Width of the text with the cursor
	int w = 0;
	TTF_SizeText(font.get(), text.append("|").c_str(), &w, NULL);
	if (cursor) { textData.text = text; }

	TextData::PosType horizAlign = textData.xMode;
	if (w <= mRect.w && horizAlign == TextData::PosType::topleft) {
		UI::assets().drawText(textData, &mRect);
	} else {
		// Set left
		textData.xMode = TextData::PosType::topleft;
		if (horizAlign == TextData::PosType::center) {
			textData.x = mRect.cX() - (int)(w / 2);
		} else {
			textData.x = mRect.x2() - w;
		}
		// Draw
		UI::assets().drawText(textData, &mRect);
		// Reset alignment
		textData.xMode = horizAlign;
		textData.getPosFromRect(mRect);
	}
}

void TextInput::setRect(Rect rect) {
	mRect = rect;
	textData.getPosFromRect(rect);
}

void TextInput::setCharConstraint(
	const std::function<bool(const char& ch)>& chFunc) {
	validChar = chFunc;
}

void TextInput::addInput(const std::string& str) {
	for (char ch : str) {
		if (validChar(ch)) { currInput << ch; }
	}
}