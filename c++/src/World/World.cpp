#include "World.h"

const int World::SEC_PER_DAY = 60 * 24;
const int World::MS_PER_DAY = SEC_PER_DAY * 1000;
const int World::NOON = (int)(World::MS_PER_DAY / 2);
const int World::DAY = (int)(World::MS_PER_DAY / 4);
const int World::NIGHT = World::DAY * 3;

// World
World::~World() {
	fr.close();
	fw.discard();
}

void World::setFile(std::string fName) {
	fileName = fName;
	if (!isFile(fileName)) {
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
	blockData.clear();
	blocks.clear();
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
	time = (time + dt.milliseconds()) % MS_PER_DAY;
	// Run auto save
	nextSave -= dt.seconds();
	if (nextSave <= 0) {
		saveProgress = saveWorld(saveProgress);
		if (saveProgress >= 1) {
			saveProgress = 0.;
			nextSave = 30.;
			// Player.write();
		}
	}
	// Update minimap

	// Update chunks
	//manager.tick();
}

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
	int blocksLeft = (int)std::ceil(numBlocks / 100);
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
	// Save world info
	saveInfo();
	saveBlocks(0., dim.y);
	saveMap(0.);
	if (!fw.commit()) {
		std::cerr << "World::saveWorld(): File Close Error" << std::endl;
	}
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
	// If no data, return early to avoid divide by zero
	if (dim.x <= 0 || dim.y <= 0) { return 1.; }
	// Get current row, column, and blocks left to save
	const int startRow = (int)(progress * dim.y);
	for (int row = startRow; row < startRow + numRows && row < dim.y; row++) {
		// Save the tile id
		tile::Id val = blocks[row][0].id;
		size_t cnt = 0;
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
	return (double)(startRow + numRows) / dim.y;
}

double World::saveMap(double progress) {
	if (!fw.isOpen()) {
		std::cerr << "World::saveMap(): File not open" << std::endl;
		return 1.;
	}

	return 1.;
}

void World::placeBlock(int x, int y, tile::Id block) {}

void World::destroyBlock(int x, int y) {}

void World::removeBlock(int x, int y, tile::Id block) {}

Rect World::getScreenRect(const SDL_Point& playerPos) {
	int w = UI::width(), h = UI::height();
	int worldW = dim.x * gameVals::BLOCK_W;
	int worldH = dim.y * gameVals::BLOCK_W;
	Rect r(0, 0, w, h);
	if (worldW >= w) {
		r.setCenterX(playerPos.x);
		if (r.x < 0) { r.x = 0; }
		else if (r.x2() > worldW) { r.setX2(worldW); }
	} else {
		r.x = (int)((worldW - w) / 2);
	}
	if (worldH >= h) {
		r.setCenterY(playerPos.y);
		if (r.y < 0) { r.y = 0; }
		else if (r.y2() > worldH) { r.setY2(worldH); }
	} else {
		r.y = (int)((worldH - h) / 2);
	}
	return r;
}

void World::draw(const SDL_Point& playerPos) {
	int worldW = dim.x * gameVals::BLOCK_W;
	int worldH = dim.y * gameVals::BLOCK_W;
	Rect screen = getScreenRect(playerPos);
	Rect worldRect(std::abs(screen.x), std::abs(screen.y),
		std::min(screen.w, worldW), std::min(screen.h, worldH));
	int lbX = (int)(std::max(screen.x, 0) / gameVals::BLOCK_W);
	int ubX = (int)std::ceil(std::min(screen.x2(), worldW) / gameVals::BLOCK_W);
	int lbY = (int)(std::max(screen.y, 0) / gameVals::BLOCK_W);
	int ubY = (int)std::ceil(std::min(screen.y2(), worldH) / gameVals::BLOCK_W);
	Rect r(lbX * gameVals::BLOCK_W - screen.x, lbY * gameVals::BLOCK_W - screen.y,
		gameVals::BLOCK_W, gameVals::BLOCK_W);
	AssetManager& assets = UI::assets();
	assets.rect(&worldRect, Tile::getTile(tile::Id::AIR)->getMapColor());
	for (int row = lbY; row < ubY; row++) {
		for (int col = lbX; col < ubX; col++) {
			tile::Id id = blocks[row][col].id;
			if (id != tile::Id::AIR) {
				SDL_Texture* tex = Tile::getTile(id)->getImage({ col, row });
				assets.drawTexture(tex, r);
			}
			r.x += gameVals::BLOCK_W;
		}
		r.x = lbX * gameVals::BLOCK_W - screen.x;
		r.y += gameVals::BLOCK_W;
	}
	SDL_Texture* playerTex = assets.getAsset(
		createFile(PLAYER_IMGS,"player_pig", ".png"));
	r = Rect::getMinRect(playerTex, gameVals::BLOCK_W, gameVals::BLOCK_W * 2);
	r.setCenter(playerPos - screen.topLeft());
	assets.drawTexture(playerTex, r);
	assets.thickRect(worldRect, 2, BLACK);
	
}

void World::drawLight(const Rect& rect) {}

int World::surfaceH() const { return (int)(dim.y / 2); }
int World::underground() const { return (int)(dim.y * 2 / 3); }
SDL_Color World::skyColor() const {
	return SDL_Color{ 0,0,(uint8_t)(255 * (1 - std::pow((time - NOON) / NOON, 2))) };
}