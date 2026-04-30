#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ENCORE].effect == EFFECT_ENCORE);
}

SINGLE_BATTLE_TEST("Panic Shed uses Encore when first dropping below 50 percent HP")
{
    GIVEN {
        PLAYER(SPECIES_ACCELGOR) { Ability(ABILITY_STICKY_HOLD); UniqueAbility(ABILITY_PANIC_SHED); HP(100); MaxHP(160); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_PANIC_SHED);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ENCORE, player);
    }
}

SINGLE_BATTLE_TEST("Panic Shed triggers only once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_ACCELGOR) { Ability(ABILITY_STICKY_HOLD); UniqueAbility(ABILITY_PANIC_SHED); Item(ITEM_SITRUS_BERRY); HP(100); MaxHP(200); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_PANIC_SHED);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_PANIC_SHED);
        }
    }
}

SINGLE_BATTLE_TEST("Panic Shed does not trigger if the user was already below 50 percent HP before the hit")
{
    GIVEN {
        PLAYER(SPECIES_ACCELGOR) { Ability(ABILITY_STICKY_HOLD); UniqueAbility(ABILITY_PANIC_SHED); HP(80); MaxHP(200); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_PANIC_SHED);
        }
    }
}
