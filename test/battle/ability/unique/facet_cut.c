#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_METAL_CLAW].type == TYPE_STEEL);
    ASSUME(gBattleMoves[MOVE_METAL_CLAW].split == SPLIT_PHYSICAL);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].split == SPLIT_SPECIAL);
    ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_REFLECT].effect == EFFECT_REFLECT);
    ASSUME(gBattleMoves[MOVE_LIGHT_SCREEN].effect == EFFECT_LIGHT_SCREEN);
    ASSUME(gSpeciesInfo[SPECIES_DIANCIE].types[0] == TYPE_ROCK || gSpeciesInfo[SPECIES_DIANCIE].types[1] == TYPE_ROCK);
    ASSUME(gSpeciesInfo[SPECIES_DIANCIE].types[0] == TYPE_FAIRY || gSpeciesInfo[SPECIES_DIANCIE].types[1] == TYPE_FAIRY);
}

SINGLE_BATTLE_TEST("Facet Cut uses Reflect after the first super effective physical hit")
{
    GIVEN {
        PLAYER(SPECIES_DIANCIE) { HP(400); MaxHP(400); Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_FACET_CUT); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(100); Moves(MOVE_METAL_CLAW); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_METAL_CLAW); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FACET_CUT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_REFLECT, player);
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_REFLECT);
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_LIGHTSCREEN));
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Facet Cut uses Light Screen after the first super effective special hit")
{
    GIVEN {
        PLAYER(SPECIES_DIANCIE) { HP(400); MaxHP(400); Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_FACET_CUT); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { SpAttack(100); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_WATER_GUN); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FACET_CUT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LIGHT_SCREEN, player);
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_REFLECT));
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_LIGHTSCREEN);
    }
}

SINGLE_BATTLE_TEST("Facet Cut only triggers once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_DIANCIE) { HP(400); MaxHP(400); Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_FACET_CUT); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(100); SpAttack(100); Moves(MOVE_METAL_CLAW, MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_METAL_CLAW); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_WATER_GUN); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FACET_CUT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_REFLECT, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_FACET_CUT);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_LIGHT_SCREEN, player);
        }
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_REFLECT);
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_LIGHTSCREEN));
    }
}

SINGLE_BATTLE_TEST("Facet Cut does not trigger after neutral damage")
{
    GIVEN {
        PLAYER(SPECIES_DIANCIE) { HP(400); MaxHP(400); Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_FACET_CUT); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(100); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_FACET_CUT);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_REFLECT, player);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_LIGHT_SCREEN, player);
        }
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_REFLECT));
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_LIGHTSCREEN));
    }
}
