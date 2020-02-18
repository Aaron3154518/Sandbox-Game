# Created on 4 December 2019

from sys import byteorder
from random import choice
from pygame.locals import *
from Objects.TileTypes import *
from Objects.ItemTypes import Block, create_block_from_zip
from Objects.DroppedItem import DroppedItem
from Objects import INV
from NPCs import Mobs as mobs
from Player.ActiveUI import ActiveUI
from Player.Inventory import Inventory
from Tools import constants as c, item_ids as i, tile_ids as t
from Tools import objects as o
from World import WorldGenerator
from World.World import World
from UI.Operations import TextInput


# Normal Tiles
class Air(Tile):
    def __init__(self):
        super().__init__(t.AIR)


class Dirt(Tile):
    def __init__(self):
        super().__init__(t.DIRT, hardness=1, img=INV + "dirt.png")
        self.add_drop(i.DIRT, 1)
        self.map_color = (150, 75, 0)
        Block(i.DIRT, t.DIRT, name="Dirt", img=INV + "dirt.png")


class Stone(Tile):
    def __init__(self):
        super().__init__(t.STONE, hardness=2, img=INV + "stone.png")
        self.add_drop(i.STONE, 1)
        self.map_color = (200, 200, 200)
        Block(i.STONE, t.STONE, name="Stone", img=INV + "stone.png")


class Snow(Tile):
    def __init__(self):
        super().__init__(t.SNOW, hardness=1, img=INV + "snow.png")
        self.add_drop(i.SNOW_BALL, 2, 5)
        self.map_color = (255, 255, 255)
        Block(i.SNOW, t.SNOW, name="Snow", img=INV + "snow.png")


class Wood(Tile):
    def __init__(self):
        super().__init__(t.WOOD, hardness=1, img=INV + "wood_tile.png")
        self.add_drop(i.WOOD, 1)
        self.map_color = (100, 50, 0)
        Block(i.WOOD, t.WOOD, name="Wood", img=INV + "wood_item.png")


class Leaves(Tile):
    def __init__(self):
        super().__init__(t.LEAVES, hardness=0, img=INV + "leaves.png")
        self.add_drop(i.LEAVES, 1)
        self.map_color = (0, 0, 100)
        Block(i.LEAVES, t.LEAVES, name="Leaves", img=INV + "leaves.png")


class Boulder1(Tile):
    def __init__(self):
        super().__init__(t.BOULDER1, hardness=0, img=INV + "boulder_1.png")
        self.add_drop(i.STONE, 0, max_amnt=1)
        self.map_color = (100, 100, 64)


class Boulder2(Tile):
    def __init__(self):
        super().__init__(t.BOULDER2, hardness=0, img=INV + "boulder_2.png", dim=(2, 1))
        self.map_color = (100, 100, 64)
        self.add_drop(i.STONE, 0, max_amnt=2)


class Boulder3(Tile):
    def __init__(self):
        super().__init__(t.BOULDER3, hardness=0, img=INV + "boulder_3.png", dim=(3, 2))
        self.map_color = (100, 100, 64)
        self.add_drop(i.STONE, 1, max_amnt=3)


class ShinyStone1(Tile):
    def __init__(self):
        super().__init__(t.SHINY_STONE_1, hardness=1, img=INV + "shiny_stone_1.png")
        self.add_drop(i.SHINY_STONE_1, 1)
        self.map_color = (175, 175, 175)
        Block(i.SHINY_STONE_1, t.SHINY_STONE_1, name="Shiny Stone: Tier 1",
              img=INV + "shiny_stone_1.png")


class ShinyStone2(Tile):
    def __init__(self):
        super().__init__(t.SHINY_STONE_2, hardness=1, img=INV + "shiny_stone_2.png")
        self.add_drop(i.SHINY_STONE_2, 1)
        self.map_color = (150, 150, 150)
        Block(i.SHINY_STONE_2, t.SHINY_STONE_2, name="Shiny Stone: Tier 2",
              img=INV + "shiny_stone_2.png")


class ShinyStone3(Tile):
    def __init__(self):
        super().__init__(t.SHINY_STONE_3, hardness=1, img=INV + "shiny_stone_3.png")
        self.add_drop(i.SHINY_STONE_3, 1)
        self.map_color = (125, 125, 125)
        Block(i.SHINY_STONE_3, t.SHINY_STONE_3, name="Shiny Stone: Tier 3",
              img=INV + "shiny_stone_3.png")


class DragonEgg(Tile):
    def __init__(self):
        super().__init__(t.DRAGON_EGG, hardness=3, img=INV + "dragon_egg.png")
        self.add_drop(i.DRAGON_EGG, 1)
        self.map_color = (64, 200, 0)
        Block(i.DRAGON_EGG, t.DRAGON_EGG, name="Dragon Egg", img=INV + "dragon_egg.png")

    def on_break(self, pos):
        d = mobs.Dragon()
        d.set_pos((pos[0] + c.random_sign() * randint(10, 20)) * BLOCK_W,
                  (pos[1] + c.random_sign() * randint(10, 20)) * BLOCK_W)
        o.player.handler.entities.append(d)
        return False


# End


# Crafting Stations
class WorkTable(CraftingStation):
    def __init__(self):
        super().__init__(t.WORK_TABLE, dim=(2, 1), img=INV + "work_table.png")
        self.on_surface = True
        self.map_color = (54, 78, 154)
        self.add_drop(i.WORK_TABLE, 1)
        Block(i.WORK_TABLE, t.WORK_TABLE, name="Work Table", img=INV + "work_table.png")

    def get_recipes(self):
        return [[[i.SNOW, 1], [i.SNOW_BALL, 4]],
                [[i.FOREST, 1], [i.DIRT, 50], [i.CAT, 1]],
                [[i.MOUNTAIN, 1], [i.STONE, 10], [i.SNOW, 15]],
                [[i.VALLEY, 1], [i.STONE, 50], [i.ZOMBIE, 1]]]


# Spawners
class CatSpawner(SpawnTile):
    def __init__(self):
        super().__init__(t.CAT, mobs.Cat, item_id=i.CAT)
        self.add_drop(i.CAT, 1)


class ZombieSpawner(SpawnTile):
    def __init__(self):
        super().__init__(t.ZOMBIE, mobs.Zombie, item_id=i.ZOMBIE)
        self.add_drop(i.ZOMBIE, 1)


class DoomBunnySpawner(SpawnTile):
    def __init__(self):
        super().__init__(t.DOOM_BUNNY, mobs.DoomBunny, item_id=i.DOOM_BUNNY)
        self.add_drop(i.DOOM_BUNNY, 1)


class HelicopterSpawner(SpawnTile):
    def __init__(self):
        super().__init__(t.HELICOPTER, mobs.Helicopter, item_id=i.HELICOPTER)
        self.add_drop(i.HELICOPTER, 1)
        self.map_color = (0, 0, 80)


class BirdieSpawner(SpawnTile):
    def __init__(self):
        super().__init__(t.BIRDIE, mobs.Birdie, item_id=i.BIRDIE)
        self.add_drop(i.BIRDIE, 1)
        self.map_color = (0, 0, 80)


# End


# Tiles that do stuff
class DimensionHopper(FunctionalTile):
    def __init__(self):
        super().__init__(t.DIMENSION_HOPPER, 0, hardness=2, img=INV + "dimension_hopper.png")
        self.scroller = None
        self.add_drop(i.DIMENSION_HOPPER, 1)
        self.map_color = (0, 0, 0)
        Block(i.DIMENSION_HOPPER, t.DIMENSION_HOPPER, name="Dimension Hopper",
              img=INV + "dimension_hopper.png")

    def activate(self, pos):
        o.player.active_ui = self.UI(pos)

    class UI(ActiveUI):
        def __init__(self, pos):
            from os import listdir
            from Tools.VerticalScroller import VerticalScroller

            dim = (c.MIN_W // 2, c.MIN_H * 3 // 4)

            ActiveUI.__init__(self, pg.Surface(dim), pg.Rect((0, 0), dim), pos=pos)
            self.on_resize()
            self.scroller = VerticalScroller(self.rect.size, background=(0, 200, 128))

            font = c.get_scaled_font(self.rect.w, int(self.rect.h / 8), "_" * 25, "Times New Roman")
            for file in listdir("saves/universes/" + o.world.universe):
                if file.endswith(".wld"):
                    name = file[:-4]
                    text = font.render(name, 1, (255, 255, 255))
                    self.scroller.add_item(text, name)

            self.ui.blit(self.scroller.surface, (0, 0))
            pg.draw.rect(self.ui, (0, 0, 0), self.ui.get_rect(), 2)

        def process_events(self, events, mouse, keys):
            for e in events:
                if e.type == BUTTON_WHEELUP or e.type == BUTTON_WHEELDOWN:
                    self.scroller.do_scroll(e.type == BUTTON_WHEELUP)
                    self.ui.blit(self.scroller.surface, (0, self.scroller.scroll))
                    pg.draw.rect(self.ui, (0, 0, 0), self.ui.get_rect(), 2)
                elif e.type == MOUSEBUTTONUP and e.button == BUTTON_LEFT:
                    pos = pg.mouse.get_pos()
                    if self.rect.collidepoint(*pos):
                        mouse[BUTTON_LEFT - 1] = False
                        pos = (pos[0] - self.rect.x, pos[1] - self.rect.y)
                        world = self.scroller.click(pos)
                        if world != "":
                            o.change_world(world)
                            o.player.active_ui = None
                    else:
                        continue
                elif e.type == KEYUP and e.key == K_ESCAPE:
                    keys[K_ESCAPE] = False
                    o.player.active_ui = None
                else:
                    continue

        def on_resize(self):
            self.rect.center = pg.display.get_surface().get_rect().center


class WorldBuilder(FunctionalTile):
    INV_SPOTS = 6

    def __init__(self):
        super().__init__(t.WORLD_BUILDER, 4 * self.INV_SPOTS, img=INV + "world_builder.zip")
        self.on_surface = True
        self.add_drop(i.WORLD_BUILDER, 1)
        self.map_color = (0, 0, 0)
        create_block_from_zip(i.WORLD_BUILDER, t.WORLD_BUILDER, "World Builder",
                              INV + "world_builder.zip")

    def activate(self, pos):
        data = c.get_from_dict(*pos, o.world.block_data)
        if data is not None:
            o.player.active_ui = self.UI(pos, data)

    def on_place(self, pos):
        from Player.Inventory import new_inventory
        c.update_dict(*pos, new_inventory((1, self.INV_SPOTS)), o.world.block_data)

    def on_break(self, pos):
        data = c.get_from_dict(*pos, o.world.block_data)
        if data is not None:
            # Check if we have any i
            for byte in data:
                if byte != 0:
                    return False
            # If not, remove our data
            c.remove_from_dict(*pos, o.world.block_data)
        return True

    class UI(ActiveUI):
        WRITE_ORDER = ["Biome", "Structure", "Size"]

        def __init__(self, pos, data):
            self.name = ""
            self.cursor = False
            # Load inventories and rectangles
            self.invs = {"Biome": Inventory((2, 2), max_stack=1, items_list=o.biomes.keys()),
                         "Structure": Inventory((1, 1), max_stack=4, items_list=[i.BONUS_STRUCTURE]),
                         "Size": Inventory((1, 1), max_stack=1, items_list=WorldGenerator.WORLD_DIMS.keys())}
            for j, key in enumerate(self.WRITE_ORDER):
                # Load data
                num_bytes = self.invs[key].num_bytes
                self.invs[key].load(data[:num_bytes])
                data = data[num_bytes:]
            # Set rectangles
            text_h = c.INV_W // 2
            # Text
            text = {"Biome": "Biome Cards",
                    "Structure": "Bonus Structure",
                    "Size": "Size Card"}
            # Get font and inventory section width
            longest = c.get_widest_string(text.values())
            font = c.get_scaled_font(-1, text_h, longest)
            inv_w = font.size(longest)[0]
            # Get all surfaces to draw
            surfaces = []
            y = 0
            for key in self.WRITE_ORDER:
                text_s = font.render(text[key], 1, (255, 255, 255))
                text_rect = text_s.get_rect(centerx=inv_w // 2, centery=y + text_h // 2)
                surfaces.append([text_s, text_rect])
                y += text_h
                inv = self.invs[key]
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
            super().__init__(s, s.get_rect(), pos=pos)
            self.on_resize()

            if not o.player.inventory.open:
                o.player.inventory.toggle()

        @property
        def data(self):
            data = bytearray()
            for key in self.WRITE_ORDER:
                data += self.invs[key].write()
            return data

        def process_events(self, events, mouse, keys):
            temp = self.cursor
            self.cursor = (pg.time.get_ticks() // 400) % 2 == 0
            if temp != self.cursor:
                self.draw_name()
            pos = pg.mouse.get_pos()
            if self.rect.collidepoint(*pos) and o.player.use_time <= 0:
                pos = [pos[0] - self.rect.x, pos[1] - self.rect.y]
                # Clicked create
                if self.create_rect.collidepoint(*pos):
                    if not self.invs["Biome"].is_empty() and not self.invs["Size"].is_empty() and self.name != "":
                        for e in events:
                            if e.type == MOUSEBUTTONUP and e.button == BUTTON_LEFT:
                                events.remove(e)
                                new = World(o.world.universe, self.name)
                                items = self.invs["Biome"].inv_items.flatten().tolist()
                                items += [self.invs["Size"].inv_items[0][0]]
                                items += [i.BONUS_STRUCTURE] * self.invs["Structure"].inv_amnts[0][0]
                                WorldGenerator.generate_world(new, modifiers=items)
                                del new
                                from Player.Inventory import new_inventory
                                c.update_dict(*self.block_pos, new_inventory((1, WorldBuilder.INV_SPOTS)),
                                              o.world.block_data)
                                o.player.active_ui = None
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
                            c.update_dict(*self.block_pos, self.data, o.world.block_data)
                            break
            if keys[K_ESCAPE]:
                o.player.active_ui = None
                keys[K_ESCAPE] = False
            for e in events:
                if e.type == KEYDOWN:
                    if e.key == K_BACKSPACE:
                        self.name = self.name[:-1]
                    elif e.key == K_SPACE:
                        self.name += " "
                    elif len(pg.key.name(e.key)) == 1:
                        self.name += e.unicode
                    else:
                        continue
                    self.draw_name()

        def draw_name(self):
            font = c.get_scaled_font(*self.name_rect.size, self.name + "|")
            text = font.render(self.name + ("|" if self.cursor else ""), 1, (255, 255, 255))
            text_rect = text.get_rect(center=self.name_rect.center)
            self.ui.fill((0, 0, 0), self.name_rect)
            self.ui.blit(text, text_rect)

        def on_resize(self):
            self.rect.center = pg.display.get_surface().get_rect().center


class Chest(FunctionalTile):
    INV_DIM = (10, 5)

    def __init__(self):
        super().__init__(t.CHEST, 4 * self.INV_DIM[0] * self.INV_DIM[1], hardness=1, img=INV + "chest.png",
                         dim=(2, 2))
        self.on_surface = True
        self.add_drop(i.CHEST, 1)
        self.map_color = (200, 200, 0)
        Block(i.CHEST, t.CHEST, name="Chest", img=INV + "chest.png")

    def on_place(self, pos):
        from Player.Inventory import new_inventory
        c.update_dict(*pos, new_inventory(self.INV_DIM), o.world.block_data)

    def on_break(self, pos):
        data = c.get_from_dict(*pos, o.world.block_data)
        if data is not None:
            # Check if we have any i
            for byte in data:
                if byte != 0:
                    return False
            # If not, remove our data
            c.remove_from_dict(*pos, o.world.block_data)
        return True

    def activate(self, pos):
        data = c.get_from_dict(*pos, o.world.block_data)
        if data is not None:
            o.player.active_ui = self.UI(pos, data)

    class UI(ActiveUI):
        def __init__(self, pos, data):
            self.inventory = Inventory(Chest.INV_DIM)
            ActiveUI.__init__(self, self.inventory.surface,
                              self.inventory.rect.move(0, self.inventory.rect.h), pos=pos)
            self.inventory.load(data)

            if not o.player.inventory.open:
                o.player.inventory.toggle()

        def process_events(self, events, mouse, keys):
            pos = pg.mouse.get_pos()
            if self.rect.collidepoint(*pos):
                pos = [pos[0] - self.rect.x, pos[1] - self.rect.y]
                if o.player.use_time <= 0:
                    if mouse[BUTTON_LEFT - 1]:
                        self.inventory.left_click(pos)
                    elif mouse[BUTTON_RIGHT - 1]:
                        self.inventory.right_click(pos)
                    c.update_dict(*self.block_pos, self.inventory.write(), o.world.block_data)
            if keys[K_ESCAPE]:
                o.player.active_ui = None
                keys[K_ESCAPE] = False


class Crusher(FunctionalTile):
    def __init__(self):
        super().__init__(t.CRUSHER, 4, img=INV + "crusher.zip", dim=(2, 2))
        self.on_surface = True
        self.add_drop(i.CRUSHER, 1)
        self.map_color = (64, 64, 64)
        create_block_from_zip(i.CRUSHER, t.CRUSHER, "Crusher", INV + "crusher.zip")

    def on_place(self, pos):
        c.update_dict(*pos, (0).to_bytes(4, byteorder), o.world.block_data)

    def on_break(self, pos):
        data = c.get_from_dict(*pos, o.world.block_data)
        if data is not None:
            amnt = int.from_bytes(data[:2], byteorder)
            if amnt != 0:
                # Drop contents
                from random import choice
                item = int.from_bytes(data[2:4], byteorder)
                o.player.drop_item(DroppedItem(item, amnt), choice([True, False]),
                                   [pos[0] * BLOCK_W, pos[1] * BLOCK_W])
        c.remove_from_dict(*pos, o.world.block_data)
        return True

    def activate(self, pos):
        data = c.get_from_dict(*pos, o.world.block_data)
        if data is not None:
            o.player.active_ui = self.UI(pos, data, 9)
            if not o.player.inventory.open:
                o.player.inventory.toggle()

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
            self.inventory = Inventory((1, 1), items_list=self.ITEMS, max_stack=9)
            self.inventory.rect = pg.Rect((w - c.INV_W) // 2, 0, c.INV_W, c.INV_W)
            self.inventory.load(data)
            s.blit(self.inventory.surface, self.inventory.rect)

            ActiveUI.__init__(self, s, s.get_rect(), pos=pos)

            self.on_resize()

        def on_resize(self):
            self.rect.center = pg.display.get_surface().get_rect().center

        def process_events(self, events, mouse, keys):
            pos = pg.mouse.get_pos()
            if self.rect.collidepoint(*pos):
                pos = [pos[0] - self.rect.x, pos[1] - self.rect.y]
                for e in events:
                    if e.type == MOUSEBUTTONUP and e.button == BUTTON_LEFT and \
                            self.text_rect.collidepoint(*pos) and self.inventory.inv_amnts[0][0] != 0:
                        from Objects import CRUSH_DROPS
                        items = {}
                        item = self.inventory.inv_items[0][0]
                        # Go through each item and get a random drop
                        while self.inventory.inv_amnts[0][0] > 0:
                            idx, amnt = CRUSH_DROPS[item](randint(1, 100))
                            if idx in items.keys():
                                items[idx] += amnt
                            else:
                                items[idx] = amnt
                            self.inventory.inv_amnts[0][0] -= 1
                        # Drop the results
                        block_pos = [self.block_pos[0] * BLOCK_W, self.block_pos[1] * BLOCK_W]
                        for idx, amnt in zip(items.keys(), items.values()):
                            max_stack = o.items[idx].max_stack
                            # Make sure we don't drop more than max stack
                            while amnt > 0:
                                transfer = min(max_stack, amnt)
                                item_obj = DroppedItem(idx, transfer)
                                o.player.drop_item(item_obj, choice([True, False]), block_pos)
                                amnt -= transfer
                        c.update_dict(*self.block_pos, self.inventory.write(), o.world.block_data)
                        self.inventory.update_item(0, 0)

                if o.player.use_time <= 0 and self.inventory.rect.collidepoint(*pos):
                    pos = [pos[0] - self.inventory.rect.x, pos[1] - self.inventory.rect.y]
                    if mouse[BUTTON_LEFT - 1]:
                        self.inventory.left_click(pos)
                    elif mouse[BUTTON_RIGHT - 1]:
                        self.inventory.right_click(pos)
                    c.update_dict(*self.block_pos, self.inventory.write(), o.world.block_data)

                self.ui.fill((0, 0, 0), self.inventory.rect)
                self.ui.blit(self.inventory.surface, self.inventory.rect)

            if keys[K_ESCAPE]:
                c.update_dict(*self.block_pos, self.inventory.write(), o.world.block_data)
                o.player.active_ui = None

# End
