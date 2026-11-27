#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Lone Fang grants STAB only to a sole attack without stacking natural STAB", s16 damage)
{
    u16 move, ability, other;
    PARAMETRIZE { move = MOVE_EARTHQUAKE; ability = ABILITY_BREAKDANCE; other = MOVE_GLARE; }
    PARAMETRIZE { move = MOVE_EARTHQUAKE; ability = ABILITY_LONE_FANG; other = MOVE_GLARE; }
    PARAMETRIZE { move = MOVE_EARTHQUAKE; ability = ABILITY_LONE_FANG; other = MOVE_DRAGON_CLAW; }
    PARAMETRIZE { move = MOVE_DRAGON_CLAW; ability = ABILITY_BREAKDANCE; other = MOVE_GLARE; }
    PARAMETRIZE { move = MOVE_DRAGON_CLAW; ability = ABILITY_LONE_FANG; other = MOVE_GLARE; }
    GIVEN {
        PLAYER(SPECIES_DRUDDIGON) { UniqueAbility(ability); Moves(move, other); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
        EXPECT_EQ(results[3].damage, results[4].damage);
    }
}

SINGLE_BATTLE_TEST("Lone Fang pivots after Outrage or after Dragon Tail forces the target out")
{
    u16 move;
    PARAMETRIZE { move = MOVE_OUTRAGE; }
    PARAMETRIZE { move = MOVE_DRAGON_TAIL; }
    GIVEN {
        PLAYER(SPECIES_DRUDDIGON) { Ability(ABILITY_NO_GUARD); UniqueAbility(ABILITY_LONE_FANG); Moves(move, MOVE_GLARE); }
        PLAYER(SPECIES_EEVEE);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_BULBASAUR);
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_CELEBRATE); SEND_OUT(player, 1); }
    } SCENE {
        HP_BAR(opponent);
        if (move == MOVE_DRAGON_TAIL)
            MESSAGE("Foe Bulbasaur was dragged out!");
        ABILITY_POPUP(player, ABILITY_LONE_FANG);
    } THEN {
        EXPECT_EQ(player->species, SPECIES_EEVEE);
    }
}

SINGLE_BATTLE_TEST("Lone Fang stays in when protected against, using status, or carrying coverage")
{
    u16 move, other, foeMove;
    PARAMETRIZE { move = MOVE_EARTHQUAKE; other = MOVE_GLARE; foeMove = MOVE_PROTECT; }
    PARAMETRIZE { move = MOVE_GLARE; other = MOVE_EARTHQUAKE; foeMove = MOVE_CELEBRATE; }
    PARAMETRIZE { move = MOVE_EARTHQUAKE; other = MOVE_OUTRAGE; foeMove = MOVE_CELEBRATE; }
    GIVEN {
        PLAYER(SPECIES_DRUDDIGON) { UniqueAbility(ABILITY_LONE_FANG); Moves(move, other); }
        PLAYER(SPECIES_EEVEE);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, foeMove); }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_DRUDDIGON);
    }
}

DOUBLE_BATTLE_TEST("Lone Fang finishes Earthquake before switching out")
{
    GIVEN {
        PLAYER(SPECIES_DRUDDIGON) { UniqueAbility(ABILITY_LONE_FANG); Moves(MOVE_EARTHQUAKE); }
        PLAYER(SPECIES_PIDGEY);
        PLAYER(SPECIES_EEVEE);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_EARTHQUAKE); SEND_OUT(playerLeft, 2); }
    } SCENE {
        HP_BAR(opponentLeft);
        HP_BAR(opponentRight);
        ABILITY_POPUP(playerLeft, ABILITY_LONE_FANG);
    } THEN {
        EXPECT_EQ(playerLeft->species, SPECIES_EEVEE);
    }
}
