#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Tripwire remains armed while Disguise absorbs a hit")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_SPIDOPS) { Ability(ABILITY_DISGUISE); UniqueAbility(ABILITY_TRIPWIRE); HP(400); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_TACKLE); }
        TURN { SWITCH(opponent, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DISGUISE);
        HP_BAR(player, damage: 50);
        ABILITY_POPUP(player, ABILITY_TRIPWIRE);
        HP_BAR(opponent, damage: 50);
    } THEN {
        EXPECT_EQ(player->species, SPECIES_SPIDOPS);
        EXPECT_EQ(player->hp, 350);
        EXPECT_EQ(opponent->hp, 350);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Tripwire damages and lowers Speed when the first opposing Pokemon switches in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_SPIDOPS) { Ability(ABILITY_INSOMNIA); UniqueAbility(ABILITY_TRIPWIRE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CHARIZARD) { HP(80); MaxHP(80); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); SWITCH(opponent, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TRIPWIRE);
        MESSAGE("Foe Charizard was caught\nin Spidops's tripwire!");
        HP_BAR(opponent, damage: 10);
    } THEN {
        EXPECT_EQ(opponent->hp, 70);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Tripwire only triggers once each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_SPIDOPS) { Ability(ABILITY_INSOMNIA); UniqueAbility(ABILITY_TRIPWIRE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CHARIZARD) { HP(80); MaxHP(80); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_BLASTOISE) { HP(80); MaxHP(80); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); SWITCH(opponent, 1); }
        TURN { MOVE(player, MOVE_CELEBRATE); SWITCH(opponent, 2); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TRIPWIRE);
        MESSAGE("Foe Charizard was caught\nin Spidops's tripwire!");
        HP_BAR(opponent, damage: 10);
        NOT ABILITY_POPUP(player, ABILITY_TRIPWIRE);
    } THEN {
        EXPECT_EQ(opponent->hp, 80);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Tripwire refreshes after the user switches out and back in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_SPIDOPS) { Ability(ABILITY_INSOMNIA); UniqueAbility(ABILITY_TRIPWIRE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CHARIZARD) { HP(80); MaxHP(80); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_BLASTOISE) { HP(80); MaxHP(80); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); SWITCH(opponent, 1); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); SWITCH(opponent, 2); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TRIPWIRE);
        MESSAGE("Foe Charizard was caught\nin Spidops's tripwire!");
        ABILITY_POPUP(player, ABILITY_TRIPWIRE);
        MESSAGE("Foe Blastoise was caught\nin Spidops's tripwire!");
    } THEN {
        EXPECT_EQ(opponent->species, SPECIES_BLASTOISE);
        EXPECT_EQ(opponent->hp, 70);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Tripwire handles a foe fainting on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_SPIDOPS) { Ability(ABILITY_INSOMNIA); UniqueAbility(ABILITY_TRIPWIRE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CHARIZARD) { HP(10); MaxHP(80); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_BLASTOISE) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); SWITCH(opponent, 1); SEND_OUT(opponent, 2); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TRIPWIRE);
        MESSAGE("Foe Charizard was caught\nin Spidops's tripwire!");
        HP_BAR(opponent, damage: 10);
        MESSAGE("Foe Charizard fainted!");
        MESSAGE("2 sent out Blastoise!");
    } THEN {
        EXPECT_EQ(opponent->species, SPECIES_BLASTOISE);
        EXPECT_EQ(opponent->hp, opponent->maxHP);
    }
}
