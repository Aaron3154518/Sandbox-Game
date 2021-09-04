#include "TextInput.h"

constexpr int CURSOR_DELAY = 500;
constexpr double TEXT_H_FRAC = .8;

TextInput::TextInput() {
	textData.x = textData.y = textData.w = textData.h = 0;
	textData.xMode = TextData::PosType::topleft;
	textData.yMode = TextData::PosType::center;
	textData.color = WHITE;
}

bool TextInput::handleEvents(Event& e) {
	time += e.dt.milliseconds();
	time = time % (2 * CURSOR_DELAY);
	if (e.checkAndHandleKey(SDLK_BACKSPACE, Event::ButtonStatus::PRESSED)) {
		std::string str = currInput.str();
		currInput.str(str.substr(0, str.length() - 1));
		currInput.seekp(0, std::ios_base::end);
	} else {
		addInput(e.inputText);
	}
	return false;
}

void TextInput::draw() {
	AssetManager& assets = Window::Get().assets();
	assets.rect(&mRect, bkgrnd);

	SharedFont font = assets.getFont(textData);
	if (!font) { return; }

	textData.text = currInput.str();

	if (textData.text.empty() && !hintText.empty()) {
		Uint8 prevA = textData.color.a;
		textData.color.a = 128;
		textData.text = hintText;

		TextureData drawData = assets.renderText(textData);
		drawData.boundary = mRect;
		assets.drawTexture(drawData);

		textData.color.a = prevA;
		textData.text = "";
	}

	// Text dimensions
	int w = 0, h = 0;
	TTF_SizeText(font.get(), textData.text.c_str(), &w, &h);
	// Cursor dimensions
	int cW = 0, cH = 0;
	TTF_SizeText(font.get(), "|", &cW, &cH);
	// Adjust to real size
	double factor = (double)textData.h / h;
	cW *= factor;
	cH *= factor;
	w = w * factor + cW;

	if (w <= mRect.w && textData.xMode == TextData::PosType::topleft) {
		assets.drawText(textData);
	} else {
		// Set left
		TextData::PosType horizAlign = textData.xMode;
		textData.xMode = TextData::PosType::topleft;
		if (horizAlign == TextData::PosType::center) {
			textData.x = mRect.cX() - (int)(w / 2);
		} else {
			textData.x = mRect.x2() - w;
		}
		// Draw
		TextureData drawData = assets.renderText(textData);
		drawData.boundary = mRect;
		assets.drawTexture(drawData);
		// Reset alignment
		textData.xMode = horizAlign;
		textData.setPos(mRect);
	}
	if (time >= CURSOR_DELAY) {
		int x = mRect.x + std::min(w, mRect.w) - std::max(cW * 3 / 4, 1);
		int y = mRect.y + ((mRect.h - cH) / 2);
		Rect r(x, y, cW / 2, cH);
		assets.rect(&r, WHITE);
	}
}

void TextInput::setRect(Rect rect) {
	textData.h = rect.h * TEXT_H_FRAC;
	mRect = rect;
	textData.setPos(rect);
}

void TextInput::setBackgroundColor(const SDL_Color& c) {
	bkgrnd = c;
}

void TextInput::setTextColor(const SDL_Color& c) {
	textData.color = c;
}

void TextInput::setHint(const std::string& hint) {
	hintText = hint;
}

void TextInput::setFont(const std::string& id) {
	textData.fontId = id;
	textData.useFont = false;
}

void TextInput::setFont(SharedFont font) {
	textData.setFont(font);
}

void TextInput::setXMode(TextData::PosType mode) {
	textData.xMode = mode;
}

void TextInput::setYMode(TextData::PosType mode) {
	textData.yMode = mode;
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