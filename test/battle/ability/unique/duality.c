#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_PHYSICAL(MOVE_TACKLE));
    ASSUME(IS_MOVE_SPECIAL(MOVE_WATER_GUN));
}

SINGLE_BATTLE_TEST("Duality announces its current offensive focus on entry and after every turn")
{
    GIVEN {
        PLAYER(SPECIES_ESPATHRA) { Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DUALITY);
        MESSAGE("Espathra's mind\ntook the lead!");
        ABILITY_POPUP(player, ABILITY_DUALITY);
        MESSAGE("Espathra's body\ntook the lead!");
        ABILITY_POPUP(player, ABILITY_DUALITY);
        MESSAGE("Espathra's mind\ntook the lead!");
    }
}

SINGLE_BATTLE_TEST("Duality announces when Attack and Sp. Atk are balanced")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(100); SpAttack(100); UniqueAbility(ABILITY_DUALITY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DUALITY);
        MESSAGE("Wobbuffet found balance\nbetween body and mind!");
        ABILITY_POPUP(player, ABILITY_DUALITY);
        MESSAGE("Wobbuffet found balance\nbetween body and mind!");
    }
}

SINGLE_BATTLE_TEST("Duality accounts for stat stages when announcing its offensive focus")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(100); SpAttack(100); UniqueAbility(ABILITY_DUALITY); Moves(MOVE_SWORDS_DANCE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DUALITY);
        MESSAGE("Wobbuffet found balance\nbetween body and mind!");
        ABILITY_POPUP(player, ABILITY_DUALITY);
        MESSAGE("Wobbuffet's mind\ntook the lead!");
    }
}

SINGLE_BATTLE_TEST("Duality announces its initial focus after Download")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) {
            Ability(ABILITY_DOWNLOAD);
            UniqueAbility(ABILITY_DUALITY);
            Attack(100);
            SpAttack(120);
            Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_WOBBUFFET) {
            Defense(50);
            SpDefense(100);
            Moves(MOVE_CELEBRATE);
        }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DOWNLOAD);
        ABILITY_POPUP(player, ABILITY_DUALITY);
        MESSAGE("Wobbuffet's body\ntook the lead!");
    }
}

SINGLE_BATTLE_TEST("Duality swaps Attack and Sp. Atk every other turn for physical moves")
{
    s16 firstHit;
    s16 secondHit;
    s16 thirdHit;

    GIVEN {
        PLAYER(SPECIES_ESPATHRA) { Level(100); Attack(200); SpAttack(50); Speed(100); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(100); HP(1000); MaxHP(1000); Defense(100); SpDefense(100); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &firstHit);
        HP_BAR(opponent, captureDamage: &secondHit);
        HP_BAR(opponent, captureDamage: &thirdHit);
    } THEN {
        EXPECT_GT(firstHit, secondHit);
        EXPECT_EQ(firstHit, thirdHit);
    }
}

SINGLE_BATTLE_TEST("Duality swaps Attack and Sp. Atk every other turn for special moves")
{
    s16 firstHit;
    s16 secondHit;

    GIVEN {
        PLAYER(SPECIES_ESPATHRA) { Level(100); Attack(200); SpAttack(50); Speed(100); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(100); HP(1000); MaxHP(1000); Defense(100); SpDefense(100); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_WATER_GUN, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &firstHit);
        HP_BAR(opponent, captureDamage: &secondHit);
    } THEN {
        EXPECT_LT(firstHit, secondHit);
    }
}
