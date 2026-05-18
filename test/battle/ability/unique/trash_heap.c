#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
    ASSUME(!gBattleMoves[MOVE_WATER_GUN].makesContact);
    ASSUME(gBattleMoves[MOVE_POISON_GAS].effect == EFFECT_POISON);
    ASSUME(gBattleMoves[MOVE_TOXIC_SPIKES].effect == EFFECT_TOXIC_SPIKES);
    ASSUME(gBattleMoves[MOVE_ACID_SPRAY].effect == EFFECT_SPECIAL_DEFENSE_DOWN_HIT_2);
    ASSUME(gBattleMoves[MOVE_SLUDGE].effect == EFFECT_POISON_HIT);
    ASSUME(gBattleMoves[MOVE_MUD_SLAP].effect == EFFECT_ACCURACY_DOWN_HIT);
    ASSUME(gBattleMoves[MOVE_SMOKESCREEN].effect == EFFECT_ACCURACY_DOWN);
    ASSUME(gBattleMoves[MOVE_SPIKES].effect == EFFECT_SPIKES);
    ASSUME(gBattleMoves[MOVE_CLEAR_SMOG].effect == EFFECT_CLEAR_SMOG);
    ASSUME(gBattleMoves[MOVE_BELCH].effect == EFFECT_BELCH);
    ASSUME(gBattleMoves[MOVE_STOCKPILE].effect == EFFECT_STOCKPILE);
}

SINGLE_BATTLE_TEST("Trash Heap uses a random trash move after being hit by a contact move")
{
    GIVEN {
        PLAYER(SPECIES_GARBODOR) { Ability(ABILITY_STENCH); UniqueAbility(ABILITY_TRASH_HEAP); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE, WITH_RNG(RNG_ROGUE_TRASH_HEAP, MOVE_SLUDGE)); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TRASH_HEAP);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SLUDGE, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Trash Heap can choose a self-targeting trash move after contact")
{
    GIVEN {
        PLAYER(SPECIES_GARBODOR) { Ability(ABILITY_STENCH); UniqueAbility(ABILITY_TRASH_HEAP); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE, WITH_RNG(RNG_ROGUE_TRASH_HEAP, MOVE_STOCKPILE)); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TRASH_HEAP);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STOCKPILE, player);
        MESSAGE("Garbodor stockpiled 1!");
    }
}

SINGLE_BATTLE_TEST("Trash Heap does not trigger after the user faints to a contact move")
{
    GIVEN {
        PLAYER(SPECIES_TRUBBISH) { HP(1); MaxHP(100); Defense(1); Ability(ABILITY_STENCH); UniqueAbility(ABILITY_TRASH_HEAP); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_AZUMARILL) { Attack(200); Speed(100); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE, criticalHit: TRUE, WITH_RNG(RNG_ROGUE_TRASH_HEAP, MOVE_SLUDGE)); }
    } SCENE {
        MESSAGE("A critical hit!");
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_TRASH_HEAP);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SLUDGE, player);
        }
        MESSAGE("Trubbish fainted!");
    } THEN {
        EXPECT_EQ(player->hp, 0);
    }
}

SINGLE_BATTLE_TEST("Trash Heap can choose every trash move")
{
    static const u16 expectedMoves[] =
    {
        MOVE_POISON_GAS,
        MOVE_TOXIC_SPIKES,
        MOVE_ACID_SPRAY,
        MOVE_SLUDGE,
        MOVE_MUD_SLAP,
        MOVE_SMOKESCREEN,
        MOVE_SPIKES,
        MOVE_CLEAR_SMOG,
        MOVE_BELCH,
        MOVE_STOCKPILE,
    };

    PASSES_RANDOMLY(ARRAY_COUNT(expectedMoves), ARRAY_COUNT(expectedMoves), RNG_ROGUE_TRASH_HEAP);

    GIVEN {
        PLAYER(SPECIES_GARBODOR) { Ability(ABILITY_STENCH); UniqueAbility(ABILITY_TRASH_HEAP); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TRASH_HEAP);
    } THEN {
        EXPECT_EQ(gCalledMove, expectedMoves[gBattleTestRunnerState->runTrial]);
    }
}
