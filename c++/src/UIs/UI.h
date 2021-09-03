#ifndef UI_H
#define UI_H

#include <deque>
#include <iostream>
#include <memory>
#include <vector>

#include <SDL.h>

#include "../Definitions.h"
#include "../Utils/AssetManager.h"
#include "../Utils/Event.h"
#include "../Utils/Utils.h"
#include "../Window.h"

class UI {
	friend class Window;
public:
	UI() = default;
	~UI() = default;

	// Functions for running the UI
	virtual void runUI();
	virtual void initUI() {}
	virtual void tickUI(Event& e) {}

protected:
	bool running = false, textInput = false;

	// nextUis - All UIs to run after the current one
	// subUIs - All UIs to run after the current one but then
	//        - return to the current one
	std::vector<std::shared_ptr<UI>> nextUIs, subUIs;
};

#endif