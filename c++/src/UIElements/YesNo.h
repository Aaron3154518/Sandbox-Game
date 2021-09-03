#ifndef YES_NO_H
#define YES_NO_H

#include <SDL.h>
#include <SDL_ttf.h>

#include "Button.h"
#include "UIElement.h"
#include "../Definitions.h"
#include "../Utils/AssetManager.h"
#include "../Utils/Rect.h"
#include "../Utils/Utils.h"

class YesNo : public UIElement {
public:
	YesNo();
	YesNo(std::string _prompt) : prompt(_prompt) {}
	~YesNo() = default;

	bool handleEvents(Event& e);
	void draw();

	void setRect(Rect rect);

	void setPrompt(std::string str) { prompt = str; setRect(mRect); }
	bool getAnswer() const { return answer; }

private:
	bool answer = false;
	std::string prompt = "No prompt provided";
	int scroll = 0, maxScroll = 0, scrollAmnt = 1;
	TextureData promptTex;
	Button yesButton, noButton;

	const static std::string YES_IMG, NO_IMG;
};

#endif