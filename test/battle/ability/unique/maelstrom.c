#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_RECOVER));
    ASSUME(IS_MOVE_STATUS(MOVE_STRENGTH_SAP));
    ASSUME(gBattleMoves[MOVE_WHIRLPOOL].effect == EFFECT_TRAP);
}

SINGLE_BATTLE_TEST("Maelstrom uses Whirlpool after Recover")
{
    GIVEN {
        PLAYER(SPECIES_JELLICENT) { HP(50); MaxHP(100); Ability(ABILITY_WATER_ABSORB); UniqueAbility(ABILITY_MAELSTROM); Moves(MOVE_RECOVER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MAELSTROM);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WHIRLPOOL, player);
    }
}

SINGLE_BATTLE_TEST("Maelstrom uses Whirlpool after Strength Sap")
{
    GIVEN {
        PLAYER(SPECIES_JELLICENT) { HP(50); MaxHP(100); Ability(ABILITY_WATER_ABSORB); UniqueAbility(ABILITY_MAELSTROM); Moves(MOVE_STRENGTH_SAP); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_STRENGTH_SAP); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MAELSTROM);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WHIRLPOOL, player);
    }
}

SINGLE_BATTLE_TEST("Maelstrom does not activate after other status moves")
{
    GIVEN {
        PLAYER(SPECIES_JELLICENT) { Ability(ABILITY_WATER_ABSORB); UniqueAbility(ABILITY_MAELSTROM); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_MAELSTROM);
    }
}
