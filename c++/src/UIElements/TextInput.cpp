#include "TextInput.h"

constexpr int CURSOR_DELAY = 500;
constexpr double TEXT_H_FRAC = .8;

TextInput::TextInput() {
	textData.color = WHITE;
	updateMe = true;
}

bool TextInput::handleEvents(Event& e) {
	time += e.dt.milliseconds();
	time = time % (2 * CURSOR_DELAY);
	if (any8(e[SDLK_BACKSPACE], Event::Button::PRESSED)) {
		std::string str = currInput.str();
		currInput.str(str.substr(0, str.length() - 1));
		currInput.seekp(0, std::ios_base::end);
		e.handleKey(SDLK_BACKSPACE);
		updateMe = true;
	} else {
		addInput(e.inputText);
	}
	return false;
}

void TextInput::draw() {
	AssetManager& assets = Window::Get().assets();

	SharedFont font = textData.getFont(assets);;
	if (!font) { return; }

	// Text dimensions
	int w = 0, h = 0;
	TTF_SizeText(font.get(), textData.text.c_str(), &w, &h);
	// Cursor dimensions
	int cW = 0, cH = 0;
	TTF_SizeText(font.get(), "|", &cW, &cH);
	// Adjust to real size
	double factor = TEXT_H_FRAC * mRect.h / h;
	cW *= factor; cH *= factor;
	w = w * factor + cW;

	if (updateMe) {
		rData.asset.setTexture(
			Window::Get().assets().createTexture(mRect.w, mRect.h));
		assets.setRenderTarget(rData.asset.texture.get());
		assets.rect(NULL, bkgrnd);

		RenderData data;
		data.boundary = Rect(0, 0, mRect.w, mRect.h);
		Rect textR(0, 0, mRect.w - cW, mRect.h);

		textData.text = currInput.str();

		// If there's no text, draw the hint text
		if (textData.text.empty() && !hintText.empty()) {
			Uint8 prevA = textData.color.a;
			textData.color.a = 128;
			textData.text = hintText;

			data.asset.setTexture(assets.renderText(textData));
			data.fitToAsset(assets, 0, mRect.h * TEXT_H_FRAC);
			data.dest.setPos(textR, xMode, yMode);

			textData.color.a = prevA;
			textData.text = "";
		} else {
			data.asset.setTexture(assets.renderText(textData));
			data.fitToAsset(assets, 0, mRect.h * TEXT_H_FRAC);
			data.dest.setPos(textR, xMode, yMode);
			data.dest.setX2(std::min(data.dest.x2(), mRect.w - cW));
			w = data.dest.x2() + cW;
		}

		assets.drawTexture(data);
		assets.resetRenderTarget();

		updateMe = false;
	}

	assets.drawTexture(rData);

	if (time >= CURSOR_DELAY) {
		Rect r(0, 0, cW / 2, cH);
		r.setCY(mRect.cY());
		r.setCX(mRect.x + std::min(w, mRect.w) - std::max(cW / 2, 1));
		assets.rect(&r, WHITE);
	}
}

void TextInput::setRect(Rect rect) {
	mRect = rData.dest = rData.boundary = rect;
	updateMe = true;
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
	textData.setFontId(id);
	textData.font.reset();
}

void TextInput::setFont(SharedFont font) {
	textData.setFont(font);
}

void TextInput::setXMode(Rect::PosType mode) {
	xMode = mode;
}

void TextInput::setYMode(Rect::PosType mode) {
	yMode = mode;
}

void TextInput::setCharConstraint(
	const std::function<bool(const char& ch)>& chFunc) {
	validChar = chFunc;
}

void TextInput::addInput(const std::string& str) {
	for (char ch : str) {
		if (validChar(ch)) {
			currInput << ch;
			updateMe = true;
		}
	}
}