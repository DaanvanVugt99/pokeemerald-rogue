#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_AQUA_JET].effect == EFFECT_HIT);
    ASSUME(gBattleMoves[MOVE_AQUA_JET].power != 0);
    ASSUME(gBattleMoves[MOVE_AQUA_JET].type == TYPE_WATER);
}

SINGLE_BATTLE_TEST("Hero Landing uses Aqua Jet on switch-in after an allied faint")
{
    GIVEN {
        PLAYER(SPECIES_WYNAUT) { HP(1); MaxHP(100); Speed(1); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_FINIZEN) { Level(50); Attack(100); Speed(100); Ability(ABILITY_WATER_VEIL); UniqueAbility(ABILITY_HERO_LANDING); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CHARMANDER) { HP(1000); MaxHP(1000); Defense(100); Speed(1); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); SEND_OUT(player, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_HERO_LANDING);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AQUA_JET, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Hero Landing does not use Aqua Jet without an allied faint")
{
    GIVEN {
        PLAYER(SPECIES_WYNAUT) { Speed(1); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_FINIZEN) { Level(50); Attack(100); Speed(100); Ability(ABILITY_WATER_VEIL); UniqueAbility(ABILITY_HERO_LANDING); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CHARMANDER) { HP(1000); MaxHP(1000); Defense(100); Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_HERO_LANDING);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_AQUA_JET, player);
            HP_BAR(opponent);
        }
    } THEN {
        EXPECT_EQ(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Hero Landing's Aqua Jet respects Psychic Terrain", s16 damage)
{
    u16 targetSpecies;
    u16 targetAbility;

    PARAMETRIZE { targetSpecies = SPECIES_WOBBUFFET; targetAbility = ABILITY_SHADOW_TAG; }
    PARAMETRIZE { targetSpecies = SPECIES_CLAYDOL; targetAbility = ABILITY_LEVITATE; }

    GIVEN {
        PLAYER(SPECIES_WYNAUT) { HP(1); MaxHP(100); Speed(1); Moves(MOVE_PSYCHIC_TERRAIN); }
        PLAYER(SPECIES_FINIZEN) { Level(50); Attack(100); Speed(100); Ability(ABILITY_WATER_VEIL); UniqueAbility(ABILITY_HERO_LANDING); Moves(MOVE_CELEBRATE); }
        OPPONENT(targetSpecies) { HP(1000); MaxHP(1000); Attack(100); Defense(100); Speed(100); Ability(targetAbility); Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_PSYCHIC_TERRAIN); }
        TURN { MOVE(player, MOVE_PSYCHIC_TERRAIN); MOVE(opponent, MOVE_TACKLE); SEND_OUT(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHIC_TERRAIN, player);
        ABILITY_POPUP(player, ABILITY_HERO_LANDING);
        if (targetAbility == ABILITY_SHADOW_TAG)
        {
            MESSAGE("Finizen cannot use Aqua Jet!");
            NOT { ANIMATION(ANIM_TYPE_MOVE, MOVE_AQUA_JET, player); }
        }
        else
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_AQUA_JET, player);
            HP_BAR(opponent, captureDamage: &results[i].damage);
        }
    } THEN {
        if (targetAbility == ABILITY_SHADOW_TAG)
            EXPECT_EQ(opponent->hp, opponent->maxHP);
        else
            EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}
