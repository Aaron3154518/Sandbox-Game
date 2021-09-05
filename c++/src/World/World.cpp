#include "World.h"
#include "../UIs/Game.h"

//#define DEBUG_WORLD

const std::string FIRST_WORLD = "Earth";

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
		saveWorld();
	}
	loadWorld();
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
			Game::Player().save();
		}
	}
	// Update minimap

	// Update chunks
	//manager.tick();

	// Update dropped items
	PlayerAccess& player = Game::Player();
	for (auto it = droppedItems.begin(); it != droppedItems.end(); it++) {
		it->move(dt);
		if (!it->pickUpImmune() && player.pickUp(*it)) {
			it = droppedItems.erase(it);
			if (it == droppedItems.end()) { break; }
		}
	}
}

void World::reset() {
	blockChanges.clear();
	droppedItems.clear();

	nextSave = SAVE_DELAY;
	saveProgress = 0.;
}

// World loading
void World::read(IO& io) {
	loadInfo(io);
	loadBlocks(io, 0., dim.y);
	loadMap(io, 0.);
}

void World::loadWorld() {
	reset();

#ifdef DEBUG_WORLD
	std::cerr << "Loading World" << std::endl;
#endif
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

	read(fr);

	if (!fr.close()) {
		std::cerr << "World::loadWorld(): File Close Error" << std::endl;
	}
}

double World::loadWorld(double progress) {
	if (progress == 0) {
		reset();

#ifdef DEBUG_WORLD
		std::cerr << "Loading World" << std::endl;
#endif
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
		loadInfo(fr);
	}
	if (!fr.isOpen()) {
		std::cerr << "World::loadWorld(): File not open" << std::endl;
		return 1.;
	}
	progress *= 2;
	if (progress < 1.) { progress = loadBlocks(fr, progress, 3) / 2.; } else { progress = loadMap(fr, progress - 1) / 2. + .5; }
	if (progress >= 1.) {
		if (!fr.close()) {
			std::cerr << "World::loadWorld(): File Close Error" << std::endl;
		}
		//manager.setup();
		//manager.load_all();
	}
	return progress;
}

void World::loadInfo(IO& io) {
	// Load file data
	io.read(canDelete);
	io.read(type);
	io.read(dim);
	io.read(spawn);
	io.read(time);
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
double World::loadBlocks(IO& io, double progress, int numRows) {
	// If no data, return early to avoid divide by zero
	if (dim.x <= 0 || dim.y <= 0) { return 1.; }
	// Get current row, column, and blocks left to save
	const int startRow = (int)(progress * dim.y);
	for (int row = startRow; row < startRow + numRows && row < dim.y; row++) {
		int col = 0;
		while (col < dim.x) {
			// Extract tile id and number of tiles
			tile::Id val;
			io.read(val);
			size_t cnt;
			io.read(cnt);
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

double World::loadMap(IO& io, double progress) {
	return 1.;
}

// World saving
void World::write(IO& io) {
	saveInfo(io);
	saveBlocks(io, 0., dim.y);
	saveMap(io, 0.);
}

void World::saveWorld() {
#ifdef DEBUG_WORLD
	std::cerr << "Saving World" << std::endl;
#endif
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
	write(fw);

	if (!fw.commit()) {
		std::cerr << "World::saveWorld(): File Close Error" << std::endl;
	}

	applyBlockChanges();
}

double World::saveWorld(double progress) {
	if (progress == 0) {
#ifdef DEBUG_WORLD
		std::cerr << "Saving World" << std::endl;
#endif
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
		saveInfo(fw);
	}
	if (!fw.isOpen()) {
		std::cerr << "World::saveWorld(): File not open" << std::endl;
		return 1.;
	}
	progress *= 2;
	if (progress < 1.) { progress = saveBlocks(fw, progress, 3) / 2.; } else { progress = saveMap(fw, progress - 1.) / 2. + .5; }
	if (progress >= 1.) {
		if (!fw.commit()) {
			std::cerr << "World::saveWorld(): File Close Error" << std::endl;
		}
		applyBlockChanges();
#ifdef DEBUG_WORLD
		std::cerr << "Successfully Saved" << std::endl;
#endif
	}
	return progress;
}

// TODO: Verion info
void World::saveInfo(IO& io) {
	// Write data
	io.write(canDelete);
	io.write(type);
	io.write(dim);
	io.write(spawn);
	io.write(time);
}

double World::saveBlocks(IO& io, double progress, int numRows) {
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
		io.write(val);
		io.write(cnt);
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

double World::saveMap(IO& io, double progress) {
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

void World::checkCollisions(Point<int> size,
	Point<double>& pos, Point<double>& d) const {
	const int BW = gameVals::BLOCK_W();
	const Point<int> wDim = toPoint(dim);
	// Break up displacement into smaller parts
	while (d.x != 0 || d.y != 0) {
		// Get distances based on traveling 1 block in the largest direction
		// This way we travel at most 1 block in any direction
		double frac = std::min(BW / std::max(std::abs(d.x), std::abs(d.y)), 1.);
		Point<double> d_{ d.x * frac, d.y * frac };
		d.x -= d_.x; d.y -= d_.y;

		// Current block - {{left, right}, {top, bottom}}
		Point<Point<int>> currBlock{ {0,0}, {0,0} };
		// Next block in the direction of travel
		Point<int> nextBlock{ 0,0 };
		// Distance to next block
		Point<double> toNext{ 0.,0. };
		for (auto idx : getDimList()) {
			// Get current and next block
			currBlock[idx].x = std::floor(pos[idx] / BW);
			currBlock[idx].y = std::ceil((pos[idx] + size[idx]) / BW) - 1;
			// Going top/left or bottom/right
			nextBlock[idx] = d_[idx] < 0 ? std::floor((pos[idx] + d_[idx])
				/ BW) : std::ceil((pos[idx] + size[idx] + d_[idx]) / BW) - 1;
			// Use top left or bottom right
			Dim idx2 = d_[idx] <= 0 ? Dim::x : Dim::y;
			// We didn't move to a new block
			if (currBlock[idx][idx2] == nextBlock[idx]) {
				pos[idx] += d_[idx];
				d_[idx] = 0.;
			// Stop at top left world boundary
			} else if (nextBlock[idx] < 0) {
				pos[idx] = d_[idx] = 0.;
			// Stop at bottom right world bondary
			} else if (nextBlock[idx] >= wDim[idx]) {
				pos[idx] = (wDim[idx] * BW) - size[idx];
				d_[idx] = 0.;
			// Move to the next block
			} else {
				toNext[idx] = (nextBlock[idx] * BW) - pos[idx]
					- (d_[idx] < 0 ? -BW : size[idx]);
			}
		}

		// We are only moving in one direction
		if (d_.x * d_.y == 0. && (d_.x != 0. || d_.y != 0.)) {
			// The Points below are set up as follows:
			// {value for moving idx, value for stationary idx}
			Point<Dim> idxs = d_.y == 0 ? (Point<Dim>) { Dim::x, Dim::y } :
				(Point<Dim>) { Dim::y, Dim::x };
			Point<int> lbs = { nextBlock[idxs.x], currBlock[idxs.y].x };
			Point<int> ubs = { lbs.x + 1, currBlock[idxs.y].y + 1 };
			// We can move freely in the moving direction
			pos[idxs.x] += anySolidBlocks(lbs[idxs.x], ubs[idxs.x],
				lbs[idxs.y], ubs[idxs.y]) ? toNext[idxs.x] : d_[idxs.x];
		// We are moving in both directions
		} else if (d_.x * d_.y != 0) {
			// Shortest time to next block
			Point<double> nextFrac{ toNext.x / d_.x, toNext.y / d_.y };
			// Shortest time direction
			Dim idx1 = nextFrac.x <= nextFrac.y ? Dim::x : Dim::y;
			// Longest time direction
			Dim idx2 = nextFrac.x <= nextFrac.y ? Dim::y : Dim::x;
			// Move in the shortest time direction first
			// as it will change blocks first
			std::vector<Point<Dim>> idxList = { {idx1, idx2}, {idx2,idx1} };
			for (Point<Dim> idxs : idxList) {
				// The Points below are set up as follows:
				// {value for moving idx, value for stationary idx}
				Point<int> lbs = { nextBlock[idxs.x], currBlock[idxs.y].x };
				Point<int> ubs = { lbs.x + 1, currBlock[idxs.y].y + 1 };
				pos[idxs.x] += anySolidBlocks(lbs[idxs.x], ubs[idxs.x],
					lbs[idxs.y], ubs[idxs.y]) ? toNext[idxs.x] : d_[idxs.x];
			}
		}
	}
}

bool World::touchingBlocks(const Point<double>& pos, const Point<double>& dim,
	bool x, bool topLeft) const {
	float p1, p2, d1, d2;
	if (x) { p1 = pos.x; p2 = pos.y; d1 = dim.x; d2 = dim.y; } else { p1 = pos.y; p2 = pos.x; d1 = dim.y; d2 = dim.x; }
	float l1 = d1 * gameVals::BLOCK_W(), l2 = d2 * gameVals::BLOCK_W();
	// Check if we are actually touching a new block
	if ((int)fabsf(p1 + (topLeft ? 0 : l1)) % gameVals::BLOCK_W() == 0) {
		// Get the next x block
		int next = (int)(topLeft ? (p1 / gameVals::BLOCK_W()) - 1 :
			ceilf((p1 + l1) / gameVals::BLOCK_W()));
		// Check if we are going to the world edge
		if (topLeft ? next < 0 : next >= d1) { return true; }
		// Otherwise check if there is a solid block
		else {
			Point<int> range{ (int)(p2 / gameVals::BLOCK_W()),
				(int)ceilf((p2 + l2) / gameVals::BLOCK_W()) };
			return x ? anySolidBlocks(next, next + 1, range.x, range.y) :
				anySolidBlocks(range.x, range.y, next, next + 1);
		}
	}
	return false;
}

bool World::anySolidBlocks(int x1, int x2, int y1, int y2) const {
	if (x1 < 0) { x1 = 0; }
	if (y1 < 0) { y1 = 0; }
	if (x2 > dim.x) { x2 = dim.x; }
	if (y2 > dim.y) { y2 = dim.y; }
	for (int y = y1; y < y2; y++) {
		for (int x = x1; x < x2; x++) {
			TilePtr tile = Tile::getTile(getBlock(x, y).id);
			if (tile->getTileData(Tile::TileData::barrier)) { return true; }
		}
	}
	return false;
}

void World::forceInWorld(Point<double>& p, Point<double> _dim) const {
	if (p.x < 0) { p.x = 0.; }
	if (p.y < 0) { p.y = 0.; }
	SDL_Point bDim = { dim.x * gameVals::BLOCK_W(), dim.y * gameVals::BLOCK_W() };
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
	SDL_Point screenC = Window::Get().screenDim() / 2;
	SDL_Point worldC = getScreenRect(center).center();
	SDL_Point result = { mouse.x - screenC.x + worldC.x,
		mouse.y - screenC.y + worldC.y };
	return blocks ? result /= gameVals::BLOCK_W() : result;
}

SDL_Point World::getWorldMousePos(SDL_Point mouse, bool blocks) const {
	return getWorldMousePos(mouse, Game::Player().getCPos(), blocks);
}

Rect World::getScreenRect(SDL_Point center) const {
	SDL_Point dim = Window::Get().screenDim();
	int worldW = width(), worldH = height();
	Rect r(0, 0, dim.x, dim.y);
	if (worldW >= dim.x) {
		r.setCX(center.x);
		if (r.x < 0) { r.x = 0; } 		else if (r.x2() > worldW) { r.setX2(worldW); }
	} else {
		r.x = (int)((worldW - dim.x) / 2);
	}
	if (worldH >= dim.y) {
		r.setCY(center.y);
		if (r.y < 0) { r.y = 0; } 		else if (r.y2() > worldH) { r.setY2(worldH); }
	} else {
		r.y = (int)((worldH - dim.y) / 2);
	}
	return r;
}

// Static functions
void World::createNewInfoFile(IO& io) {
	io.write(FIRST_WORLD);
}

void World::createNewWorld(IO& io) {
	World world;
	world.write(io);
}

bool World::createNewUniverse(const std::string& univName) {
	FileWrite fw;

	// Create info file
	if (!fw.open(gameVals::univInfoFile(univName))) {
		std::cerr << "Could not open universe info file" << std::endl;
		return false;
	}
	createNewInfoFile(fw);
	if (!fw.commit()) {
		std::cerr << "Failed to write to universe info file" << std::endl;
		return false;
	}

	// Create world file
	if (!fw.open(gameVals::worldFile(univName, FIRST_WORLD))) {
		std::cerr << "Could not open world file" << std::endl;
		return false;
	}
	createNewWorld(fw);
	if (!fw.commit()) {
		std::cerr << "Failed to write to world file" << std::endl;
		return false;
	}

	return true;
}

const Block& World::airBlock() {
	const static Block AIR_BLOCK;
	return AIR_BLOCK;
}

Block World::createBlock(tile::Id tileId, uint8_t dx, uint8_t dy) {
	Block b;
	b.id = tileId;
	b.setSrc(dx, dy);
	b.data.reset();
	return b;
}

SDL_Point World::toBlockPos(SDL_Point p) {
	SDL_Point result = p / gameVals::BLOCK_W();
	if (p.x < 0) { --result.x; }
	if (p.y < 0) { --result.y; }
	return result;
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
		if (saving()) { blockChanges[loc] = b; } else { blocks[loc.y][loc.x] = b; }
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
	dropItem(drop, dir, Game::Player().getCPosf());
}
void World::dropItem(const DroppedItem& drop, DroppedItem::DropDir dir,
	Point<double> pos) {
	droppedItems.push_back(drop);
	droppedItems.back().drop(pos, dir);
}

void World::drawDroppedItems(const Rect& worldRect) {
	SDL_Point tl = worldRect.topLeft();
	for (DroppedItem& drop : droppedItems) {
		TextureData data;
		data.dest = drop.getRect() + tl;
		data.setTexture(drop.getInfo().getImage());
		Window::Get().assets().drawTexture(data);
	}
}

// WorldAccess
// Update world blocks
bool WorldAccess::placeBlock(SDL_Point loc, tile::Id tileId) {
	if (getBlock(loc).id != tile::Id::AIR) { return false; }
	TilePtr tile = Tile::getTile(tileId);
	// Calculate block rectangle
	Point<uint8_t> tDim = tile->getDim();
	SDL_Point dim = getDim();
	if (loc.x < 0 || loc.y < 0 ||
		loc.x + tDim.x > dim.x || loc.y + tDim.y > dim.y) {
		return false;
	}
	Rect bRect(loc.x * gameVals::BLOCK_W(), loc.y * gameVals::BLOCK_W(),
		tDim.x * gameVals::BLOCK_W(), tDim.y * gameVals::BLOCK_W());
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
			return true;
		}
	}
	return false;
}

void WorldAccess::draw(SDL_Point center) {
	AssetManager& assets = Window::Get().assets();

	int worldW = width(), worldH = height();
	Rect screen = getScreenRect(center);
	// Where on the screen to draw the world
	Rect worldRect(std::abs(screen.x), std::abs(screen.y),
		std::min(screen.w, worldW), std::min(screen.h, worldH));
	int lbX = (int)(std::max(screen.x, 0) / gameVals::BLOCK_W());
	int ubX = (int)ceil(std::min(screen.x2(), worldW) / gameVals::BLOCK_W());
	int lbY = (int)(std::max(screen.y, 0) / gameVals::BLOCK_W());
	int ubY = (int)ceil(std::min(screen.y2(), worldH) / gameVals::BLOCK_W());
	// TODO: batch rendering to avoid texture reassignment
	// Draw blocks
	assets.rect(&worldRect, skyColor());
	TextureData data;
	data.useTexture = true;
	data.dest = Rect(lbX * gameVals::BLOCK_W() - screen.x,
		lbY * gameVals::BLOCK_W() - screen.y,
		gameVals::BLOCK_W(), gameVals::BLOCK_W());
	for (int row = lbY; row < ubY; row++) {
		for (int col = lbX; col < ubX; col++) {
			tile::Id id = getBlock(col, row).id;
			if (id != tile::Id::AIR) {
				data.texture = Tile::getTile(id)->getImage({ col, row });
				assets.drawTexture(data);
			}
			data.dest.x += gameVals::BLOCK_W();
		}
		data.dest.x = lbX * gameVals::BLOCK_W() - screen.x;
		data.dest.y += gameVals::BLOCK_W();
	}
	// Draw dropped items
	drawDroppedItems(worldRect);
	// Draw border
	assets.thickRect(worldRect, 2, AssetManager::BorderType::inside, BLACK);
	// Draw player
	data.useTexture = false;
	data.textureId = gameVals::entities() + "player_pig.png";
	data.dest = assets.getMinRect(data.textureId,
		gameVals::BLOCK_W(), gameVals::BLOCK_W() * 2);
	data.dest.setCenter(center - screen.topLeft());
	assets.drawTexture(data);
}

void WorldAccess::drawLight(const Rect& rect) {}