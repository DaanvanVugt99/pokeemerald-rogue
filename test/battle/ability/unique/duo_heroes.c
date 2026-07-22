#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].power > 0);
    ASSUME(gBattleMoves[MOVE_CELEBRATE].power == 0);
}
SINGLE_BATTLE_TEST("Royal Advance waits for an attack and transfers only once per battle")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { UniqueAbility(ABILITY_ROYAL_ADVANCE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WYNAUT) { Speed(50); Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ABILITY_POPUP(player, ABILITY_ROYAL_ADVANCE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
    }
}

SINGLE_BATTLE_TEST("Royal Advance transfers after fainting")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1); MaxHP(1); UniqueAbility(ABILITY_ROYAL_ADVANCE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WYNAUT) { HP(300); MaxHP(300); Speed(50); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); SEND_OUT(player, 1); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ABILITY_POPUP(player, ABILITY_ROYAL_ADVANCE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

SINGLE_BATTLE_TEST("Royal Guard halves only the first hit after switching out", s16 firstDamage, s16 secondDamage)
{
    bool32 hasRoyalGuard;

    PARAMETRIZE { hasRoyalGuard = FALSE; }
    PARAMETRIZE { hasRoyalGuard = TRUE; }

    GIVEN {
        if (hasRoyalGuard)
            PLAYER(SPECIES_WOBBUFFET) { UniqueAbility(ABILITY_ROYAL_GUARD); Moves(MOVE_CELEBRATE); }
        else
            PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WYNAUT) { HP(500); MaxHP(500); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(100); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
        TURN { MOVE(opponent, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        if (hasRoyalGuard)
            ABILITY_POPUP(player, ABILITY_ROYAL_GUARD);
        HP_BAR(player, captureDamage: &results[i].firstDamage);
        HP_BAR(player, captureDamage: &results[i].secondDamage);
    } FINALLY {
        EXPECT_EQ(results[0].firstDamage, results[0].secondDamage);
        EXPECT_MUL_EQ(results[0].firstDamage, UQ_4_12(0.5), results[1].firstDamage);
        EXPECT_MUL_EQ(results[1].firstDamage, UQ_4_12(2.0), results[1].secondDamage);
    }
}

SINGLE_BATTLE_TEST("Royal Advance keeps switch and opposing gimmick uses separate")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { UniqueAbility(ABILITY_ROYAL_ADVANCE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WYNAUT) { HP(500); MaxHP(500); Speed(50); UniqueAbility(ABILITY_ROYAL_ADVANCE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_GARDEVOIR) { HP(500); MaxHP(500); Attack(10); Defense(100); Item(ITEM_GARDEVOIRITE); Speed(200); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_TACKLE, megaEvolve: TRUE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_MEGA_EVOLUTION, opponent);
        ABILITY_POPUP(player, ABILITY_ROYAL_ADVANCE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
    }
}

SINGLE_BATTLE_TEST("Royal Guard responds to an opposing gimmick", s16 firstDamage, s16 secondDamage)
{
    bool32 hasRoyalGuard;

    PARAMETRIZE { hasRoyalGuard = FALSE; }
    PARAMETRIZE { hasRoyalGuard = TRUE; }

    GIVEN {
        if (hasRoyalGuard)
            PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); UniqueAbility(ABILITY_ROYAL_GUARD); Moves(MOVE_CELEBRATE); }
        else
            PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(100); TeraType(TYPE_PSYCHIC); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE, tera: TRUE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_TERA_ACTIVATE, opponent);
        if (hasRoyalGuard)
            ABILITY_POPUP(player, ABILITY_ROYAL_GUARD);
        HP_BAR(player, captureDamage: &results[i].firstDamage);
        HP_BAR(player, captureDamage: &results[i].secondDamage);
    } FINALLY {
        EXPECT_EQ(results[0].firstDamage, results[0].secondDamage);
        EXPECT_MUL_EQ(results[0].firstDamage, UQ_4_12(0.5), results[1].firstDamage);
        EXPECT_MUL_EQ(results[1].firstDamage, UQ_4_12(2.0), results[1].secondDamage);
    }
}
