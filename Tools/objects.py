# Created on 3 December 2019
# Contains variable data that multiple objects need

# BE WARY OF IMPORT LOOPS!!!
from sys import exit
import pygame as pg
from Tools.constants import MS_PER_DAY, NOON
from UI.Operations import CompleteTask

items, tiles = {}, {}
player, world = None, None
# World time
world_time = MS_PER_DAY * .4
# Amount of time since last update
dt = 0


def init():
    from inspect import getmembers, isclass
    from Player.Player import Player
    from Objects import ItemObjects, TileObjects
    from World.World import World
    from World.WorldSelector import run_selector, PLAYER, UNIVERSE

    # Compile a list of items
    items.clear()
    for name, obj in getmembers(ItemObjects):
        if isclass(obj):
            if "Objects.ItemObjects" in str(obj):
                instance = obj()
                items[instance.idx] = instance

    # Compile a list of tiles
    tiles.clear()
    for name, obj in getmembers(TileObjects):
        if isclass(obj):
            if "Objects.TileObjects" in str(obj):
                instance = obj()
                tiles[instance.idx] = instance

    global player, world
    player = Player(run_selector(PLAYER))
    player.load()

    universe_name = run_selector(UNIVERSE)
    world = World(universe_name, universe_name)


def get_sky_color():
    return 0, 0, 255 * (1 - pow((world_time - NOON) / NOON, 2))


def tick(delta_time):
    global world_time, dt
    dt = delta_time
    world_time = (world_time + dt) % MS_PER_DAY


# Closes current world
def close_world():
    CompleteTask(world.save_part, task_args=[2], draw_args=("Saving World",), can_exit=False).run_now()
    player.write()


# Loads a new world
def load_world():
    if not CompleteTask(world.load_part, task_args=[3], draw_args=("Loading World Blocks",)).run_now():
        pg.quit()
        exit(0)
    world.draw_blocks()
    player.spawn()


# Changes world
def change_world(new_world):
    def screen_goes_white(progress):
        display = pg.display.get_surface()
        player.draw_ui()
        overlay = pg.Surface(display.get_size())
        overlay.fill((255 * progress, 255 * progress, 255))
        overlay.set_alpha(255 * progress)
        display.blit(overlay, (0, 0))

    CompleteTask(world.save_part, task_args=[10], can_exit=False,
                 draw_ui=screen_goes_white, draw_args=()).run_now()
    player.write()
    world.name = new_world
    load_world()
