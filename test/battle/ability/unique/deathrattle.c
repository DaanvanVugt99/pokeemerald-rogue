#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SHADOW_BALL].power != 0);
    ASSUME(gBattleMoves[MOVE_SHADOW_BALL].type == TYPE_GHOST);
    ASSUME(gBattleMoves[MOVE_DRAGON_RAGE].effect == EFFECT_DRAGON_RAGE);
}

SINGLE_BATTLE_TEST("Deathrattle primes after an allied faint and heals from the first damaging move")
{
    GIVEN {
        PLAYER(SPECIES_WYNAUT) { HP(1); MaxHP(100); Speed(1); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_GREAVARD) { Level(50); HP(50); MaxHP(100); Speed(100); SpAttack(100); Ability(ABILITY_PICKUP); UniqueAbility(ABILITY_DEATHRATTLE); Moves(MOVE_SHADOW_BALL); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(1); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); SEND_OUT(player, 1); }
        TURN { MOVE(player, MOVE_SHADOW_BALL); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SHADOW_BALL); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_GT(player->hp, 50);
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniquePersistentStateActive);
    }
}

SINGLE_BATTLE_TEST("Deathrattle does not heal without an allied faint")
{
    GIVEN {
        PLAYER(SPECIES_WYNAUT) { Speed(1); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_GREAVARD) { Level(50); HP(50); MaxHP(100); Speed(100); SpAttack(100); Ability(ABILITY_PICKUP); UniqueAbility(ABILITY_DEATHRATTLE); Moves(MOVE_SHADOW_BALL); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SHADOW_BALL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_DEATHRATTLE);
            HP_BAR(player);
        }
    } THEN {
        EXPECT_EQ(player->hp, 50);
    }
}

SINGLE_BATTLE_TEST("Deathrattle curses the attacker before this Pokemon faints")
{
    GIVEN {
        PLAYER(SPECIES_HOUNDSTONE) { HP(40); MaxHP(100); Speed(1); Ability(ABILITY_SAND_RUSH); UniqueAbility(ABILITY_DEATHRATTLE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(100); Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_DRAGON_RAGE); SEND_OUT(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_RAGE, opponent);
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_DEATHRATTLE);
    } THEN {
        EXPECT(opponent->status2 & STATUS2_CURSED);
        EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_HP), 0);
    }
}

SINGLE_BATTLE_TEST("Deathrattle does not curse after the user's final Pokemon faints")
{
    GIVEN {
        PLAYER(SPECIES_HOUNDSTONE) { HP(40); MaxHP(100); Speed(1); Ability(ABILITY_SAND_RUSH); UniqueAbility(ABILITY_DEATHRATTLE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(100); Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_RAGE, opponent);
        HP_BAR(player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_DEATHRATTLE);
        }
    } THEN {
        EXPECT(!(opponent->status2 & STATUS2_CURSED));
        EXPECT_EQ(player->hp, 0);
    }
}
