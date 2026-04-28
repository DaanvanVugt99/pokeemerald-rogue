#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_MAGICAL_LEAF].type == TYPE_GRASS);
    ASSUME(gBattleMoves[MOVE_ICY_WIND].type == TYPE_ICE);
    ASSUME(gBattleMoves[MOVE_MAGICAL_LEAF].power > 20);
}

#if B_SNOW_WARNING < GEN_9
SINGLE_BATTLE_TEST("White Canopy summons hail on switch-in")
#elif B_SNOW_WARNING >= GEN_9
SINGLE_BATTLE_TEST("White Canopy summons snow on switch-in")
#endif
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_WHITE_CANOPY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_WHITE_CANOPY);
        #if B_SNOW_WARNING < GEN_9
        MESSAGE("It started to hail!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HAIL_CONTINUES);
        #elif B_SNOW_WARNING >= GEN_9
        MESSAGE("It started to snow!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_SNOW_CONTINUES);
        #endif
    }
}

SINGLE_BATTLE_TEST("White Canopy adds a 20 BP Icy Wind after Grass-type moves in Snow/Hail")
{
    s16 firstHit;
    s16 secondHit;

    GIVEN {
        PLAYER(SPECIES_ABOMASNOW) { Ability(ABILITY_SOUNDPROOF); UniqueAbility(ABILITY_WHITE_CANOPY); Moves(MOVE_MAGICAL_LEAF); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_MAGICAL_LEAF); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MAGICAL_LEAF, player);
        HP_BAR(opponent, captureDamage: &firstHit);
        ABILITY_POPUP(player, ABILITY_WHITE_CANOPY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ICY_WIND, player);
        HP_BAR(opponent, captureDamage: &secondHit);
    } THEN {
        EXPECT_GT(firstHit, 0);
        EXPECT_GT(secondHit, 0);
        EXPECT_LT(secondHit, firstHit);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("White Canopy does not add Icy Wind if weather is no longer Snow/Hail")
{
    GIVEN {
        PLAYER(SPECIES_ABOMASNOW) { Speed(1); Ability(ABILITY_SOUNDPROOF); UniqueAbility(ABILITY_WHITE_CANOPY); Moves(MOVE_MAGICAL_LEAF); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_SUNNY_DAY); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUNNY_DAY); MOVE(player, MOVE_MAGICAL_LEAF); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUNNY_DAY, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MAGICAL_LEAF, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_WHITE_CANOPY);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_ICY_WIND, player);
        }
    }
}
