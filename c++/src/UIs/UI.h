#ifndef UI_H
#define UI_H

#include <iostream>
#include <vector>
#include <deque>
#include <memory>

#include <SDL.h>

#include "../Definitions.h"
#include "../Utils/Utils.h"

// Forward Declarations
class Event;
class AssetManager;

class UI {
	friend int ::main(int argc, char* argv[]);
public:
	UI() = default;
	~UI() = default;

	// Functions for running the UI
	virtual void runUI();
	virtual void initUI() {}
	virtual void tickUI(Event& e) {}

	static void resize(int nW, int nH) { w = nW; h = nH; }

	// Helper functions for rendering
	static void setDrawColor(const SDL_Color& c);
	static void resetDrawColor();
	static void setRenderTarget(SDL_Texture* tex);
	static void resetRenderTarget();

	// Getters
	static int width();
	static int height();
	static SDL_Renderer* renderer();
	static AssetManager& assets();
protected:
	Uint32 gameTime = 0;
	bool running = false, textInput = false;

	// nextUis - All UIs to run after the current one
	// subUIs - All UIs to run after the current one but then
	//        - return to the current one
	std::vector<std::shared_ptr<UI>> nextUIs, subUIs;

	//std::shared_ptr<UI> addNextUI(UI* ui);
	//std::shared_ptr<UI> addSubUI(UI* ui);
private:
	static bool initialized;
	static int maxH, maxW, w, h;
	static SDL_Renderer* mRenderer;
	static SDL_Window* mWindow;
	static AssetManager mAssetManager;

	static void init();
	static void clean();
	static void run();

	// Main will run every ui in active
	static std::deque<std::shared_ptr<UI>> activeUIs;
};

#endif