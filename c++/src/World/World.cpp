#include "World.h"
#include "../GameObjects.h"

// For testing write during save
//int cntr = 600;

// World
World::~World() {
	fr.close();
	fw.discard();
}

void World::setFile(std::string fName) {
	fr.close(); fw.discard();
	fileName = fName;
	if (!isFile(fileName)) {
		time = 0;
		dim = { 10,10 };
		numBlocks = dim.x * dim.y;
		spawn = { 5,5 };
		newWorld();
		blocks[0][5] = Block{ tile::Id::STONE };
		saveWorld();
	}
	loadWorld();
	printInfo(true);
}

void World::newWorld() {
	blocks.clear();
	blockChanges.clear();
	// Allocate blocks
	std::vector<Block> row(dim.x);
	blocks.resize(dim.y, row);
	row.clear();
}

void World::printInfo(bool printBlocks) const {
	std::cerr << std::endl;
	std::cerr << "World Type: " << type << std::endl;
	std::cerr << "Can Delete: " << (canDelete ? "True" : "False") << std::endl;
	std::cerr << "Number of Blocks: " << numBlocks << std::endl;
	std::cerr << "Dimensions: " << dim.x << " x " << dim.y << std::endl;
	std::cerr << "Spawn Point: " << "(" << spawn.x << ", " << spawn.y << ")" << std::endl;
	if (printBlocks) {
		std::cerr << "Blocks:" << std::endl;
		for (auto& row : blocks) {
			for (auto& block : row) {
				std::cerr << block.id << " ";
			}
			std::cerr << std::endl;
		}
	}
	std::cerr << std::endl;
}

void World::tick(Timestep& dt) {
	time = (time + dt.milliseconds() * 100) % MS_PER_DAY;
	// Run auto save
	if (!saving()) { nextSave -= dt.seconds(); }
	if (saving()) {
		saveProgress = saveWorld(saveProgress);
		if (saveProgress >= 1) {
			saveProgress = 0.;
			nextSave = SAVE_DELAY;
			// Player.write();
		}
	}
	// Update minimap

	// Update chunks
	//manager.tick();

	// Update dropped items
	PlayerAccess& player = GameObjects::player();
	for (auto it = droppedItems.begin(); it != droppedItems.end(); it++) {
		it->move(dt);
		if (!it->pickUpImmune() && player.pickUp(*it)) {
			it = droppedItems.erase(it);
			if (it == droppedItems.end()) { break; }
		}
	}
}

// File saving/loading
// These should use blocks[][] directly
void World::loadWorld() {
	std::cerr << "Loading World" << std::endl;
	if (fw.isOpen()) {
		std::cerr << "World::loadWorld(): Cannot open file for reading - "
			<< "file is currently open for writing" << std::endl;
		return;
	}
	// Open the file
	fr.close();
	if (!fr.open(fileName)) {
		std::cerr << "World::loadWorld(): Unable to open world file for reading" << std::endl;
		return;
	}
	loadInfo();
	loadBlocks(0., dim.y);
	drawWorld(0.);
	if (!fr.close()) {
		std::cerr << "World::loadWorld(): File Close Error" << std::endl;
	}
}

double World::loadWorld(double progress) {
	if (progress == 0) {
		std::cerr << "Loading World" << std::endl;
		if (fw.isOpen()) {
			std::cerr << "World::loadWorld(): Cannot open file for reading - "
				<< "file is currently open for writing" << std::endl;
			return 1.;
		}
		// Open the file
		fr.close();
		if (!fr.open(fileName)) {
			std::cerr << "World:loadWorld(): Unable to open world file for reading" << std::endl;
			return 1.;
		}
		loadInfo();
	}
	progress *= 2;
	if (progress < 1.) { progress = loadBlocks(progress, 3) / 2.; }
	else { progress = drawWorld(progress - 1) / 2. + .5; }
	if (progress >= 1.) {
		if (!fr.close()) {
			std::cerr << "World::loadWorld(): File Close Error" << std::endl;
		}
		//manager.setup();
		//manager.load_all();
	}
	return progress;
}

void World::loadInfo() {
	// We are changing world information, stop any reading/writing
	fr.close();
	fw.discard();
	if (!fr.open(fileName)) {
		std::cerr << "World::loadInfo(): unable to open file" << std::endl;
		return;
	}
	// Load file data
	fr.read(canDelete);
	fr.read(type);
	fr.read(dim);
	fr.read(spawn);
	fr.read(time);
	if (dim.x < 0 || dim.y < 0) {
		std::cerr << "Invalid World Dimensions" << std::endl;
		return;
	}
	if (spawn.x < 0 || spawn.y < 0 || spawn.x >= dim.x || spawn.y >= dim.y) {
		std::cerr << "Invalid Spawn Point" << std::endl;
		spawn = { (int)(dim.x / 2), (int)(dim.y / 2) };
	}
	numBlocks = dim.x * dim.y;
	newWorld();
}

// TODO: load tile data
double World::loadBlocks(double progress, int numRows) {
	if (!fr.isOpen()) {
		std::cerr << "World::loadBlocks(): File not open" << std::endl;
		return 1.;
	}
	// If no data, return early to avoid divide by zero
	if (dim.x <= 0 || dim.y <= 0) { return 1.; }
	// Get current row, column, and blocks left to save
	const int startRow = (int)(progress * dim.y);
	for (int row = startRow; row < startRow + numRows && row < dim.y; row++) {
		int col = 0;
		while (col < dim.x) {
			// Extract tile id and number of tiles
			tile::Id val;
			fr.read(val);
			size_t cnt;
			fr.read(cnt);
			// Add blocks
			for (int i = col; i < col + cnt;) {
				Block& b = blocks[row][i];
				b.id = val;
				// If not multiblock part
				if (b.src == 0) {
					// Create multiblock
					// TODO: get tile dimensions
					SDL_Point bDim{ 1, 1 };
					for (int dy = 0; dy < bDim.y; dy++) {
						for (int dx = 0; dx < bDim.x; dx++) {
							Block& b2 = blocks[row + dy][i + dx];
							b2 = b;
							b2.setSrc(dx, dy);
						}
					}
					i += bDim.x;
				} else { i++; }
			}
			col += cnt;
		}
	}
	return (double)(startRow + numRows) / dim.y;
	/*// Get current row, column, and blocks left to load
	int currentBlock = (int)(progress * numBlocks + .5 / numBlocks);
	int col = currentBlock % dim.x, row = (int)(currentBlock / dim.x);
	int blocksLeft = (int)ceil(numBlocks / 100);
	// Write data to array
	while (blocksLeft > 0) {
		// Extract tile id and number of tiles
		tile::Id val;
		readFile(file, &val);
		int num;
		readFile(file, &num);
		// TODO: Tile defaults to air if it doesn't exist
		// Make sure we don't go over the row
		if (col + num > dim.x) { num = dim.x - col; }
		if (val != tile::Id::AIR) {
			for (int i = 0; i < num; i++) {
				blocks[row][col + i] = val;
			}
			// TODO: Everything with the tile objects
			tile = game_vars.tiles[val]
				// If the block has a width > 1, there is automatically only one block in this strip
				if tile.dim[0] != 1 :
			// Save multiblock parts
			for dr in range(tile.dim[1]) :
				for dc in range(tile.dim[0]) :
					blocks[row + dr][col + dc] = -(dc * 100 + dr)
					blocks[row][col] = val
					// Otherwise, it is either a single block or a vertical multiblock
				else:
			// Load single block(top left)
			blocks[row][col:col + num] = [val] * num
				// Load vertical multiblock
				for i in range(1, tile.dim[1]) :
					blocks[row + i, col:col + num] = [-i] * num
					// Loop through every block
					for col1 in range(col, col + num) :
						// Add the block to applicable lists
						add_block(col1, row, val)
						// Load extra data if the block has any
						if tile.has_data :
							amnt = int.from_bytes(f_obj.read(2), byteorder)
							c.update_dict(col1, row, f_obj.read(amnt), block_data)
							
		}
		// Update our column and blocks left
		blocksLeft -= num;
		col += num;
		// Check if we made it to the next row
		if (col >= dim.x) {
			col %= dim.x;
			row += 1;
			// Check if we hit the end of the map
			if (row >= dim.y) { return 1.; }
		}
	}
	// Return our progress
	return (double)(row * dim.x + col) / numBlocks;*/
}

double World::drawWorld(double progress) {
	if (!fr.isOpen()) {
		std::cerr << "World::drawWorld(): File not open" << std::endl;
		return 1.;
	}

	return 1.;
}

void World::saveWorld() {
	std::cerr << "Saving World" << std::endl;
	if (fr.isOpen()) {
		std::cerr << "World::saveWorld(): Cannot open file for writing - "
			<< "file is currently open for reading" << std::endl;
		return;
	}
	// Open the file
	fw.discard();
	if (!fw.open(fileName)) {
		std::cerr << "World::saveWorld(): Unable to open world file for writing" << std::endl;
		return;
	}
	applyBlockChanges();
	// Save world info
	saveInfo();
	saveBlocks(0., dim.y);
	saveMap(0.);
	if (!fw.commit()) {
		std::cerr << "World::saveWorld(): File Close Error" << std::endl;
	}
	applyBlockChanges();
	std::cerr << "Successfully Saved" << std::endl;
}

double World::saveWorld(double progress) {
	if (progress == 0) {
		std::cerr << "Saving World" << std::endl;
		if (fr.isOpen()) {
			std::cerr << "World::saveWorld(): Cannot open file for writing - "
				<< "file is currently open for reading" << std::endl;
			return 1.;
		}
		// Open the file
		fw.discard();
		if (!fw.open(fileName)) {
			std::cerr << "World::saveWorld(): Unable to open world file for writing" << std::endl;
			return 1.;
		}
		applyBlockChanges();
		// Save world info
		saveInfo();
	}
	progress *= 2;
	if (progress < 1.) { progress = saveBlocks(progress, 3) / 2.; }
	else { progress = saveMap(progress - 1.) / 2. + .5; }
	if (progress >= 1.) {
		if (!fw.commit()) {
			std::cerr << "World::saveWorld(): File Close Error" << std::endl;
		}
		applyBlockChanges();
		std::cerr << "Successfully Saved" << std::endl;
	}
	return progress;
}

// TODO: Verion info
void World::saveInfo() {
	if (!fw.isOpen()) {
		std::cerr << "World::saveInfo(): File not open" << std::endl;
		return;
	}
	// Write data
	fw.write(canDelete);
	fw.write(type);
	fw.write(dim);
	fw.write(spawn);
	fw.write(time);
}

double World::saveBlocks(double progress, int numRows) {
	if (!fw.isOpen()) {
		std::cerr << "World::saveBlocks(): File not open" << std::endl;
		return 1.;
	}
	// For testing write during save
	//if (progress >= .5 && cntr-- > 0) { return progress; }
	// If no data, return early to avoid divide by zero
	if (dim.x <= 0 || dim.y <= 0) { return 1.; }
	// Get current row, column, and blocks left to save
	const int startRow = toInt(progress * dim.y);
	tile::Id val;
	size_t cnt;
	int row;
	for (row = startRow; row < startRow + numRows && row < dim.y; row++) {
		// Save the tile id
		val = blocks[row][0].id;
		cnt = 0;
		// Iterate through this row
		for (int col = 0; col < dim.x; col++) {
			// If the block changes, save id and count
			if (blocks[row][col].id != val) {
				fw.write(val);
				fw.write(cnt);
				val = blocks[row][col].id;
				cnt = 0;
			}
			cnt++;
		}
		fw.write(val);
		fw.write(cnt);
		/*// Write data
		f_obj.write(val.to_bytes(2, byteorder))
			f_obj.write(num_byte)
			// Write any extra data
			if game_vars.tiles[val].has_data :
				// Write it for each block
				for x in range(col, col1) :
					// We have to write the correct number of bytes no matter what
					// Bad block data is better than bad world data
					bytes_ = c.get_from_dict(x, row, block_data)
					// Create a new bytearray
					if bytes_ is None :
		f_obj.write(bytearray(2))
					else:
		f_obj.write(len(bytes_).to_bytes(2, byteorder))
			f_obj.write(bytes_)*/
	}
	return (double)row / dim.y;
}

double World::saveMap(double progress) {
	if (!fw.isOpen()) {
		std::cerr << "World::saveMap(): File not open" << std::endl;
		return 1.;
	}

	return 1.;
}

void World::applyBlockChanges() {
	// Update blocks array
	for (auto& pair : blockChanges) {
		SDL_Point loc = pair.first;
		blocks[loc.y][loc.x] = pair.second;
	}
	blockChanges.clear();
}

// Functions involving the world blocks
void World::getBlockSrc(int& x, int& y) const {
	if (x < 0 || y < 0 || x >= dim.x || y >= dim.y) { return; }
	const Block& b = blocks[y][x];
	int nX = x - b.dx(), nY = y - b.dy();
	if (nX < 0 || nY < 0 || nX >= dim.x || nY >= dim.y) { return; }
	x = nX; y = nY;
}

SDL_Point World::getBlockSrc(SDL_Point loc) const {
	getBlockSrc(loc.x, loc.y);
	return loc;
}

bool World::checkCollisions(Point<double>& pos, Point<double> dim,
	Point<double>& d) const {
	dim.x *= gameVals::BLOCK_W; dim.y *= gameVals::BLOCK_W;
	pos.x += d.x; pos.y += d.y;
	forceInWorld(pos, dim);
	return false;
}

bool World::touchingBlocks(const Point<double>& pos, const Point<double>& dim,
	bool x, bool topLeft) const {
	float p1, p2, d1, d2;
	if (x) { p1 = pos.x; p2 = pos.y; d1 = dim.x; d2 = dim.y;  }
	else { p1 = pos.y; p2 = pos.x; d1 = dim.y; d2 = dim.x; }
	float l1 = d1 * gameVals::BLOCK_W, l2 = d2 * gameVals::BLOCK_W;
	// Check if we are actually touching a new block
	if ((int)fabsf(p1 + (topLeft ? 0 : l1)) % gameVals::BLOCK_W == 0) {
		// Get the next x block
		int next = (int)(topLeft ? (p1 / gameVals::BLOCK_W) - 1 :
			ceilf((p1 + l1) / gameVals::BLOCK_W));
		// Check if we are going to the world edge
		if (topLeft ? next < 0 : next >= d1) { return true; }
		// Otherwise check if there is a solid block
		else {
			Point<int> range{(int)(p2 / gameVals::BLOCK_W),
				(int)ceilf((p2 + l2) / gameVals::BLOCK_W)};
			return x ? anySolidBlocks(next, next + 1, range.x, range.y) :
				anySolidBlocks(range.x, range.y, next, next + 1);
		}
	}
	return false;
}

bool World::anySolidBlocks(int x1, int x2, int y1, int y2) const {
	if (x1 < 0) { x1 = 0; }
	if (y1 < 0) { y1 = 0; }
	if (x2 >= dim.x) { x2 = dim.x - 1; }
	if (y2 >= dim.y) { y2 = dim.y - 1; }
	for (; y1 < y2; y1++) {
		for (; x1 < x2; x1++) {
			TilePtr tile = Tile::getTile(blocks[y1][x1].id);
			if (tile->getTileData(Tile::TileData::barrier)) { return true; }
		}
	}
	return false;
}

void World::forceInWorld(Point<double>& p, Point<double> _dim) const {
	if (p.x < 0) { p.x = 0.; }
	if (p.y < 0) { p.y = 0.; }
	SDL_Point bDim = { dim.x * gameVals::BLOCK_W, dim.y * gameVals::BLOCK_W };
	if (p.x > bDim.x - _dim.x) { p.x = bDim.x - _dim.x; }
	if (p.y > bDim.y - _dim.y) { p.y = bDim.y - _dim.y; }
}
void World::forceInWorld(SDL_Point& p, Point<double> _dim) const {
	if (p.x < 0) { p.x = 0; }
	if (p.y < 0) { p.y = 0; }
	if (p.x > dim.x - _dim.x) { p.x = dim.x - _dim.x; }
	if (p.y > dim.y - _dim.y) { p.y = dim.y - _dim.y; }
}

bool World::isInWorld(SDL_Point loc) const {
	return loc.x >= 0 && loc.x < dim.x&& loc.y >= 0 && loc.y < dim.y;
}

int World::surfaceH() const { return (int)(dim.y / 2); }
int World::underground() const { return (int)(dim.y * 2 / 3); }
SDL_Color World::skyColor() const {
	return SDL_Color{ 0,0,(uint8_t)
		(255 * (1 - std::abs(((double)time - NOON) / NOON))) };
}

// Visual functions
SDL_Point World::getWorldMousePos(SDL_Point mouse, SDL_Point center,
	bool blocks) const {
	SDL_Point screenC = { (int)(UI::width() / 2), (int)(UI::height() / 2) };
	SDL_Point worldC = getScreenRect(center).center();
	SDL_Point result = { mouse.x - screenC.x + worldC.x,
		mouse.y - screenC.y + worldC.y };
	return blocks ? getBlockPos(result) : result;
}

Rect World::getScreenRect(SDL_Point center) const {
	int w = UI::width(), h = UI::height();
	int worldW = width(), worldH = height();
	Rect r(0, 0, w, h);
	if (worldW >= w) {
		r.setCenterX(center.x);
		if (r.x < 0) { r.x = 0; }
		else if (r.x2() > worldW) { r.setX2(worldW); }
	} else {
		r.x = (int)((worldW - w) / 2);
	}
	if (worldH >= h) {
		r.setCenterY(center.y);
		if (r.y < 0) { r.y = 0; }
		else if (r.y2() > worldH) { r.setY2(worldH); }
	} else {
		r.y = (int)((worldH - h) / 2);
	}
	return r;
}

// Static functions
const Block& World::airBlock() {
	const static Block AIR_BLOCK;
	return AIR_BLOCK;
}

Block World::createBlock(tile::Id tileId, uint8_t dx, uint8_t dy) {
	Block b;
	b.id = tileId;
	b.setSrc(dx, dy);
	initializeBlock(b);
	return b;
}

void World::initializeBlock(Block& b) {
	b.data.reset();
	TilePtr tile = Tile::getTile(b.id);
	b.spawner = tile->getTileData(Tile::TileData::spawner);
	b.crafter = tile->getTileData(Tile::TileData::crafting);
}

// Getters/Setters
const Block& World::getBlock(int x, int y) const {
	return getBlock(SDL_Point{ x,y });
}
const Block& World::getBlock(SDL_Point loc) const {
	if (!isInWorld(loc)) { return airBlock(); }
	if (saving()) {
		auto it = blockChanges.find(loc);
		if (it != blockChanges.end()) { return it->second; }
	}
	return blocks[loc.y][loc.x];
}
void World::setBlock(int x, int y, const Block& b) {
	setBlock(SDL_Point{ x,y }, b);
}
void World::setBlock(SDL_Point loc, const Block& b) {
	if (isInWorld(loc)) {
		if (saving()) { blockChanges[loc] = b; }
		else { blocks[loc.y][loc.x] = b; }
	}
}
void World::setBlockData(int x, int y, ByteArray& data) {
	setBlockData(SDL_Point{ x,y }, data);
}
void World::setBlockData(SDL_Point loc, ByteArray& data) {
	if (isInWorld(loc)) {
		if (saving()) {
			Block b = getBlock(loc);
			b.data = data;
			blockChanges[loc] = b;
		} else { blocks[loc.y][loc.x].data = data; }
	}
}

void World::dropItem(const DroppedItem& drop, DroppedItem::DropDir dir) {
	dropItem(drop, dir, GameObjects::player().getCPosf());
}
void World::dropItem(const DroppedItem& drop, DroppedItem::DropDir dir,
	Point<double> pos) {
	droppedItems.push_back(drop);
	droppedItems.back().drop(pos, dir);
}

void World::drawDroppedItems(const Rect& worldRect) const {
	SDL_Point tl = worldRect.topLeft();
	AssetManager& assets = UI::assets();
	for (const DroppedItem& drop : droppedItems) {
		Rect r = drop.getRect() + tl;
		assets.drawTexture(drop.getInfo().getImage(), r);
	}
}

// WorldAccess
// Update world blocks
bool WorldAccess::placeBlock(SDL_Point loc, tile::Id tileId) {
	TilePtr tile = Tile::getTile(tileId);
	// Calculate block rectangle
	Point<uint8_t> tDim = tile->getDim();
	SDL_Point dim = getDim();
	if (loc.x < 0 || loc.y < 0 ||
		loc.x + tDim.x > dim.x || loc.y + tDim.y > dim.y) {
		return false;
	}
	Rect bRect(loc.x * gameVals::BLOCK_W, loc.y * gameVals::BLOCK_W,
		tDim.x * gameVals::BLOCK_W, tDim.y * gameVals::BLOCK_W);
	if (/*!handler.collidesWithEntity(bRect) && */tile->canPlace(loc)) {
		for (uint8_t dy = 0; dy < tDim.y; dy++) {
			for (uint8_t dx = 0; dx < tDim.x; dx++) {
				int x = loc.x + dx, y = loc.y + dy;
				Block b = createBlock(tileId, dx, dy);
				setBlock(loc, b);
			}
		}
		// manager.blockChange(loc, tileId, true);
		tile->onPlace(loc);
		return true;
	}
	return false;
}

bool WorldAccess::breakBlock(SDL_Point loc) {
	if (!isInWorld(loc)) { return false; }
	loc = getBlockSrc(loc);
	Block b = getBlock(loc);
	if (b.id != tile::Id::AIR) {
		TilePtr tile = Tile::getTile(b.id);
		if (tile->onBreak(loc)) {
			// Destroy all parts
			Point<uint8_t> tDim = tile->getDim();
			SDL_Point dim = getDim();
			if (loc.x + tDim.x > dim.x || loc.y + tDim.y > dim.y) {
				return false;
			}
			for (uint8_t dy = 0; dy < tDim.y; dy++) {
				for (uint8_t dx = 0; dx < tDim.x; dx++) {
					int x = loc.x + dx, y = loc.y + dy;
					setBlock(x, y, airBlock());
				}
			}
			// manager.blockChange(loc, tileId, false);
			// TODO: Drop items
			return true;
		}
	}
	return false;
}

void WorldAccess::draw(SDL_Point center) {
	AssetManager& assets = UI::assets();

	int worldW = width(), worldH = height();
	Rect screen = getScreenRect(center);
	// Where on the screen to draw the world
	Rect worldRect(std::abs(screen.x), std::abs(screen.y),
		std::min(screen.w, worldW), std::min(screen.h, worldH));
	int lbX = (int)(std::max(screen.x, 0) / gameVals::BLOCK_W);
	int ubX = (int)ceil(std::min(screen.x2(), worldW) / gameVals::BLOCK_W);
	int lbY = (int)(std::max(screen.y, 0) / gameVals::BLOCK_W);
	int ubY = (int)ceil(std::min(screen.y2(), worldH) / gameVals::BLOCK_W);
	// Draw blocks
	assets.rect(&worldRect, skyColor());
	Rect r(lbX * gameVals::BLOCK_W - screen.x, lbY * gameVals::BLOCK_W - screen.y,
		gameVals::BLOCK_W, gameVals::BLOCK_W);
	for (int row = lbY; row < ubY; row++) {
		for (int col = lbX; col < ubX; col++) {
			tile::Id id = getBlock(col, row).id;
			if (id != tile::Id::AIR) {
				SDL_Texture* tex = Tile::getTile(id)->getImage({ col, row });
				assets.drawTexture(tex, r);
			}
			r.x += gameVals::BLOCK_W;
		}
		r.x = lbX * gameVals::BLOCK_W - screen.x;
		r.y += gameVals::BLOCK_W;
	}
	// Draw dropped items
	drawDroppedItems(worldRect);
	// Draw border
	assets.thickRect(worldRect, 2, BLACK);
	// Draw player
	SDL_Texture* playerTex = assets.getAsset(
		gameVals::entities() + "player_pig.png");
	r = Rect::getMinRect(playerTex, gameVals::BLOCK_W, gameVals::BLOCK_W * 2);
	r.setCenter(center - screen.topLeft());
	assets.drawTexture(playerTex, r);
}

void WorldAccess::drawLight(const Rect& rect) {}