#include "World.h"

const int World::SEC_PER_DAY = 60 * 24;
const int World::MS_PER_DAY = SEC_PER_DAY * 1000;
const int World::NOON = (int)(World::MS_PER_DAY / 2);
const int World::DAY = (int)(World::MS_PER_DAY / 4);
const int World::NIGHT = World::DAY * 3;

World::setFile(const std::string& fName) {
	fileName = fName;
	std::cerr << "World File: " << fileName << std::endl;
	// Create the file if it doesn't already exist
	if (!isFile(fileName)) {
		newWorld({ 10, 10 });
		blocks[0][0].id = TileId::STONE;
		std::cerr << dim.x << " x " << dim.y << std::endl;
		for (int i = 0; i < dim.x; i++) {
			for (int j = 0; j < dim.y; j++) {
				std::cerr << blocks[i][j].id << " ";
			}
			std::cerr << std::endl;
		}
		double progress = 0.;
		while (progress < 1) { progress = saveWorld(progress); }
		newWorld({ 100, 100 });
	}
	double progress = 0.;
	while (progress < 1) { progress = loadWorld(progress); }
	std::cerr << dim.x << " x " << dim.y << std::endl;
	for (int i = 0; i < dim.x; i++) {
		for (int j = 0; j < dim.y; j++) {
			std::cerr << blocks[i][j].id << " ";
		}
		std::cerr << std::endl;
	}
	std::cerr << Tile::getTile(TileId::AIR)->id() << std::endl;
	std::cerr << DragonEgg::Id() << " " << Portal::Id() << std::endl;
}

World::~World() {

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

void World::newWorld(const SDL_Point& newDim) {
	dim = newDim;
	numBlocks = dim.x * dim.y;
	blocks.clear();
	std::vector<Block> row(dim.x, Block{ TileId::AIR, 0x00 });
	blocks.resize(dim.x, row);
	row.clear();
	// Map
}

double World::loadWorld(double progress) {
	if (progress == 0) {
		std::cerr << "Loading World" << std::endl;
		// Reset world data
		//blockData.clear();
		crafters.clear();
		spawners.clear();
		// Load world info, automatically opens the file
		loadInfo(false);
	}
	progress *= 2;
	if (progress < 1.) { progress = loadBlocks(progress, 3) / 2.; }
	else { progress = drawWorld(progress - 1) / 2. + .5; }
	if (progress >= 1.) {
		file.close();
		//manager.setup();
		//manager.load_all();
	}
	return progress;
}

void World::loadInfo(bool closeFile) {
	// Open the file
	file.close();
	file.open(fileName, std::ios_base::in | std::ios_base::binary);
	// Load file data
	//readFile(file, &canDelete);
	//readFile(file, &type);
	//readFile(file, &time);
	//readFile(file, &dim);
	//readFile(file, &spawn);
	// Check if we should close the file
	if (closeFile) { file.close(); }

	newWorld(dim);
}

double World::loadBlocks(double progress, int numRows) {
	// If there is no data, just end the process
	if (blocks.empty()) { return 1.; }
	// Get current row, column, and blocks left to save
	const int startRow = (int)(progress * dim.y);
	for (int row = startRow; row < startRow + numRows && row < dim.y; row++) {
		int col = 0;
		while (col < dim.x) {
			// Extract tile id and number of tiles
			TileId val;
			//readFile(file, &val);
			int num;
			//readFile(file, &num);
			// Add blocks
			for (int i = col; i < col + num;) {
				blocks[row][i].id = val;
				// If not multiblock part
				if (blocks[row][i].src == 0) {
					// Create multiblock
					// TODO: get tile dimensions
					SDL_Point bDim{ 1, 1 };
					for (int dy = 0; dy < bDim.y; dy++) {
						for (int dx = 0; dx < bDim.x; dx++) {
							blocks[row + dy][i + dx].setSrc(dx, dy);
							addBlock(i + dx, row + dy,
								blocks[row + dy][i + dx].id);
						}
					}
					// Load file data
					// TODO: Load tile data
					i += bDim.x;
				} else { i++; }
			}
			col += num;
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
		TileId val;
		readFile(file, &val);
		int num;
		readFile(file, &num);
		// TODO: Tile defaults to air if it doesn't exist
		// Make sure we don't go over the row
		if (col + num > dim.x) { num = dim.x - col; }
		if (val != TileId::AIR) {
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
	return 1.;
}

double World::saveWorld(double progress) {
	if (progress == 0) {
		std::cerr << "Saving World" << std::endl;
		// Save world info, automatically opens the file
		saveInfo();
	}
	progress *= 2;
	if (progress < 1.) { progress = saveBlocks(progress, 3) / 2.; }
	else { progress = saveMap(progress - 1) / 2. + .5; }
	if (progress >= 1.) { file.close(); }
	return progress;
}

void World::saveInfo() {
	// Opend the file
	file.close();
	file.open(fileName, std::ios_base::out | std::ios_base::binary);
	// Write data
	//writeFile(file, &canDelete);
	//writeFile(file, &type);
	//writeFile(file, &time);
	//writeFile(file, &dim);
	//writeFile(file, &spawn);
}

double World::saveBlocks(double progress, int numRows) {
	// If there is no data, just end the process
	if (blocks.empty()) { return 1.; }
	// Get current row, column, and blocks left to save
	const int startRow = (int)(progress * dim.y);
	for (int row = startRow; row < startRow + numRows && row < dim.y; row++) {
		int col = 0;
		// Save the tile id
		TileId val = blocks[row][col].id;
		// Keep going until we hit a new tile or the end of the row
		// This ignores blocks_left so we can store the entire strip of this block type
		while (col < dim.x && val == blocks[row][col].id) {
			col += 1; // TODO: += Block width
		}
		//writeFile(file, &val);
		//writeFile(file, &col);
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
	return 1.;
}

void World::placeBlock(int x, int y, TileId block) {}

void World::destroyBlock(int x, int y) {}

void World::addBlock(int x, int y, TileId block) {}

void World::removeBlock(int x, int y, TileId block) {}

Rect World::getScreenRect(const SDL_Point& playerPos) {
	return Rect();
}

void World::drawLight(const Rect& rect) {}

int World::surfaceH() const { return (int)(dim.y / 2); }
int World::underground() const { return (int)(dim.y * 2 / 3); }
SDL_Color World::skyColor() const {
	return SDL_Color{ 0,0,(uint8_t)(255 * (1 - std::pow((time - NOON) / NOON, 2))) };
}