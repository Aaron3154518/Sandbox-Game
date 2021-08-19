#include "Player.h"
#include "../GameObjects.h"

Player::Player() {
	// Setup rectangles
	SDL_Texture* tex = UI::assets().getAsset(fullImgFile());
	mRect = Rect::getMinRect(tex, gameVals::BLOCK_W * 3 / 2, -1);
	dim = { (double)mRect.w / gameVals::BLOCK_W,
		(double)mRect.h / gameVals::BLOCK_W };
	spriteRect = Rect::getMinRect(tex, gameVals::SPRITE_W, gameVals::SPRITE_W);
	armRect = Rect(0, 0, (int)(mRect.w / 4), (int)(mRect.h / 3));
	collectionRange = Rect(0, 0, 6 * gameVals::BLOCK_W, 6 * gameVals::BLOCK_W);
	placementRange = Rect(0, 0, 7 * gameVals::BLOCK_W, 7 * gameVals::BLOCK_W);

	// Set gravity
	a.y = 10;
}

void Player::tick(Event& e) {
	// If we are dead, update respawn counter
	if (respawnCtr > 0) {
		respawnCtr -= e.dt.milliseconds();
		if (respawnCtr <= 0) { respawn(); }
	// Otherwise handle events
	} else {
		/*// When we open the inventory, check if crafting should open as well
		if (inventory.open && craftingOpen) {
			if (!activeUI) {
				setActiveUI(craftingUI);
			}
		// Otherwise make sure crafting is closed
		} else if (activeUI == craftingUI) { clearActiveUI(); }*/

		/*// Send events to the active ui
		if (activeUI) {
			activeUI.tick();
			// TODO: Dragging
			activeUI.handleEvents(e);
		}*/

		// Check events
		if (useTime <= 0 && e.scroll != 0) {
			// Scroll the hotbar
			std::cerr << "Scroll Hotbar" << std::endl;
			//inventory.scroll(e.scroll > 0);
		} else if (e.right.clicked &&
			(e.keyDown(SDLK_LSHIFT) || e.keyDown(SDLK_RSHIFT))) {
			std::cerr << "Start Dragging UI" << std::endl;
			/*if (activeUI && activeUI.canDrag &&
				SDL_PointInRect(&pos, &activeUI.rect)) {
				// dragging = true
			}*/
		} /* else if (e.keyPressed(SDLK_ESCAPE) && activeUI && activeUI != craftingUI) {
			activeUI.onExit();
			resetActiveUI();
		}*/ else if (e.keyReleased(SDLK_m)) {
			mapOpen = true;
			// map.setCenter(mRect.cX() / gameVals::BLOCK_W, mRect.cY() / gameVals::BLOCK_W);
			// map.zoom = 1;
		}

		// Check map zoom
		if (e.keyDown(SDLK_RIGHT)) {
			/* map.zoom += e.dt.milliseconds * 10;
			if(map.zoom > 5) { map.zoom = 5; }
			*/
		}
		if (e.keyDown(SDLK_LEFT)) {
			/* map.zoom -= e.dt.milliseconds * 10;
			if(map.zoom < 1) { map.zoom = 1; }
			*/
		}

		/*// Check for using an item
		if (useTime <= 0 &&
			(!activeUI || !SDL_PointInRect(&e.mouse, &actievUI.rect))) {
			if (e.left.clicked) {
				leftClick();
			} else {
				firstSwing = true;
				if (e.right.clicked) {
					rightClick();
				} else {
					inventory.holdingR = 0;
				}
			}
		}*/

		// If using an item, let it deal with use time
		if (itemUsed != item::Id::numItems) {
/*			ItemPtr = Item::getItem(itemUsed);
			ItemPtr->onTick();
			if (useTime <= 0) { itemUsed = item::Id::numItems; }*/
		// Otherwise handle use time ourselves
		} else if (useTime >= 0) {
			useTime -= e.dt.milliseconds();
		}

		// Handle movement
		if (canMove) {
			// acc = stats.getStat("acceleration");
			double acc = 3;
			// If both keys are pressed, don't change acceleration
			if (e.keyDown(SDLK_a) && !e.keyDown(SDLK_d)) { a.x = -acc; }
			if (e.keyDown(SDLK_d) && !e.keyDown(SDLK_a)) { a.x = acc; }
			// Check for jump
			if (e.keyDown(SDLK_SPACE) && collided.y == CollideType::botRight) {
				v.y = -6;// -stats.getStat("jump_speed");
			}
			move(e.dt);
		}

		if (immunity > 0) { immunity -= e.dt.milliseconds(); }
	}
}

void Player::move(Timestep dt) {
	uint32_t ms = dt.milliseconds();
	if (ms == 0) { return; }
	double sec = dt.seconds();

	// Get displacement
	Point<double> d;
	for (Dim _d : getDimList()) {
		// Calculate displacement
		d[_d] = gameVals::BLOCK_W * sec * (v[_d] + a[_d] * sec / 2);
		// Calculate velocity
		v[_d] += a[_d] * sec;
		double maxV = 5;
		if (fabsf(v[_d]) > maxV) {
			v[_d] = std::copysignf(maxV, v[_d]);
		}
	}

	// Add drag force
	if (v.x != 0) { a.x = -4 * v.x; }

	Point<double> prevD = d, prevP = pos;
	// Check for collisions and set new position
	bool didHit = GameObjects::GetWorld().checkCollisions(pos, dim, d);
	setPos(pos);

	// Get actual change in position
	d = { pos.x - prevP.x, pos.y - prevP.y };
	// Update collisions
	for (Dim _d : getDimList()) {
		// If we didn't move the full distance, we hit something
		if (!eq(d[_d], prevD[_d])) {
			collided[_d] = prevD[_d] > d[_d] ?
				CollideType::botRight : CollideType::topLeft;
		// Didn't move, get collision based on accelleration
		} else if (d[_d] == 0 && prevD[_d] == 0 && a[_d] != 0) {
			std::cerr << "Here" << std::endl;
			if (GameObjects::GetWorld().touchingBlocks(
				pos, dim, _d == Dim::x, a[_d] < 0)) {
				collided[_d] = a[_d] < 0 ? CollideType::topLeft :
					CollideType::botRight;
			} else { collided[_d] = CollideType::none; }
		} else {
			collided[_d] = CollideType::none;
		}
		// Update velocity
		if (collided[_d] != CollideType::none) { v[_d] = 0; }
	}

	// Check if we are off the ground
	if (collided.y == CollideType::none) {
		// If we are falling, add to our fall distance
		fallDist = v.y > 0 ? fallDist + d.y : 0;
	// Check if we hit the ground
	} else if (collided.y == CollideType::botRight) {
		// If the fall distance was great enough, deal damage
		if (fallDist > 10 * gameVals::BLOCK_W) {
			hit((int)(fallDist / gameVals::BLOCK_W) - 10, 0, 0);
		}
		fallDist = 0;
	}
}

void Player::draw() {
	Rect r = GameObjects::GetWorld().getScreenRect(mRect.center());
	AssetManager& assets = UI::assets();

	Rect shiftRect = mRect - r.topLeft();
	assets.drawTexture(fullImgFile(), shiftRect);

	drawUI();
}

void Player::drawUI() {

}

void Player::setPos(const Point<double>& newPos) {
	pos = newPos;
	mRect.setTopLeft(SDL_Point{ (int)pos.x, (int)pos.y });
	collectionRange.setCenter(mRect.center());
	placementRange.setCenter(mRect.center());
}

void Player::hit(int damage, int centerX, int kbPower) {
	int defense = 0;
	damage = std::max(0, damage - defense);
	int hp = 1;
	//stats.hp -= damage;
	// addDamageText(dmg, mRect.center());
	if (hp <= 0) {
		respawnCtr = 5000;
		mapOpen = false;
	} else if (kbPower != 0) {
		immunity = 1000;
		v.x = kbPower * std::copysign(1, mRect.cX() - centerX);
		v.y = -3;
	}

}

void Player::respawn() {}

std::string Player::fullImgFile() const {
	return gameVals::entities() + img;
}