#include "Player.h"
#include "../GameObjects.h"

const double Player::PICKUP_DX = 1.5, Player::PICKUP_DY = 1.5;
const double Player::PLACE_DX = 2.5, Player::PLACE_DY = 2;
const std::string Player::IMG = gameVals::entities() + "player_pig.png";

Player::Player() {
	// Setup rectangles
	mTex.textureId = IMG;
	cursorTex.dest = Rect(0, 0, gameVals::ITEM_W(), gameVals::ITEM_W());
	SharedTexture tex = Window::Get().assets().getAsset(IMG);
	mRect = Rect::getMinRect(tex.get(), (int)(gameVals::BLOCK_W() * 3 / 2), 0);
	dim = { (double)mRect.w / gameVals::BLOCK_W(),
		(double)mRect.h / gameVals::BLOCK_W() };
	spriteRect = Rect::getMinRect(tex.get(), gameVals::SPRITE_W(), gameVals::SPRITE_W());
	armRect = Rect(0, 0, (int)(mRect.w / 4), (int)(mRect.h / 3));
	setPos(Point<double>{0, 0});

	// Set gravity
	a.y = 10;

	// Setup inventory
	inventory.drawInventory();
	inventory.setPos(SDL_Point{ 0,0 });
}

// Update functions
void Player::handleEvents(Event& e) {
	SDL_Point dim = Window::Get().screenDim();
	Rect screenR(0, 0, dim.x, dim.y);
	// If we are dead, update respawn counter
	if (respawnCtr > 0) {
		respawnCtr -= e.dt.milliseconds();
		if (respawnCtr <= 0) { respawn(); }
	// Otherwise handle events
	} else if (mapOpen) {
		e.nextUI(screenR, true);
		if (e.checkAndHandleKey(SDLK_m, Event::ButtonStatus::RELEASED)) {
			std::cerr << "Close Map" << std::endl;
			mapOpen = false;
			// map.setCenter(mRect.cX() / gameVals::BLOCK_W(), mRect.cY() / gameVals::BLOCK_W());
			// map.zoom = 1;
		}
	} else {
		/*// When we open the inventory, check if crafting should open as well
		if (inventory.open && craftingOpen) {
			if (!activeUI) {
				setActiveUI(craftingUI);
			}
		// Otherwise make sure crafting is closed
		} else if (activeUI == craftingUI) { clearActiveUI(); }*/

		// Check active ui
		/*
		// Send events to the active ui
		if (activeUI) {
			activeUI.tick();
			// TODO: Dragging
			activeUI.handleEvents(e);
		}
		// Drag active ui
		if (e.right.pressed &&
			(e.keyDown(SDLK_LSHIFT) || e.keyDown(SDLK_RSHIFT))) {
			std::cerr << "Start Dragging UI" << std::endl;
			if (activeUI && activeUI.canDrag &&
				SDL_PointInRect(&pos, &activeUI.rect)) {
				// dragging = true
			}
		}
		*/

		// Check crafting ui

		// Inventory
		e.nextUI(screenR, false);
		inventory.handleEvents(e);

		// Player
		e.nextUI(screenR, false);

		// Map Toggle
		if (e.checkAndHandleKey(SDLK_m, Event::ButtonStatus::RELEASED)) {
			std::cerr << "Open Map" << std::endl;
			mapOpen = true;
			// map.setCenter(mRect.cX() / gameVals::BLOCK_W(), mRect.cY() / gameVals::BLOCK_W());
			// map.zoom = 1;
		}
		// Minimap zoom
		if (e.checkAndHandleKey(SDLK_EQUALS, Event::ButtonStatus::HELD)) {
			std::cerr << "Minimap Zoom In" << std::endl;
			/* map.zoom += e.dt.milliseconds * 10;
			if(map.zoom > 5) { map.zoom = 5; }
			*/
		}
		if (e.checkAndHandleKey(SDLK_MINUS, Event::ButtonStatus::HELD)) {
			std::cerr << "Minimap Zoom Out" << std::endl;
			/* map.zoom -= e.dt.milliseconds * 10;
			if(map.zoom < 1) { map.zoom = 1; }
			*/
		}

		// Handle movement
		if (canMove) {
			// acc = stats.getStat("acceleration");
			double acc = 3;
			a.x = 0;
			if (e.checkAndHandleKey(SDLK_a, Event::ButtonStatus::HELD)) {
				a.x -= acc;
			}
			if (e.checkAndHandleKey(SDLK_d, Event::ButtonStatus::HELD)) {
				a.x += acc;
			}
			// Check for jump
			if (collided.y == CollideType::botRight
				&& e.checkAndHandleKey(SDLK_SPACE, Event::ButtonStatus::HELD)) {
				v.y = -6; // -stats.getStat("jump_speed");
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

	// Add drag force
	if (v.x != 0 && a.x * v.x <= 0) {
		double newA = -4 * v.x;
		if (fabsf(newA) > fabsf(a.x)) { a.x = newA; }
	}

	// Get displacement
	Point<double> d;
	for (Dim _d : getDimList()) {
		// Calculate displacement
		d[_d] = gameVals::BLOCK_W() * sec * (v[_d] + a[_d] * sec / 2);
		// Calculate velocity
		v[_d] += a[_d] * sec;
		double maxV = 5;
		if (fabsf(v[_d]) > maxV) {
			v[_d] = std::copysignf(maxV, v[_d]);
		}
	}

	Point<double> prevD = d, prevP = pos;
	// Check for collisions and set new position
	bool didHit = GameObjects::world().checkCollisions(pos, dim, d);
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
			if (GameObjects::world().touchingBlocks(
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
		if (fallDist > 10 * gameVals::BLOCK_W()) {
			hit((int)(fallDist / gameVals::BLOCK_W()) - 10, 0, 0);
		}
		fallDist = 0;
	}
}

void Player::draw() {
	Rect r = GameObjects::world().getScreenRect(mRect.center());
	SDL_Point shift = r.topLeft();
	AssetManager& assets = Window::Get().assets();

	// Player image
	mTex.dest = mRect - shift;
	assets.drawTexture(mTex);

#ifdef PLAYER_DEBUG
	assets.thickRect(pickUpRange - shift, 1,
		AssetManager::BorderType::outside, GREEN);
	assets.thickRect(placementRange - shift, 1,
		AssetManager::BorderType::outside, RED);
#endif

	// Inventory
	inventory.draw();

	drawUI();

	// Draw held item
	ItemInfo mouseItem = inventory.getHeldItem();
	if (mouseItem.isItem()) {
		cursorTex.texture = mouseItem.getImage();
		cursorTex.dest.setCenter(mousePos());
		assets.drawTexture(cursorTex);
	}
}

void Player::drawUI() {

}

bool Player::placeBlock(SDL_Point worldPos, tile::Id tileId) {
	SDL_Point loc = GameObjects::world().toBlockPos(worldPos);
	TilePtr tile = Tile::getTile(tileId);
	Point<uint8_t> tDim = tile->getDim();
	// Can't place block over player
	if (SDL_PointInRect(&worldPos, &placementRange)
		&& !collidesPlayer(Rect(loc.x, loc.y, tDim.x, tDim.y))) {
		// Check if we can place the block
		return GameObjects::world().placeBlock(loc, tileId);
	}
	return false;
}

bool Player::breakBlock(SDL_Point worldPos) {
	SDL_Point loc = GameObjects::world().toBlockPos(worldPos);
	WorldAccess& world = GameObjects::world();
	// Make sure we aren't hitting air
	loc = world.getBlockSrc(loc);
	const Block& block = world.getBlock(loc);
	if (block.id == tile::Id::AIR) { return false; }
	TilePtr tile = Tile::getTile(block.id);
	// Make sure this block doesn't have an open UI
	//if (activeUI && activeUI.blockPos == loc) { return false; }
	// Make sure this block is in range and check if we destroyed it
	int power = 1; // stats.getStat("power");
	if (SDL_PointInRect(&worldPos, &placementRange)
		&& tile->hit(loc, power)) {
		return world.breakBlock(loc);
	}
	return false;
}

void Player::setFile(std::string fName) {
	fr.close(); fw.discard();
	fileName = fName;
	if (!isFile(fileName)) { save(); }
	load();
}

void Player::load() {
	std::cerr << "Loading Player" << std::endl;
	if (fw.isOpen()) {
		std::cerr << "Player::read(): Cannot open file for reading - "
			<< "file is currently open for writing" << std::endl;
		return;
	}
	// Open the file
	fr.close();
	if (!fr.open(fileName)) {
		std::cerr << "Player::read(): Unable to open player file for reading" << std::endl;
		return;
	}

	// Read inventory
	inventory.read(fr);

	if (!fr.close()) {
		std::cerr << "Player::read(): File Close Error" << std::endl;
	}
}

void Player::save() {
	std::cerr << "Saving Player" << std::endl;
	if (fr.isOpen()) {
		std::cerr << "Player::write(): Cannot open file for writing - "
			<< "file is currently open for reading" << std::endl;
		return;
	}
	// Open the file
	fw.discard();
	if (!fw.open(fileName)) {
		std::cerr << "Player::write(): Unable to open player file for writing" << std::endl;
		return;
	}

	// Write inventory
	inventory.write(fw);

	if (!fw.commit()) {
		std::cerr << "Player::write(): File Close Error" << std::endl;
	}	
}

void Player::saveNewPlayer(IO& io) {
	PlayerInventory inv;
	inv.write(io);
}

// Inventory stuff
bool Player::pickUp(DroppedItem& drop) {
	drop.setPulled(false);
	if (rectsOverlap(drop.getRect(), pickUpRange)) {
		ItemInfo& item = drop.getInfo();
		if (inventory.isSpaceForItem(item)) {
			drop.attract(Point<double>{mRect.cX(), mRect.cY()});
			if (distance(mRect.center(), drop.getRect().center()) <= 1) {
				return inventory.pickUpItem(item);
			}
		}
	}
	return false;
}

// Other functions
Point<double> Player::getCPosf() const {
	return Point<double>{mRect.cX(), mRect.cY()};
}

void Player::setPos(const Point<double>& newPos) {
	pos = newPos;
	mRect.setTopLeft(SDL_Point{ (int)pos.x, (int)pos.y });
	pickUpRange.w = mRect.w + 2 * PICKUP_DX * gameVals::BLOCK_W();
	pickUpRange.h = mRect.h + 2 * PICKUP_DY * gameVals::BLOCK_W();
	pickUpRange.setCenter(mRect.center());
	placementRange.w = mRect.w + 2 * PLACE_DX * gameVals::BLOCK_W();
	placementRange.h = mRect.h + 2 * PLACE_DY * gameVals::BLOCK_W();
	placementRange.setCenter(mRect.center());
}

bool Player::collidesPlayer(Rect blockRect) const {
	Rect r = toBlockRect(mRect);
	return rectsOverlap(r, blockRect);
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