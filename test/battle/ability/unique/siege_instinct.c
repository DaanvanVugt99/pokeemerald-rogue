#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
    ASSUME(!gBattleMoves[MOVE_EARTHQUAKE].makesContact);
    ASSUME(gBattleMoves[MOVE_PROTECT].effect == EFFECT_PROTECT);
}

SINGLE_BATTLE_TEST("Siege Instinct contact moves hit through Protect for quarter damage", s16 damage)
{
    bool32 protected;

    PARAMETRIZE { protected = TRUE; }
    PARAMETRIZE { protected = FALSE; }

    GIVEN {
        PLAYER(SPECIES_GREAT_TUSK) { Attack(100); Speed(50); Ability(ABILITY_PROTOSYNTHESIS); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Defense(100); Speed(100); Moves(MOVE_PROTECT, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, protected ? MOVE_PROTECT : MOVE_CELEBRATE); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(4.0), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Siege Instinct remains inactive with another Paradox ally")
{
    GIVEN {
        PLAYER(SPECIES_GREAT_TUSK) { Speed(50); Ability(ABILITY_PROTOSYNTHESIS); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_IRON_BUNDLE) { Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(100); Moves(MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Siege Instinct does not pierce protection with non-contact moves")
{
    GIVEN {
        PLAYER(SPECIES_GREAT_TUSK) { Speed(50); Ability(ABILITY_PROTOSYNTHESIS); Moves(MOVE_EARTHQUAKE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(100); Moves(MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_EARTHQUAKE); }
    } THEN {
        EXPECT_EQ(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Siege Instinct retains protective contact effects")
{
    GIVEN {
        PLAYER(SPECIES_GREAT_TUSK) { HP(80); MaxHP(80); Speed(50); Ability(ABILITY_PROTOSYNTHESIS); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(100); Moves(MOVE_SPIKY_SHIELD); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SPIKY_SHIELD); MOVE(player, MOVE_TACKLE); }
    } THEN {
        EXPECT_LT(player->hp, player->maxHP);
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}
