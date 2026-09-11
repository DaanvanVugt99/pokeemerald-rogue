#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_FLAME_RELAY].effect == EFFECT_HIT_ESCAPE);
    ASSUME(gBattleMoves[MOVE_FLAME_RELAY].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_FLAME_RELAY].power == 70);
    ASSUME(gBattleMoves[MOVE_FLAME_RELAY].accuracy == 100);
    ASSUME(gBattleMoves[MOVE_FLAME_RELAY].pp == 20);
    ASSUME(gBattleMoves[MOVE_FLAME_RELAY].priority == 0);
    ASSUME(gBattleMoves[MOVE_FLAME_RELAY].split == SPLIT_PHYSICAL);
}

SINGLE_BATTLE_TEST("Flame Relay damages the target before switching to an ally")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        PLAYER(SPECIES_WYNAUT) { Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FLAME_RELAY); SEND_OUT(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLAME_RELAY, player);
        HP_BAR(opponent);
        MESSAGE("Go! Wynaut!");
    }
}

SINGLE_BATTLE_TEST("Flame Relay cannot pivot through Protect")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_FLAME_RELAY); MOVE(opponent, MOVE_PROTECT); }
    } SCENE {
        NONE_OF {
            HP_BAR(opponent);
            MESSAGE("Go! Wynaut!");
        }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WOBBUFFET);
    }
}

SINGLE_BATTLE_TEST("Flame Relay cannot pivot through Flash Fire")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_FLASH_FIRE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FLAME_RELAY); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_FLASH_FIRE);
        NONE_OF {
            HP_BAR(opponent);
            MESSAGE("Go! Wynaut!");
        }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WOBBUFFET);
    }
}

SINGLE_BATTLE_TEST("Flame Relay still damages the target without a replacement")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FLAME_RELAY); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLAME_RELAY, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WOBBUFFET);
    }
}

SINGLE_BATTLE_TEST("Flame Relay receives Rough Skin contact damage before switching")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        PLAYER(SPECIES_WYNAUT) { Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_ROUGH_SKIN); Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FLAME_RELAY); SEND_OUT(player, 1); }
    } SCENE {
        HP_BAR(opponent);
        ABILITY_POPUP(opponent, ABILITY_ROUGH_SKIN);
        HP_BAR(player);
        MESSAGE("Go! Wynaut!");
    }
}

SINGLE_BATTLE_TEST("Flame Relay does not request a replacement when the battle ends")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        PLAYER(SPECIES_WYNAUT) { Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FLAME_RELAY); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLAME_RELAY, player);
        HP_BAR(opponent);
        NOT MESSAGE("Go! Wynaut!");
    }
}
