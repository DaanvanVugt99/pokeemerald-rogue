#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_RECOVER].effect == EFFECT_RESTORE_HP);
    ASSUME(gBattleMoves[MOVE_RECOVER].healBlockBanned);
    ASSUME(gBattleMoves[MOVE_GASTRO_ACID].effect == EFFECT_GASTRO_ACID);
}

SINGLE_BATTLE_TEST("Bog Body prevents opposing Pokemon from using healing moves")
{
    u16 species;

    PARAMETRIZE { species = SPECIES_WOOPER_PALDEAN; }
    PARAMETRIZE { species = SPECIES_CLODSIRE; }

    GIVEN {
        PLAYER(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
        PLAYER(species) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(50); MaxHP(100); Moves(MOVE_RECOVER); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_RECOVER); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BOG_BODY);
        MESSAGE("Foe Wobbuffet was\nprevented from healing!");
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, opponent);
    } THEN {
        EXPECT_EQ(opponent->hp, 50);
    }
}

SINGLE_BATTLE_TEST("Bog Body prevents opposing passive item recovery")
{
    GIVEN {
        PLAYER(SPECIES_CLODSIRE) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(50); MaxHP(100); Item(ITEM_LEFTOVERS); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BOG_BODY);
        NONE_OF {
            HP_BAR(opponent);
            MESSAGE("Foe Wobbuffet's Leftovers restored its HP a little!");
        }
    } THEN {
        EXPECT_EQ(opponent->hp, 50);
    }
}

SINGLE_BATTLE_TEST("Bog Body prevents opposing Shell Bell recovery")
{
    GIVEN {
        PLAYER(SPECIES_CLODSIRE) { HP(1000); MaxHP(1000); Ability(ABILITY_WATER_ABSORB); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(50); MaxHP(100); Attack(300); Item(ITEM_SHELL_BELL); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BOG_BODY);
        NONE_OF {
            HP_BAR(opponent);
            MESSAGE("Foe Wobbuffet's Shell Bell restored its HP a little!");
        }
    } THEN {
        EXPECT_EQ(opponent->hp, 50);
    }
}

SINGLE_BATTLE_TEST("Bog Body stops blocking healing when its holder switches out")
{
    GIVEN {
        PLAYER(SPECIES_CLODSIRE) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(50); MaxHP(100); Item(ITEM_LEFTOVERS); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BOG_BODY);
        MESSAGE("Foe Wobbuffet's Leftovers restored its HP a little!");
        HP_BAR(opponent, damage: -6);
    } THEN {
        EXPECT_EQ(opponent->hp, 56);
    }
}

SINGLE_BATTLE_TEST("Gastro Acid suppresses Bog Body's healing lock")
{
    GIVEN {
        PLAYER(SPECIES_CLODSIRE) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(50); MaxHP(100); Moves(MOVE_GASTRO_ACID, MOVE_RECOVER); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_GASTRO_ACID); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_RECOVER); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BOG_BODY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GASTRO_ACID, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, opponent);
        HP_BAR(opponent, damage: -50);
    } THEN {
        EXPECT_EQ(opponent->hp, 100);
    }
}

DOUBLE_BATTLE_TEST("Bog Body does not prevent its allies from healing")
{
    GIVEN {
        PLAYER(SPECIES_CLODSIRE) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(50); MaxHP(100); Moves(MOVE_RECOVER); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_RECOVER);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_BOG_BODY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, playerRight);
        HP_BAR(playerRight, damage: -50);
    } THEN {
        EXPECT_EQ(playerRight->hp, 100);
    }
}
