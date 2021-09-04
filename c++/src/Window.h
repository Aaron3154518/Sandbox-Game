#ifndef WINDOW_H
#define WINDOW_H

#include <deque>
#include <iostream>
#include <memory>
#include <vector>

#include <SDL.h>
#include <SDL_ttf.h>

#include "Definitions.h"
#include "UIs/UI.h"
#include "Utils/AssetManager.h"

class Window {
	friend int main(int argc, char* argv[]);
public:
	typedef std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> WindowPtr;

	static Window& Get();

	// Getters
	SDL_Point screenDim() const;
	SDL_Renderer* renderer();
	AssetManager& assets();

private:
	Window();
	~Window();
	Window(const Window& other) = delete;
	Window& operator=(const Window& other) = delete;

	WindowPtr mWindow = WindowPtr(NULL, SDL_DestroyWindow);

	AssetManager mAssetManager;

	void run();

	// Run every ui in activeUIs
	std::deque<std::shared_ptr<UI>> activeUIs;
};

#endif