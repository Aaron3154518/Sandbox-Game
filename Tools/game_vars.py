# Created on 3 December 2019
# Contains variable data that multiple objects need

# BE WARY OF IMPORT LOOPS!!!
from sys import exit
from time import time
import pygame as pg
import math
from pygame.locals import QUIT, VIDEORESIZE
from Tools.constants import BLOCK_W, resize
from Tools import constants as c
from Tools.tile_ids import AIR
from UI.Operations import CompleteTask, loading_bar, percent

# Object lists
items, tiles = {}, {}
biomes, structures = {}, {}
animations = []
# Game objects
world = handler = player = None
# Game time and time since last update (seconds)
game_time = dt = 0
# Damage text boxes ([surface, rect, timer])
dmg_text = []


def init():
    global game_time
    game_time = time()

    # Load object lists
    from inspect import getmembers, isclass
    from Objects import ItemObjects, TileObjects
    from World import WorldGenParts

    # Compile a list of items, tiles, biomes, and structures
    items.clear(), tiles.clear(), biomes.clear(), structures.clear()
    for module in [ItemObjects, TileObjects, WorldGenParts]:
        for name, obj in getmembers(module):
            if isclass(obj) and module.__name__ in str(obj):
                # The constructor automatically adds the item to the list
                obj()

    # Load world and player and initialize entity handler
    from World.World import World
    from NPCs.EntityHandler import EntityHandler
    from Player.Player import Player
    from World.WorldSelector import run_selector, PLAYER, UNIVERSE

    global world, handler, player
    universe = run_selector(UNIVERSE)
    world = World(universe, universe)
    handler = EntityHandler()
    player = Player(run_selector(PLAYER))
    player.load()

    load_world()


def tick():
    # Update game time and get time since last tick
    global game_time, dt
    dt = time() - game_time
    game_time = time()

    # Check events
    events = pg.event.get()
    for e in events:
        if e.type == QUIT:
            return False
        elif e.type == VIDEORESIZE:
            resize(e.w, e.h)
            player.on_resize()
        else:
            continue
        events.remove(e)

    # Update world
    world.tick()

    # Update the player
    if player.map_open:
        player.run_map(events)
    else:
        player.run_main(events)

    # Spawn entities
    handler.spawn()
    # Move entities, dropped items, and projectiles
    # Also checks collisions and attacks
    handler.move(player)

    draw()
    return True


def draw():
    display = pg.display.get_surface()
    display.fill(world.sky_color)
    rect = world.get_screen_rect(player.rect.center)

    if player.map_open:
        # Draw world map
        player.draw_map()
    else:
        # Draw background - sky and blocks
        world.draw_blocks(rect)
        # Draw enemies, items, and projectiles
        handler.draw_display(rect)
        # Draw pre-ui player visuals
        player.draw_pre_ui(rect)
        # Draw damage text boxes
        for arr in dmg_text:
            # Decrement textbox counter
            arr[2] -= dt
            # Check if the textbox is done
            if arr[2] <= 0:
                dmg_text.remove(arr)
            else:
                # Move the text up
                arr[1][1] -= BLOCK_W * 1.5 * dt
                # Check if we need to blit it
                r = arr[0].get_rect(center=arr[1])
                if r.colliderect(rect):
                    display.blit(arr[0], (r.x - rect.x, r.y - rect.y))
        # Draw player ui
        player.draw_ui(rect)


# Returns position of mouse with respect to the entire world
def global_mouse_pos():
    pos = pg.mouse.get_pos()
    screen_c = pg.display.get_surface().get_rect().center
    world_c = player.rect.center
    return [pos[i] + world_c[i] - screen_c[i] for i in range(2)]


# Center of the player rectangle, if blocks = True, return in block coords
def player_pos(in_blocks=False):
    if not in_blocks:
        return player.rect.center
    else:
        return [p / BLOCK_W for p in player.rect.center]


# Top left of the player rectangle, see above for in_blocks
def player_topleft(in_blocks = False):
    if not in_blocks:
        return player.pos
    else:
        return [p / BLOCK_W for p in player.pos]


# World dimensions in blocks
def world_dim():
    return world.dim


# Get block at position
def get_block_at(x, y):
    if 0 <= x < world.dim[0] and 0 <= y < world.dim[1]:
        x, y = get_topleft(x, y)
        return world.blocks[y, x]
    return 0


# Functions that affect the world object
# Returns the topleft coordinates of the block at x,y (for multiblocks)
def get_topleft(x, y):
    if 0 <= x < world.dim[0] and 0 <= y < world.dim[1]:
        idx = world.blocks[y][x]
        if idx < 0:
            idx *= -1
            x -= idx // 100
            y -= idx % 100
        return x, y
    print("get_topleft(): Coords out of bounds: {}, {}".format(x, y))
    return 0, 0


# Get block data
def get_block_data(pos):
    return c.get_from_dict(*pos, world.block_data)


# Write block data, input data as none to remove
def write_block_data(pos, data):
    if data is not None:
        c.update_dict(*pos, data, world.block_data)
    else:
        c.remove_from_dict(*pos, world.block_data)


# Attempts to break a block, returns true if successful, false otherwise
def break_block(x, y):
    if 0 <= x < world.dim[0] and 0 <= y < world.dim[1]:
        x, y = get_topleft(x, y)
        block = world.blocks[y, x]
        if block != AIR:
            tile = tiles[block]
            if tile.on_break((x, y)):
                world.destroy_block(x, y)
                block_rect = pg.Rect(x * BLOCK_W, y * BLOCK_W, BLOCK_W * tile.dim[0], BLOCK_W * tile.dim[1])
                drops = tile.get_drops()
                for drop in drops:
                    if drop[1] > 0:
                        # Drop an item
                        drop_item(*drop, None, pos_=block_rect.center)
                return True
    return False


# Attempts to place a block, returns true if successful, false otherwise
def place_block(x, y, idx):
    tile = tiles[idx]
    # Calculate block rectangle
    block_rect = pg.Rect(x * BLOCK_W, y * BLOCK_W, tile.dim[0] * BLOCK_W, tile.dim[0] * BLOCK_W)
    if not player.rect.colliderect(block_rect) and \
            not handler.collides_with_entity(block_rect) and tile.can_place((x, y)):
        world.place_block(x, y, idx)
        tile.on_place((x, y))
        return True
    return False


# Functions that affect the entity handler object
# Drops an item - input position in pixels
#               - left = None drops the item straight down
def drop_item(item, amnt, left, pos_=None):
    from Objects.DroppedItem import DroppedItem
    if pos_ is None:
        pos_ = player.rect.center
    item_obj = DroppedItem(item, amnt)
    item_obj.drop(pos_, left)
    handler.items.append(item_obj)


# Spawns an enemy
def spawn_entity(entity, pos):
    entity.set_pos(*pos)
    handler.entities.append(entity)


# Shoots a projectile
def shoot_projectile(projectile):
    if projectile.hurts_mobs:
        handler.player_projectiles.append(projectile)
    else:
        handler.mob_projectiles.append(projectile)


# Adds a damage text box
def add_damage_text(dmg, pos):
    text = c.ui_font.render(str(dmg), 1, (255, 0, 0))
    dmg_text.append([text, list(pos), 1])


# Functions that affect the player object
# Sets player's active ui
def set_active_ui(ui):
    player.active_ui = ui


# Sets the item data for the current held item
def set_current_item_data(data):
    player.inventory.set_item_data(data)


# Gets the item data for the current held item
def get_current_item_data():
    return player.inventory.get_item_data()


# Functions that check the world blocks
# Checks if a chunk contains only a given tile
def contains_only(x, y, w, h, tile):
    # If the chunk is outside the world, return false
    if not 0 <= x < world.dim[0] - w or not 0 <= y < world.dim[1] - h:
        return False
    for y1 in range(y, y + h):
        for x1 in range(x, x + w):
            if world.blocks[y1][x1] != tile:
                return False
    return True


# Checks if a chunk contains a given tile
def contains(x, y, w, h, tile):
    # lb must be >= 0, If ub < 0 then d is now <= 0
    if x < 0:
        w += x
        x = 0
    if y < 0:
        h += y
        y = 0
    # ub must be < world size, if lb > world size then d is now <= 0
    if x + w > world.dim[0]:
        w = world.dim[0] - x
    if y + h > world.dim[1]:
        h = world.dim[1] - y
    if not (w <= 0 or h <= 0):
        for y1 in range(y, y + h):
            for x1 in range(x, x + w):
                if world.blocks[y1][x1] == tile:
                    return True
    return False


# Checks if the blocks adjacent to a chunk conatain
# a given tile or only contain a given tile
def adjacent(x, y, w, h, tile, only):
    left, top, right, bot = True, True, True, True
    # lb must be >= 0, If ub < 0 then d is now <= 0
    if x < 1:
        w += x - 1
        x = 1
        left = False
    if y < 1:
        h += y - 1
        y = 1
        top = False
    # ub must be < world size, if lb > world size then d is now <= 0
    if x + w > world.dim[0] - 1:
        w = world.dim[0] - x - 1
        right = False
    if y + h > world.dim[1] - 1:
        h = world.dim[1] - y - 1
        bot = False
    if not (left or right) or not (top or bot):
        return False
    # Iterate through relevant data
    x_points = ([x - 1] if left else []) + ([x + w] if right else [])
    y_points = ([y - 1] if top else []) + ([y + h] if bot else [])
    data = world.blocks[y:y + h + 1, x_points].flatten() + world.blocks[y_points, x:x + w + 1].flatten()
    for val in data:
        equal = val == tile
        if only and not equal:
            return False
        elif not only and equal:
            return True
    return only


# Handles movement, checking for collisions with blocks
def check_collisions(pos, block_dim, d):
    abs_d = [abs(val) for val in d]
    px_dim = (BLOCK_W * block_dim[0], BLOCK_W * block_dim[1])
    # Break up displacement into smaller parts
    while max(abs_d) > BLOCK_W:
        perc = BLOCK_W / max(abs_d)
        d_ = [d[0] * perc, d[1] * perc]
        check_collisions(pos, block_dim, d_)
        d = [d[0] - d_[0], d[1] - d_[1]]
        abs_d = [abs(val) for val in d]

    # Calculate current and next block (left, top, right, bottom)
    current_block = [0, 0, 0, 0]
    next_block = [0, 0, 0, 0]
    to_next = [0, 0]
    blocks = world.blocks
    for i in range(2):
        # Get current and next block
        current_block[i] = int(pos[i] / BLOCK_W)
        current_block[i + 2] = math.ceil((pos[i] + px_dim[i]) / BLOCK_W) - 1
        next_block[i] = int((pos[i] + d[i]) / BLOCK_W)
        next_block[i + 2] = math.ceil((pos[i] + px_dim[i] + d[i]) / BLOCK_W) - 1
        # If we don't move blocks or we hit the world boundary, just do the movement
        if pos[i] + d[i] < 0:
            pos[i] = 0
            d[i] = 0
        elif next_block[i + 2] >= blocks.shape[1 - i]:
            pos[i] = (blocks.shape[1 - i] * BLOCK_W) - px_dim[i]
            d[i] = 0
        elif current_block[i if d[i] < 0 else i + 2] == next_block[i if d[i] < 0 else i + 2]:
            pos[i] += d[i]
            d[i] = 0
        else:
            # End pos - begin pos, accounting for using right or bottom sides
            to_next[i] = (next_block[i + (0 if d[i] < 0 else 2)] * BLOCK_W) - pos[i] - (
                -BLOCK_W if d[i] < 0 else px_dim[i])

    if d.count(0) == 1:
        idx = 1 - d.index(0)
        if idx == 0:
            # From lowest row to highest row, at the next column over
            collide = blocks[current_block[1]:current_block[3] + 1, next_block[0 if d[0] < 0 else 2]]
        else:
            # From the lowest column to the highest column, at the next row over
            collide = blocks[next_block[1 if d[1] < 0 else 3], current_block[0]:current_block[2] + 1]
        collide = collide.tolist()
        # All blocks are air, just do the move
        if collide.count(AIR) == len(collide):
            pos[idx] += d[idx]
        # >= 1 block is solid, truncate movement
        else:
            pos[idx] += to_next[idx]
    elif d.count(0) == 0:
        perc = [to_next[0] / d[0], to_next[1] / d[1]]
        # Index of shortest time to next block
        idx = perc.index(min(perc))
        # Index of longest time to next block
        idx2 = 1 - idx
        delta = d[idx] * max(perc)
        # When the idx direction hits the next block, idx2 has not changed blocks
        if idx == 0:
            # From lowest row to highest row, at the next column over
            collide = blocks[current_block[1]:current_block[3] + 1, next_block[0 if d[0] < 0 else 2]]
        else:
            # From the lowest column to the highest column, at the next row over
            collide = blocks[next_block[1 if d[1] < 0 else 3], current_block[0]:current_block[2] + 1]
        collide = collide.tolist()
        # All blocks are air, just do the move
        if collide.count(AIR) == len(collide):
            pos[idx] += d[idx]
        else:
            # Just move to next block and cuttoff delta
            pos[idx] += to_next[idx]
            delta = to_next[idx]

        # Calculate bounds in the direction of idx when the direction of idx2 hits the next block
        current_val = [int((pos[idx] + delta) / BLOCK_W),
                       math.ceil((pos[idx] + px_dim[idx] + delta) / BLOCK_W) - 1]
        if idx2 == 0:
            # From lowest row to highest row, at the next column over
            collide = blocks[current_val[0]:current_val[1] + 1, next_block[0 if d[0] < 0 else 2]]
        else:
            # From the lowest column to the highest column, at the next row over
            collide = blocks[next_block[1 if d[1] < 0 else 3], current_val[0]:current_val[1] + 1]
        collide = collide.tolist()
        # All blocks are air, just do the move
        if collide.count(AIR) == len(collide):
            pos[idx2] += d[idx2]
        else:
            pos[idx2] += to_next[idx2]


# Checks if we are touching blocks on the left or right
def touching_blocks_x(pos, dim, left):
    # Get dimensions in pixels
    w, h = dim[0] * BLOCK_W, dim[1] * BLOCK_W
    # Check if we are actually touching a new block (including non-solid)
    if abs(pos[0] + (0 if left else w)) % BLOCK_W == 0:
        # Get next x block
        next_x = int(pos[0] / BLOCK_W) - 1 if left else math.ceil((pos[0] + w) / BLOCK_W)
        # Check if we are going to the world edge
        if next_x < 0 if left else next_x >= world.dim[0]:
            return True
        # Otherwise check if there is a solid block
        else:
            y_range = (int(pos[1] / BLOCK_W), math.ceil((pos[1] + h) / BLOCK_W))
            collide = world.blocks[y_range[0]:y_range[1], next_x].tolist()
            return collide.count(AIR) < len(collide)
    return False


# Checks if we are touching blocks on the top or bottom
def touching_blocks_y(pos, dim, top):
    # Get dimensions in pixels
    w, h = dim[0] * BLOCK_W, dim[1] * BLOCK_W
    # Check if we are actually touching a new block (including non-solid)
    diff = (pos[1] + (0 if top else h)) % BLOCK_W
    touching = diff == 0
    if touching:
        # Get next y block
        next_y = int(pos[1] / BLOCK_W) - 1 if top else math.ceil((pos[1] + h) / BLOCK_W)
        # Check if we are going to the world edge
        if next_y < 0 if top else next_y >= world.dim[1]:
            return True
        # Otherwise check if there is a solid block
        else:
            x_range = (int(pos[0] / BLOCK_W), math.ceil((pos[0] + w) / BLOCK_W))
            collide = world.blocks[next_y, x_range[0]:x_range[1]].tolist()
            return collide.count(AIR) < len(collide)
    return False


# Functions that change the world
# Closes current world
def close_world():
    CompleteTask(world.save_part, [3], loading_bar, ["Saving World"], can_exit=False).run_now()
    player.write()


# Loads a new world
def load_world():
    # Load the world
    if not CompleteTask(world.load_part, [], percent, ["Loading World Blocks"]).run_now():
        pg.quit()
        exit(0)
    # Draw world blocks
    world.draw_world()
    # Reset entity handler
    handler.reset()
    # Set up player map
    player.set_map_source(world.map)
    # Spawn the player
    player.spawn()


# Changes world
def change_world(new_world):
    def screen_goes_white(progress):
        display = pg.display.get_surface()
        draw()
        overlay = pg.Surface(display.get_size())
        overlay.fill((255 * progress, 255 * progress, 255))
        overlay.set_alpha(255 * progress)
        display.blit(overlay, (0, 0))

    CompleteTask(world.save_part, [10], screen_goes_white, [], can_exit=False).run_now()
    player.write()
    world.name = new_world
    load_world()
