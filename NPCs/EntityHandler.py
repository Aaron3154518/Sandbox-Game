# Created on 22 October 2019
# Handles all entities, including loose items

from random import randint
from pygame.display import get_surface
from NPCs.SpawnConditions import SpawnConditions
from Tools.constants import BLOCK_W
from Objects.tile_ids import AIR
from Tools import objects as o


class EntityHandler:
    def __init__(self):
        self.entities = []
        self.items = []

    def move(self, player_pos):
        for entity in self.entities:
            entity.move(player_pos)
        for item in self.items:
            item.move()
            if item.pick_up_immunity <= 0 and o.player.pick_up(item):
                self.items.remove(item)

    def check_hit_entities(self, player_x, polygon, damage):
        for entity in self.entities:
            # Check if we are swinging a weapon and the entity can be hit
            # and our sword collides with the entity
            if entity.immunity <= 0 and polygon.collides_polygon(entity.rect):
                # Check if we killed the entity
                if entity.hit(damage, player_x):
                    self.entities.remove(entity)

    def check_hit_player(self, player_rect):
        for entity in self.entities:
            if player_rect.colliderect(entity.rect):
                return entity.stats.dmg, entity.rect.centerx
        return 0, 0

    def get_display(self, rect):
        display = get_surface()
        for entity in self.entities:
            if rect.colliderect(entity.rect):
                display.blit(entity.surface, (entity.pos[0] - rect.x, entity.pos[1] - rect.y))
        for item in self.items:
            if rect.colliderect(item.rect):
                display.blit(item.item.image, (item.pos[0] - rect.x, item.pos[1] - rect.y))

    def collides_with_entity(self, rect):
        for entity in self.entities:
            if entity.rect.colliderect(rect):
                return True
        return False

    def spawn(self, player_pos):
        conditions = SpawnConditions()
        conditions.check_world()
        player_pos = [i // BLOCK_W for i in player_pos]
        spawners = o.world.spawners
        for x in range(max(0, player_pos[0] - 50), min(o.world.dim[0], player_pos[0] + 50)):
            if x in spawners.keys():
                for y in range(max(0, player_pos[1] - 50, min(o.world.dim[1], player_pos[1] + 50))):
                    if y in spawners[x].keys() and randint(1, 10000) == 1:
                        entity = o.tiles[spawners[x][y]].spawn((x, y), conditions)
                        if entity is not None:
                            self.entities.append(entity)


# Maybe used in future
def get_spawn_spaces_with_hole(center, r_inner, r_outer, walking):
    # X range
    v1_min, v1_max = max(0, center[0] - r_outer), min(o.world.dim[0], center[0] + r_outer)
    # Y bounds
    v2_min1, v2_max1 = max(0, center[1] - r_outer), max(0, center[1] - r_inner)
    v2_min2, v2_max2 = min(o.world.dim[1], center[1] + r_inner), min(o.world.dim[1],
                                                                     center[1] + r_outer)
    v2_range_full = (range(v2_min1, v2_max2))
    v2_range_parts = (range(v2_min1, v2_max1), range(v2_min2, v2_max2))
    air = {}

    def add_val(v1_, v2_, val):
        if v1_ not in air.keys():
            air[v1_] = {}
        air[v1_][v2_] = val

    blocks = o.world.blocks
    for v1 in range(v1_min, v1_max):
        for v2_range in v2_range_full if abs(v1 - center[1]) <= r_inner else \
                v2_range_parts:
            air_count = 0
            hit_block = False
            v2 = 0
            for v2 in v2_range:
                block = blocks[v2][v1] if walking else blocks[v1][v2]
                if block != AIR:
                    hit_block = True
                    if air_count > 0:
                        add_val(v1, v2 - air_count, air_count)
                    air_count = 0
                elif hit_block:
                    air_count += 1
            if air_count > 0:
                add_val(v1, v2 - air_count, air_count)
    return air
