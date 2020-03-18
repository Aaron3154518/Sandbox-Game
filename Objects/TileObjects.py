# Created on 4 December 2019

from sys import byteorder
from random import choice
from pygame.locals import *
from Objects.TileTypes import *
from Objects.DroppedItem import DroppedItem
from Objects.ItemTypes import Upgradable
from Objects.Animation import Animation, OscillateAnimation
from Objects import INV, PROJ
from NPCs import Mobs as mobs
from Player.ActiveUI import ActiveUI
from Player.Inventory import Inventory
from Tools import constants as c, item_ids as i, tile_ids as t
from Tools import game_vars
from World import WorldGenerator
from World.World import World
from World.Selector import WorldSelector


# Normal Tiles
class Air(Tile):
    def __init__(self):
        super().__init__(t.AIR)


class Dirt(Tile):
    def __init__(self):
        super().__init__(t.DIRT, img=INV + "dirt.png")
        self.add_drop(i.DIRT, 1)
        self.map_color = (150, 75, 0)
        self.hp = 2


class Stone(Tile):
    def __init__(self):
        super().__init__(t.STONE, img=INV + "stone.png")
        self.add_drop(i.STONE, 1)
        self.map_color = (200, 200, 200)
        self.hardness = 1


class Snow(Tile):
    def __init__(self):
        super().__init__(t.SNOW, img=INV + "snow.png")
        self.add_drop(i.SNOW_BALL, 2, 5)
        self.map_color = (255, 255, 255)


class Wood(Tile):
    def __init__(self):
        super().__init__(t.WOOD, img=INV + "wood_tile.png")
        self.add_drop(i.WOOD, 1)
        self.map_color = (100, 50, 0)


class Leaves(Tile):
    def __init__(self):
        super().__init__(t.LEAVES, img=INV + "leaves.png")
        self.add_drop(i.LEAVES, 1)
        self.map_color = (0, 150, 0)


class Boulder1(Tile):
    def __init__(self):
        super().__init__(t.BOULDER1, img=INV + "boulder_1.png")
        self.add_drop(i.STONE, 0, max_amnt=1)
        self.map_color = (100, 100, 64)
        self.hardness = 1


class Boulder2(Tile):
    def __init__(self):
        super().__init__(t.BOULDER2, img=INV + "boulder_2.png", dim=(2, 1))
        self.map_color = (100, 100, 64)
        self.add_drop(i.STONE, 0, max_amnt=2)
        self.hardness = 1


class Boulder3(Tile):
    def __init__(self):
        super().__init__(t.BOULDER3, img=INV + "boulder_3.png", dim=(3, 2))
        self.map_color = (100, 100, 64)
        self.add_drop(i.STONE, 1, max_amnt=3)
        self.hardness = 1


class ShinyStone1(Tile):
    def __init__(self):
        super().__init__(t.SHINY_STONE_1, img=INV + "shiny_stone_1.png")
        self.add_drop(i.SHINY_STONE_1, 1)
        self.map_color = (175, 175, 175)
        self.hardness = 1


class ShinyStone2(Tile):
    def __init__(self):
        super().__init__(t.SHINY_STONE_2, img=INV + "shiny_stone_2.png")
        self.add_drop(i.SHINY_STONE_2, 1)
        self.map_color = (150, 150, 150)
        self.hardness = 2


class ShinyStone3(Tile):
    def __init__(self):
        super().__init__(t.SHINY_STONE_3, img=INV + "shiny_stone_3.png")
        self.add_drop(i.SHINY_STONE_3, 1)
        self.map_color = (125, 125, 125)
        self.hardness = 3


class DragonEgg(Tile):
    def __init__(self):
        super().__init__(t.DRAGON_EGG, img=INV + "dragon_egg.png")
        self.add_drop(i.DRAGON_EGG, 1)
        self.map_color = (64, 200, 0)
        self.hardness = 2

    def on_break(self, pos):
        game_vars.spawn_entity(mobs.Dragon(),
                               [p + randint(10, 20) * BLOCK_W * c.random_sign() for p in game_vars.player_pos()])
        return False


# End


# Crafting Stations
class WorkTable(CraftingStation):
    def __init__(self):
        super().__init__(t.WORK_TABLE, dim=(2, 1), img=INV + "work_table.png")
        self.on_surface = True
        self.map_color = (54, 78, 154)
        self.add_drop(i.WORK_TABLE, 1)
        self.hardness = 1

    def get_recipes(self):
        return [[[i.SNOW, 1], [i.SNOW_BALL, 4]],
                [[i.FOREST, 1], [i.DIRT, 50], [i.CAT, 1]],
                [[i.MOUNTAIN, 1], [i.STONE, 10], [i.SNOW, 15]],
                [[i.VALLEY, 1], [i.STONE, 50], [i.ZOMBIE, 1]],
                [[i.BASIC_SWORD, 1], [i.WOOD, 10], [i.STONE, 20]],
                [[i.CRUSHER, 1], [i.STONE, 15], [i.SHINY_STONE_1, 10]],
                [[i.CHEST, 1], [i.WOOD, 15], [i.STONE, 5]],
                [[i.WORLD_BUILDER, 1], [i.STONE, 25], [i.OBSIDIAN, 5]],
                [[i.SNOW_BALL, 1]]]


class Forge(CraftingStation):
    def __init__(self):
        super().__init__(t.FORGE, dim=(1, 2))
        self.on_surface = True
        self.map_color = (99, 99, 99)
        self.add_drop(i.FORGE, 1)
        self.hardness = 1
        self.set_animation(Animation(INV + "forge/", [d * BLOCK_W for d in self.dim], .25))

    def get_recipes(self):
        return [[[i.IRON_BAR, 1], [i.IRON_ORE, 2]],
                [[i.GOLD_BAR, 1], [i.GOLD_ORE, 2]]]


# Spawners
class CatSpawner(SpawnTile):
    def __init__(self):
        super().__init__(t.CAT, mobs.Cat(), item_id=i.CAT)
        self.add_drop(i.CAT, 1)


class ZombieSpawner(SpawnTile):
    def __init__(self):
        super().__init__(t.ZOMBIE, mobs.Zombie(), item_id=i.ZOMBIE)
        self.add_drop(i.ZOMBIE, 1)


class DoomBunnySpawner(SpawnTile):
    def __init__(self):
        super().__init__(t.DOOM_BUNNY, mobs.DoomBunny(), item_id=i.DOOM_BUNNY)
        self.add_drop(i.DOOM_BUNNY, 1)


class HelicopterSpawner(SpawnTile):
    def __init__(self):
        super().__init__(t.HELICOPTER, mobs.Helicopter(), item_id=i.HELICOPTER)
        self.add_drop(i.HELICOPTER, 1)
        self.map_color = (0, 0, 80)


class BirdieSpawner(SpawnTile):
    def __init__(self):
        super().__init__(t.BIRDIE, mobs.Birdie(), item_id=i.BIRDIE)
        self.add_drop(i.BIRDIE, 1)
        self.map_color = (0, 0, 80)


# Tiles that do stuff
class DimensionHopper(FunctionalTile):
    def __init__(self):
        super().__init__(t.DIMENSION_HOPPER, img=INV + "dimension_hopper.png")
        self.scroller = None
        self.add_drop(i.DIMENSION_HOPPER, 1)
        self.map_color = (0, 0, 0)
        self.hardness = 2

    def activate(self, pos):
        game_vars.player.set_active_ui(self.UI(pos))

    class UI(ActiveUI):
        def __init__(self, pos):
            self.selector = WorldSelector(game_vars.universe())

            ActiveUI.__init__(self, None, None, pos=pos)

            self.on_resize()

        def process_events(self, events, mouse, keys):
            if self.selector.handle_events(events):
                game_vars.player.set_active_ui(None)

        def on_resize(self):
            r = pg.Rect(0, 0, c.MIN_W // 2, c.MIN_H * 3 // 4)
            r.center = c.screen_center
            self.selector.resize(rect=r)
            self.ui, self.rect = self.selector.get_surface(), self.selector.get_rect()


class WorldBuilder(FunctionalTile):
    INV_SPOTS = 6

    def __init__(self):
        super().__init__(t.WORLD_BUILDER)
        self.on_surface = True
        self.add_drop(i.WORLD_BUILDER, 1)
        self.map_color = (0, 0, 0)
        self.hardness = 1
        self.set_animation(Animation(INV + "world_builder/", [d * BLOCK_W for d in self.dim], .25))

    def activate(self, pos):
        data = game_vars.get_block_data(pos)
        if data is not None:
            game_vars.player.set_active_ui(self.UI(pos, data))

    def on_place(self, pos):
        from Player.Inventory import new_inventory
        game_vars.write_block_data(pos, new_inventory((1, self.INV_SPOTS)))

    def on_break(self, pos):
        data = game_vars.get_block_data(pos)
        if data is not None:
            # Check if we have any items
            for byte in data:
                if byte != 0:
                    return False
            # If not, remove our data
            c.remove_from_dict(*pos, game_vars.world.block_data)
        return True

    class UI(ActiveUI):
        BIOME, STRUCTURE, SIZE = range(3)
        INV_ORDER = [BIOME, STRUCTURE, SIZE]

        def __init__(self, pos, data):
            self.name = c.WorldFile(game_vars.universe())
            self.cursor = False
            # Load inventories
            invs = {self.BIOME: Inventory((2, 2), max_stack=1, whitelist=game_vars.biomes.keys()),
                    self.STRUCTURE: Inventory((1, 1), max_stack=4, whitelist=[i.BONUS_STRUCTURE]),
                    self.SIZE: Inventory((1, 1), max_stack=1, whitelist=WorldGenerator.WORLD_DIMS.keys())}
            for idx in self.INV_ORDER:
                data = invs[idx].load(data)
            # Set text height
            text_h = c.INV_W // 2
            # Text
            text = {self.BIOME: "Biome Cards",
                    self.STRUCTURE: "Bonus Structure",
                    self.SIZE: "Size Card"}
            # Get font and inventory section width
            longest = c.get_widest_string(text.values())
            font = c.get_scaled_font(-1, text_h, longest)
            inv_w = font.size(longest)[0]
            # Get all surfaces to draw
            surfaces = []
            y = 0
            for idx in self.INV_ORDER:
                text_s = font.render(text[idx], 1, (255, 255, 255))
                text_rect = text_s.get_rect(centerx=inv_w // 2, centery=y + text_h // 2)
                surfaces.append([text_s, text_rect])
                y += text_h
                inv = invs[idx]
                inv.rect.y, inv.rect.centerx = y, inv_w // 2
                surfaces.append([inv.surface, inv.rect])
                y += inv.rect.h
            # Draw world name text
            text_h = y // 7
            font = c.get_scaled_font(inv_w, text_h, "World Name:")
            text = font.render("World Name:", 1, (255, 255, 255))
            surfaces.append([text, text.get_rect(center=(inv_w * 3 // 2, text_h * 3 // 2))])
            # Draw create button text
            font = c.get_scaled_font(inv_w, text_h * 2, "Create!")
            text = font.render("Create!", 1, (0, 200, 0))
            self.create_rect = text.get_rect(center=(inv_w * 3 // 2, text_h * 5))
            surfaces.append([text, self.create_rect])
            # Used to enter world name
            self.name_rect = pg.Rect(inv_w, text_h * 2, inv_w, text_h)
            # Draw surface
            s = pg.Surface((inv_w * 2, y))
            for surface, rect in surfaces:
                s.blit(surface, rect)
            super().__init__(s, s.get_rect(), pos=pos, invs=invs)
            self.rect.center = c.screen_center

            if not game_vars.player.inventory.open:
                game_vars.player.inventory.toggle()

        @property
        def data(self):
            data = bytearray()
            for idx in self.INV_ORDER:
                data += self.invs[idx].write()
            return data

        def process_events(self, events, mouse, keys):
            temp = self.cursor
            self.cursor = (pg.time.get_ticks() // 400) % 2 == 0
            if temp != self.cursor:
                self.draw_name()
            pos = pg.mouse.get_pos()
            if self.rect.collidepoint(*pos) and game_vars.player.use_time <= 0:
                pos = [pos[0] - self.rect.x, pos[1] - self.rect.y]
                # Clicked create
                if self.create_rect.collidepoint(*pos):
                    if not self.invs[self.BIOME].is_empty() and not self.invs[self.SIZE].is_empty() and self.name.valid:
                        for e in events:
                            if e.type == MOUSEBUTTONUP and e.button == BUTTON_LEFT:
                                events.remove(e)
                                new = World(self.name)
                                new.can_delete = True
                                items = self.invs[self.BIOME].inv_items.flatten().tolist()
                                items += [self.invs[self.SIZE].inv_items[0][0]]
                                items += [i.BONUS_STRUCTURE] * self.invs[self.STRUCTURE].inv_amnts[0][0]
                                WorldGenerator.generate_world(new, modifiers=items)
                                del new
                                from Player.Inventory import new_inventory
                                game_vars.write_block_data(self.block_pos, new_inventory((1, WorldBuilder.INV_SPOTS)))
                                game_vars.player.set_active_ui(None)
                # Clicked inventories
                else:
                    for inv in self.invs.values():
                        if inv.rect.collidepoint(*pos):
                            pos = [pos[0] - inv.rect.x, pos[1] - inv.rect.y]
                            if mouse[BUTTON_LEFT - 1]:
                                inv.left_click(pos)
                            elif mouse[BUTTON_RIGHT - 1]:
                                inv.right_click(pos)
                            self.ui.fill((0, 0, 0), inv.rect)
                            self.ui.blit(inv.surface, inv.rect)
                            game_vars.write_block_data(self.block_pos, self.data)
                            break
            for e in events:
                if e.type == KEYDOWN:
                    self.name.type_char(e)
                    self.draw_name()

        def draw_name(self):
            font = c.get_scaled_font(*self.name_rect.size, self.name.name + "|")
            text = font.render(self.name.name + ("|" if self.cursor else ""), 1, (255, 255, 255))
            text_rect = text.get_rect(center=self.name_rect.center)
            self.ui.fill((0, 0, 0), self.name_rect)
            self.ui.blit(text, text_rect)


class Chest(FunctionalTile):
    INV_DIM = (10, 5)

    def __init__(self):
        super().__init__(t.CHEST, img=INV + "chest.png", dim=(2, 2))
        self.on_surface = True
        self.add_drop(i.CHEST, 1)
        self.map_color = (200, 200, 0)
        self.hardness = 1

    def on_place(self, pos):
        from Player.Inventory import new_inventory
        game_vars.write_block_data(pos, new_inventory(self.INV_DIM))

    def on_break(self, pos):
        data = game_vars.get_block_data(pos)
        if data is not None:
            # Check if we have any i
            for byte in data:
                if byte != 0:
                    return False
            # If not, remove our data
            c.remove_from_dict(*pos, game_vars.world.block_data)
        return True

    def activate(self, pos):
        data = game_vars.get_block_data(pos)
        if data is not None:
            game_vars.player.set_active_ui(self.UI(pos, data))

    class UI(ActiveUI):
        def __init__(self, pos, data):
            inventory = Inventory(Chest.INV_DIM)
            ActiveUI.__init__(self, inventory.surface, inventory.rect.move(0, inventory.rect.h),
                              pos=pos, invs={0: inventory})
            self.can_drag = False
            self.invs[0].load(data)

            if not game_vars.player.inventory.open:
                game_vars.player.inventory.toggle()

        def process_events(self, events, mouse, keys):
            pos = pg.mouse.get_pos()
            if self.rect.collidepoint(*pos):
                pos = [pos[0] - self.rect.x, pos[1] - self.rect.y]
                if game_vars.player.use_time <= 0:
                    if mouse[BUTTON_LEFT - 1]:
                        self.invs[0].left_click(pos)
                    elif mouse[BUTTON_RIGHT - 1]:
                        self.invs[0].right_click(pos)
                    game_vars.write_block_data(self.block_pos, self.invs[0].write())


class Crusher(FunctionalTile):
    def __init__(self):
        super().__init__(t.CRUSHER, dim=(2, 2))
        self.on_surface = True
        self.add_drop(i.CRUSHER, 1)
        self.map_color = (64, 64, 64)
        self.hardness = 1
        self.set_animation(Animation(INV + "crusher/", [d * BLOCK_W for d in self.dim], .25))

    def on_place(self, pos):
        game_vars.write_block_data(pos, bytearray(4))

    def on_break(self, pos):
        data = game_vars.get_block_data(pos)
        if data is not None:
            amnt = int.from_bytes(data[:2], byteorder)
            if amnt != 0:
                # Drop contents
                from random import choice
                item = int.from_bytes(data[2:4], byteorder)
                game_vars.drop_item(DroppedItem(item, amnt), choice([True, False]),
                                    [pos[0] * BLOCK_W, pos[1] * BLOCK_W])
        game_vars.write_block_data(pos, None)
        return True

    def activate(self, pos):
        data = game_vars.get_block_data(pos)
        if data is not None:
            game_vars.player.set_active_ui(self.UI(pos, data, 9))
            if not game_vars.player.inventory.open:
                game_vars.player.inventory.toggle()

    class UI(ActiveUI):
        ITEMS = [i.SHINY_STONE_1, i.SHINY_STONE_2, i.SHINY_STONE_3]

        def __init__(self, pos, data, max_stack):
            self.max_stack = max_stack
            # Generate text
            font = c.get_scaled_font(-1, c.INV_IMG_W, "Crush!", "Times New Roman")
            text = font.render("Crush!", 1, (255, 255, 255))
            self.text_rect = text.get_rect(centery=c.INV_W * 3 // 2)
            w = max(self.text_rect.w, c.INV_W)
            self.text_rect.centerx = w // 2
            # Draw surface
            s = pg.Surface((w, c.INV_W * 2))
            s.blit(text, self.text_rect)
            # This is where we will take/add items
            inventory = Inventory((1, 1), whitelist=self.ITEMS, max_stack=9)
            inventory.rect = pg.Rect((w - c.INV_W) // 2, 0, c.INV_W, c.INV_W)
            inventory.load(data)
            s.blit(inventory.surface, inventory.rect)

            ActiveUI.__init__(self, s, s.get_rect(), pos=pos, invs={0: inventory})

            self.rect.center = c.screen_center

        def save(self):
            game_vars.write_block_data(self.block_pos, self.invs[0].write())

        def process_events(self, events, mouse, keys):
            pos = pg.mouse.get_pos()
            if self.rect.collidepoint(*pos):
                pos = [pos[0] - self.rect.x, pos[1] - self.rect.y]
                for e in events:
                    if e.type == MOUSEBUTTONUP and e.button == BUTTON_LEFT and \
                            self.text_rect.collidepoint(*pos) and self.invs[0].inv_amnts[0][0] != 0:
                        from Objects import CRUSH_DROPS
                        items = {}
                        item = self.invs[0].inv_items[0][0]
                        # Go through each item and get a random drop
                        while self.invs[0].inv_amnts[0][0] > 0:
                            idx, amnt = CRUSH_DROPS[item](randint(1, 100))
                            if idx in items.keys():
                                items[idx] += amnt
                            else:
                                items[idx] = amnt
                            self.invs[0].inv_amnts[0][0] -= 1
                        # Drop the results
                        block_pos = [self.block_pos[0] * BLOCK_W, self.block_pos[1] * BLOCK_W]
                        for idx, amnt in zip(items.keys(), items.values()):
                            max_stack = game_vars.items[idx].max_stack
                            # Make sure we don't drop more than max stack
                            while amnt > 0:
                                transfer = min(max_stack, amnt)
                                game_vars.drop_item(DroppedItem(idx, transfer), choice([True, False]), block_pos)
                                amnt -= transfer
                        self.save()
                        self.invs[0].update_item(0, 0)

                if game_vars.player.use_time <= 0 and self.invs[0].rect.collidepoint(*pos):
                    pos = [pos[0] - self.invs[0].rect.x, pos[1] - self.invs[0].rect.y]
                    if mouse[BUTTON_LEFT - 1]:
                        self.invs[0].left_click(pos)
                    elif mouse[BUTTON_RIGHT - 1]:
                        self.invs[0].right_click(pos)
                    self.save()

                self.ui.fill((0, 0, 0), self.invs[0].rect)
                self.ui.blit(self.invs[0].surface, self.invs[0].rect)


class UpgradeStation(FunctionalTile):
    def __init__(self):
        super().__init__(t.UPGRADE_STATION, img=INV + "upgrade_station.png", dim=(1, 2))
        self.on_surface = True
        self.add_drop(i.UPGRADE_STATION, 1)
        self.map_color = (200, 0, 200)
        self.hardness = 0
        self.set_animation(self.Anim(self.image))

    def get_block_img(self, data):
        if data and len(data) >= 4:
            amnt = int.from_bytes(data[:2], byteorder)
            if amnt != 0:
                item = int.from_bytes(data[2:4], byteorder)
                return game_vars.animations[self.anim_idx].get_frame(img=game_vars.items[item].inv_img)
        return game_vars.animations[self.anim_idx].get_frame()

    def on_place(self, pos):
        game_vars.write_block_data(pos, bytearray(4))

    def on_break(self, pos):
        data = game_vars.get_block_data(pos)
        if data:
            inv = Inventory((1, 1))
            inv.load(data)
            item, amnt, data = inv.inv_items[0][0], inv.inv_amnts[0][0], inv.inv_data.get((0, 0))
            if amnt != 0:
                game_vars.drop_item(DroppedItem(item, amnt, data=data), c.random_sign(), [p * BLOCK_W for p in pos])
        game_vars.write_block_data(pos, None)
        return True

    def activate(self, pos):
        data = game_vars.get_block_data(pos)
        if data is not None:
            game_vars.player.set_active_ui(self.UI(pos, data))
            if not game_vars.player.inventory.open:
                game_vars.player.inventory.toggle()

    class UI(ActiveUI):
        def __init__(self, pos, data):
            rect = pg.Rect(0, 0, c.MIN_W // 2, c.MIN_H // 2)
            rect.center = c.screen_center
            super().__init__(pg.Surface(rect.size), rect, pos=pos)

            # Set up inventory for item
            whitelist = [item_id for item_id, item in game_vars.items.items() if isinstance(item, Upgradable)]
            self.invs["Item"] = Inventory((1, 1), whitelist=whitelist, max_stack=1)
            self.invs["Item"].rect.move_ip((self.rect.w - c.INV_W) // 2, c.INV_W // 2)
            self.invs["Item"].load(data)
            # Set up upgrade section with offsets, rect, and surface
            self.dragging = self.can_click = False
            self.off_x = self.off_y = 0
            self.max_x = self.max_y = 0
            y = self.invs["Item"].rect.bottom
            self.tree_r = pg.Rect(0, y, self.rect.w, self.rect.h - y)
            self.upgrade_tree = self.tree_s = None
            # Load the data
            self.load_tree()

        def draw(self):
            super().draw()
            # Check for hovering over upgrade tree
            if self.upgrade_tree:
                pos = pg.mouse.get_pos()
                if self.rect.collidepoint(*pos):
                    pos = [pos[0] - self.rect.x, pos[1] - self.rect.y]
                    if self.tree_r.collidepoint(*pos):
                        pos = [pos[0] - self.tree_r.x + self.off_x, pos[1] - self.tree_r.y + self.off_y]
                        self.upgrade_tree.check_hover(pos)

        def save(self):
            if self.upgrade_tree:
                self.invs["Item"].inv_data[(0, 0)] = self.upgrade_tree.write()
            game_vars.write_block_data(self.block_pos, self.invs["Item"].write())

        def draw_inventory(self):
            self.ui.fill((0, 0, 0), self.invs["Item"].rect)
            self.ui.blit(self.invs["Item"].surface, self.invs["Item"].rect)

        def draw_tree(self):
            self.ui.fill((0, 0, 0), self.tree_r)
            if self.tree_s:
                self.ui.blit(self.tree_s, self.tree_r, area=((self.off_x, self.off_y), self.tree_r.size))

        def load_tree(self):
            data = self.invs["Item"].inv_data.get((0, 0))
            item = self.invs["Item"].inv_items[0][0]
            if data and item != -1:
                # Draw upgrade tree and get surface dimensions
                self.upgrade_tree = game_vars.items[item].upgrade_tree
                self.upgrade_tree.load(data)
                self.tree_s = self.upgrade_tree.get_surface()
                s_dim = self.tree_s.get_size()
                # Readjust rectangle
                y = self.invs["Item"].rect.bottom
                self.tree_r = pg.Rect(0, y, min(self.rect.w, s_dim[0]), min(self.rect.h - y, s_dim[1]))
                self.tree_r.centerx = self.rect.w // 2
                # Update max scroll and current scroll
                self.max_x = max(0, s_dim[0] - self.tree_r.w)
                self.off_x = self.max_x // 2
                self.max_y = max(0, s_dim[1] - self.tree_r.h)
                self.off_y = 0
            else:
                self.tree_s = None
            # Draw upgrade tree to ui surface
            self.draw_tree()
            self.draw_inventory()

        def process_events(self, events, mouse, keys):
            # Drag upgrade screen
            if self.dragging and mouse[BUTTON_LEFT - 1]:
                if game_vars.d_mouse.count(0) != 2:
                    self.can_click = False
                self.off_x -= game_vars.d_mouse[0]
                self.off_x = min(max(0, self.off_x), self.max_x)
                self.off_y -= game_vars.d_mouse[1]
                self.off_y = min(max(0, self.off_y), self.max_y)
                self.draw_tree()
            pos = pg.mouse.get_pos()
            if self.rect.collidepoint(*pos):
                pos = [pos[0] - self.rect.x, pos[1] - self.rect.y]
                # Process events
                for e in events:
                    # Start dragging
                    if e.type == MOUSEBUTTONDOWN and e.button == BUTTON_LEFT:
                        self.can_click = True
                        if self.tree_r.collidepoint(*pos):
                            self.dragging = True
                    elif e.type == MOUSEBUTTONUP:
                        if e.button == BUTTON_LEFT:
                            self.dragging = False
                            if self.can_click:
                                # Inventory left click, check for change in item
                                if self.invs["Item"].rect.collidepoint(*pos):
                                    # Save whatever data we have to the item (not the block!)
                                    prev = self.upgrade_tree.write() if self.upgrade_tree else None
                                    self.invs["Item"].set_data_at(0, 0, data=prev)
                                    # Do the left click
                                    pos = [pos[0] - self.invs["Item"].rect.x, pos[1] - self.invs["Item"].rect.y]
                                    self.invs["Item"].left_click(pos)
                                    # Check if we now have new item data
                                    new = self.invs["Item"].inv_data.get((0, 0))
                                    if prev != new:
                                        if new is None:
                                            self.upgrade_tree = None
                                        self.load_tree()
                                        self.save()
                                    self.draw_inventory()
                                # Click on upgrade screen
                                elif self.tree_r.collidepoint(*pos) and self.upgrade_tree:
                                    pos = [pos[0] - self.tree_r.x + self.off_x, pos[1] - self.tree_r.y + self.off_y]
                                    if self.upgrade_tree.click(pos):
                                        self.tree_s = self.upgrade_tree.get_surface()
                                        self.draw_tree()
                                    self.save()
                        elif e.button == BUTTON_RIGHT:
                            # Right click inventory and check for change in item
                            if self.invs["Item"].rect.collidepoint(*pos):
                                self.save()
                                prev = self.invs["Item"].inv_data.get((0, 0))
                                pos = [pos[0] - self.invs["Item"].rect.x, pos[1] - self.invs["Item"].rect.y]
                                self.invs["Item"].right_click(pos)
                                if prev != self.invs["Item"].inv_data.get((0, 0)):
                                    self.load_tree()
                                self.draw_inventory()

    class Anim(Animation):
        def __init__(self, img):
            super().__init__("", [0, 0])
            dim = img.get_size()
            # Set background image
            self.background = img
            # Set up default floating item
            img_w = min(dim[0], dim[1] // 2)
            self.def_img = c.load_image(PROJ + "fire_ball.png", img_w, img_w)
            # Calculate float bounds
            self.min_y, self.max_y = dim[1] // 4, dim[1] // 2
            self.x, self.y = dim[0] // 2, self.max_y
            self.going_up = True

        def update(self):
            dy = (self.max_y - self.min_y) * game_vars.dt / .75
            if self.going_up:
                self.y -= dy
                if self.y <= self.min_y:
                    self.y = self.min_y
                    self.going_up = False
            else:
                self.y += dy
                if self.y >= self.max_y:
                    self.y = self.max_y
                    self.going_up = True

        def get_frame(self, img=None):
            if not img:
                img = self.def_img
            else:
                w, h = self.def_img.get_size()
                img = c.scale_to_fit(img, w=w, h=h)
            result = self.background.copy()
            result.blit(img, img.get_rect(center=(self.x, self.y)))
            return result
