#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].split == SPLIT_PHYSICAL);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].split == SPLIT_SPECIAL);
    ASSUME(gBattleMoves[MOVE_SCARY_FACE].effect == EFFECT_SPEED_DOWN_2);
}

SINGLE_BATTLE_TEST("Black Ice uses Scary Face when this Pokemon loses Ice Face")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_EISCUE) { Speed(1); Ability(ABILITY_ICE_FACE); UniqueAbility(ABILITY_BLACK_ICE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
        ABILITY_POPUP(opponent, ABILITY_ICE_FACE);
        MESSAGE("Foe Eiscue transformed!");
        ABILITY_POPUP(opponent, ABILITY_BLACK_ICE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCARY_FACE, opponent);
    } THEN {
        EXPECT_EQ(opponent->species, SPECIES_EISCUE_NOICE_FACE);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 2);
    }
}

SINGLE_BATTLE_TEST("Black Ice does not trigger if Ice Face is not lost")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_EISCUE) { Speed(1); Ability(ABILITY_ICE_FACE); UniqueAbility(ABILITY_BLACK_ICE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(opponent, ABILITY_BLACK_ICE);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SCARY_FACE, opponent);
        }
    } THEN {
        EXPECT_EQ(opponent->species, SPECIES_EISCUE_ICE_FACE);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}
