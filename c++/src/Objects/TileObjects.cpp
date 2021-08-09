#include "TileObjects.h"

// Dragon Egg
ADD_TILE(DragonEgg)

DragonEgg::DragonEgg() {

}

void DragonEgg::onBreak() {

}

// Portal
ADD_TILE(Portal)

Portal::Portal() {

}

void Portal::summon() {
    /*data = game_vars.get_block_data(pos)
        if data:
    magic = int.from_bytes(data[:8], byteorder)
        # Must have at least 5 magic
        if magic < 5:
    return
        # Every X5 for magic increases max level by 1
        max_level = math.log(magic, 5)
        chance = max(max_level % 1, .5)
        max_level = int(max_level)
        # Find chances for current leveland up to two levels below
        if max_level == 1:
    chances = { 1: 1 }
        elif max_level == 2 :
        chances = { 2: chance, 1 : 1 - chance }
        else :
            chances = { max_level: chance, max_level - 1 : (1 - chance) * 2 / 3,
                       max_level - 2 : (1 - chance) / 3 }
            # Choose a random level
            num = uniform(0, 1)
            for lvl, chance in chances.items() :
                if num > chance:
    num -= chance
                else :
                    # Summon mage
                    # element = choice(list(ItemTypes.MagicContainer.ELEMENT_NAMES.keys()))
                    game_vars.spawn_entity(mobs.Mage(ItemTypes.MagicContainer.FIRE, lvl), [p * BLOCK_W for p in pos])
                    break
                    magic = int(magic / 2)
                    game_vars.write_block_data(pos, magic.to_bytes(8, byteorder))*/
}

void Portal::onPlace() {
    //game_vars.write_block_data(pos, bytearray(8))
}

void Portal::tick(int x, int y, Timestep dt) {
    /*magic = 0
        rect = pg.Rect(x * BLOCK_W, y * BLOCK_W, self.dim[0] * BLOCK_W, self.dim[1] * BLOCK_W)
        items = game_vars.handler.items
        for item in items :
    if item.item.magic_value > 0 and item.rect.colliderect(rect) :
        magic += item.item.magic_value * item.info.amnt
        items.remove(item)
        if magic > 0:
    data = game_vars.get_block_data((x, y))
        if data :
            current_magic = int.from_bytes(data[:8], byteorder)
            game_vars.write_block_data((x, y), (current_magic + magic).to_bytes(8, byteorder))*/
}