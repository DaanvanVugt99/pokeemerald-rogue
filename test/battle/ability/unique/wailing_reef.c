#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BITE].power > 0);
    ASSUME(IS_MOVE_PHYSICAL(MOVE_BITE));
    ASSUME(gBattleMoves[MOVE_SCRATCH].power > 0);
    ASSUME(IS_MOVE_PHYSICAL(MOVE_SCRATCH));
    ASSUME(gBattleMoves[MOVE_GROWL].power == 0);
}

SINGLE_BATTLE_TEST("Wailing Reef does not overwrite Weak Armor")
{
    GIVEN {
        PLAYER(SPECIES_CORSOLA_GALARIAN) { MaxHP(500); HP(500); Ability(ABILITY_WEAK_ARMOR); UniqueAbility(ABILITY_WAILING_REEF); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_BITE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_BITE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(opponent->pp[0], gBattleMoves[MOVE_BITE].pp - 1);
    }
}

SINGLE_BATTLE_TEST("Wailing Reef uses Spite after being hit by a damaging move")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); MaxHP(500); HP(500); UniqueAbility(ABILITY_WAILING_REEF); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_SCRATCH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_WAILING_REEF);
    } THEN {
        EXPECT_EQ(opponent->pp[0], gBattleMoves[MOVE_SCRATCH].pp - 5);
    }
}

SINGLE_BATTLE_TEST("Wailing Reef does not trigger when hit by a non-damaging move")
{
    GIVEN {
        PLAYER(SPECIES_CURSOLA) { Ability(ABILITY_WEAK_ARMOR); UniqueAbility(ABILITY_WAILING_REEF); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_GROWL); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_GROWL); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_SPITE, player);
    }
}
