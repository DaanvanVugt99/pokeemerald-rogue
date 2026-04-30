#include "global.h"
#include "pokemon.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Trash Heap uses Recycle at end of turn after the user uses an item")
{
    GIVEN {
        PLAYER(SPECIES_GARBODOR) { Ability(ABILITY_STENCH); UniqueAbility(ABILITY_TRASH_HEAP); Item(ITEM_ORAN_BERRY); Moves(MOVE_FLING); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FLING); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLING, player);
        ABILITY_POPUP(player, ABILITY_TRASH_HEAP);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECYCLE, player);
    } THEN {
        EXPECT_EQ(player->item, ITEM_ORAN_BERRY);
    }
}

SINGLE_BATTLE_TEST("Trash Heap does not trigger before the user has used an item")
{
    GIVEN {
        PLAYER(SPECIES_GARBODOR) { Ability(ABILITY_STENCH); UniqueAbility(ABILITY_TRASH_HEAP); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_TRASH_HEAP);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_RECYCLE, player);
        }
    }
}
