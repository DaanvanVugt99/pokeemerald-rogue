#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_RECOVER].split == SPLIT_STATUS);
    ASSUME(gBattleMoves[MOVE_TACKLE].split != SPLIT_STATUS);
}

SINGLE_BATTLE_TEST("Evergreen does not trigger on the first turn out")
{
    GIVEN {
        PLAYER(SPECIES_CHIKORITA)   { Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_VERDANT_HAVEN); Status1(STATUS1_POISON); Moves(MOVE_RECOVER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_VERDANT_HAVEN);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_AROMATHERAPY, player);
        }
    } THEN {
        EXPECT(player->status1 & STATUS1_POISON);
    }
}

SINGLE_BATTLE_TEST("Evergreen uses Aromatherapy after turn 1 when using status moves")
{
    GIVEN {
        PLAYER(SPECIES_CHIKORITA)   { Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_VERDANT_HAVEN); Status1(STATUS1_POISON); Moves(MOVE_RECOVER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_VERDANT_HAVEN);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AROMATHERAPY, player);
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Evergreen does not trigger on non-status moves")
{
    GIVEN {
        PLAYER(SPECIES_CHIKORITA)   { Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_VERDANT_HAVEN); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_VERDANT_HAVEN);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_AROMATHERAPY, player);
        }
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Evergreen can trigger repeatedly after turn 1")
{
    GIVEN {
        PLAYER(SPECIES_MEGANIUM)     { Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_VERDANT_HAVEN); Moves(MOVE_RECOVER); }
        OPPONENT(SPECIES_WOBBUFFET)  { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_VERDANT_HAVEN);
        ABILITY_POPUP(player, ABILITY_VERDANT_HAVEN);
    }
}
