#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_DRAGON_RAGE].effect == EFFECT_DRAGON_RAGE);
    ASSUME(gBattleMoves[MOVE_TAIL_WHIP].effect == EFFECT_DEFENSE_DOWN);
    ASSUME(gBattleMoves[MOVE_SWORDS_DANCE].effect == EFFECT_ATTACK_UP_2);
}

SINGLE_BATTLE_TEST("Second Wind cures status when dropping below half HP")
{
    GIVEN {
        PLAYER(SPECIES_PANGORO) { Speed(1); Ability(ABILITY_IRON_FIST); UniqueAbility(ABILITY_SECOND_WIND); HP(100); MaxHP(160); Status1(STATUS1_PARALYSIS); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SECOND_WIND);
        MESSAGE("Pangoro's status returned to normal!");
    } THEN {
        EXPECT_EQ(player->hp, 60);
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Second Wind removes negative stat changes without removing positive stat changes")
{
    GIVEN {
        PLAYER(SPECIES_PANGORO) { Speed(1); Ability(ABILITY_IRON_FIST); UniqueAbility(ABILITY_SECOND_WIND); HP(100); MaxHP(160); Moves(MOVE_SWORDS_DANCE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TAIL_WHIP, MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); MOVE(opponent, MOVE_TAIL_WHIP); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SECOND_WIND);
        MESSAGE("Pangoro's stat changes were reset!");
    } THEN {
        EXPECT_EQ(player->hp, 60);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Second Wind cures status and removes negative stat changes together")
{
    GIVEN {
        PLAYER(SPECIES_PANGORO) { Speed(1); Ability(ABILITY_IRON_FIST); UniqueAbility(ABILITY_SECOND_WIND); HP(100); MaxHP(160); Status1(STATUS1_PARALYSIS); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TAIL_WHIP, MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TAIL_WHIP); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SECOND_WIND);
        MESSAGE("Pangoro's status returned to normal!");
        MESSAGE("Pangoro's stat changes were reset!");
    } THEN {
        EXPECT_EQ(player->hp, 60);
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Second Wind does not trigger at exactly half HP")
{
    GIVEN {
        PLAYER(SPECIES_PANGORO) { Speed(1); Ability(ABILITY_IRON_FIST); UniqueAbility(ABILITY_SECOND_WIND); HP(120); MaxHP(160); Status1(STATUS1_PARALYSIS); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SECOND_WIND);
            MESSAGE("Pangoro's status returned to normal!");
        }
    } THEN {
        EXPECT_EQ(player->hp, 80);
        EXPECT_NE(player->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Second Wind does not trigger if the user faints")
{
    GIVEN {
        PLAYER(SPECIES_PANGORO) { Speed(1); Ability(ABILITY_IRON_FIST); UniqueAbility(ABILITY_SECOND_WIND); HP(40); MaxHP(160); Status1(STATUS1_PARALYSIS); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SECOND_WIND);
            MESSAGE("Pangoro's status returned to normal!");
        }
    } THEN {
        EXPECT_EQ(player->hp, 0);
    }
}
