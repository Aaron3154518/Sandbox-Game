#ifndef UI_H
#define UI_H

#include <iostream>
#include <queue>
#include <memory>

#include <SDL.h>

#include "../Definitions.h"

// Forward Declarations
class Event;
class AssetManager;

class UI {
public:
	// Only 'real' functions of the class
	void run();
	virtual void tick(Event& e) { std::cerr << "Wrong Tick" << std::endl; }

	static void init();
	static void clean();
	static void resize(int nW, int nH) { w = nW; h = nH; }

	static void setDrawColor(const SDL_Color& c);
	static void resetDrawColor();

	static void setRenderTarget(SDL_Texture* tex);
	static void resetRenderTarget();

	static UI* getActive() { return uiQueue.empty() ? NULL : uiQueue.front(); }
	

	static int width();
	static int height();
	static SDL_Renderer* renderer();
	static AssetManager& assets();
protected:
	Uint32 gameTime = 0;
	bool running = true;

	static void addUI(UI* ui, bool owner) { uiQueue.push(ui); ownership.push(owner); }
private:
	static bool initialized;
	static int maxH, maxW, w, h;
	static SDL_Renderer* mRenderer;
	static SDL_Window* mWindow;
	static AssetManager mAssetManager;

	// Used by main to run stuff
	static std::queue<UI*> uiQueue;
	static std::queue<bool> ownership;
};

#endif