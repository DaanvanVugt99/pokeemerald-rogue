#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Rift lets the first damaging move after switch-in ignore Protect")
{
    GIVEN {
        PLAYER(SPECIES_HOOPA) { Ability(ABILITY_MAGICIAN); UniqueAbility(ABILITY_RIFT); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        MESSAGE("Foe Wobbuffet protected itself!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
        NOT MESSAGE("Foe Wobbuffet protected itself!");
    }
}

SINGLE_BATTLE_TEST("Rift is not consumed by status moves")
{
    GIVEN {
        PLAYER(SPECIES_HOOPA) { Ability(ABILITY_MAGICIAN); UniqueAbility(ABILITY_RIFT); Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_PROTECT, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        MESSAGE("Foe Wobbuffet protected itself!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
        NOT MESSAGE("Foe Wobbuffet protected itself!");
    }
}

SINGLE_BATTLE_TEST("Rift only lets the first damaging move after switch-in ignore Protect")
{
    GIVEN {
        PLAYER(SPECIES_HOOPA) { Ability(ABILITY_MAGICIAN); UniqueAbility(ABILITY_RIFT); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE, MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_TACKLE); }
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        MESSAGE("Foe Wobbuffet protected itself!");
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
            HP_BAR(opponent);
        }
    }
}

SINGLE_BATTLE_TEST("Rift is active on all Hoopa forms")
{
    u16 species;

    PARAMETRIZE { species = SPECIES_HOOPA_CONFINED; }
    PARAMETRIZE { species = SPECIES_HOOPA_UNBOUND; }

    GIVEN {
        PLAYER(species) { Ability(ABILITY_MAGICIAN); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        MESSAGE("Foe Wobbuffet protected itself!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
        NOT MESSAGE("Foe Wobbuffet protected itself!");
    }
}
