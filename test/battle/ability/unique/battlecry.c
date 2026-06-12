#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_GROWL].effect == EFFECT_ATTACK_DOWN);
}

SINGLE_BATTLE_TEST("Battlecry does not use Growl on switch-in unless an ally fainted last turn")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PYROAR) { Ability(ABILITY_RIVALRY); UniqueAbility(ABILITY_BATTLECRY); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(opponent, ABILITY_BATTLECRY);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Battlecry uses Growl on switch-in after an ally fainted last turn")
{
    GIVEN {
        PLAYER(SPECIES_WYNAUT) { HP(1); MaxHP(100); Speed(1); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_PYROAR) { Speed(100); Ability(ABILITY_RIVALRY); UniqueAbility(ABILITY_BATTLECRY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); SEND_OUT(player, 1); }
    } SCENE {
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_BATTLECRY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Battlecry makes Growl lower Special Attack too")
{
    GIVEN {
        PLAYER(SPECIES_PYROAR) { Ability(ABILITY_RIVALRY); UniqueAbility(ABILITY_BATTLECRY); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 1);
    }
}
