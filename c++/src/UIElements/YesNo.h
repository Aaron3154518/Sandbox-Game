#ifndef YES_NO_H
#define YES_NO_H

#include <type_traits>

#include <SDL.h>
#include <SDL_ttf.h>

#include "../Definitions.h"
#include "../UIs/UI.h"
#include "UIElement.h"
#include "../Utils/Utils.h"
#include "../Utils/Rect.h"
#include "../Utils/AssetManager.h"

class YesNo : public UIElement {
public:
	YesNo() = default;
	YesNo(std::string _prompt);
	~YesNo();

	bool handleEvents(Event& e);
	void draw();

	void setRect(Rect rect);

	void setPrompt(std::string str) { prompt = str; setRect(mRect); }
	bool getAnswer() const { return answer; }

private:
	bool answer = false;
	std::string prompt = "No prompt provided";
	int scroll = 0, maxScroll = 0, scrollAmnt = 1;
	Rect promptRect, promptRectFull, yesRect, noRect;
	SDL_Texture* mTex = NULL;
	SDL_Texture* promptTex = NULL;

	const static std::string YES_IMG, NO_IMG;
};

#endif