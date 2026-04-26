#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BRAVE_BIRD].effect == EFFECT_RECOIL_33);
    ASSUME(gBattleMoves[MOVE_BRAVE_BIRD].accuracy == 100);
}

SINGLE_BATTLE_TEST("Aerial Assault prevents recoil when moving before the target")
{
    GIVEN {
        PLAYER(SPECIES_STARAPTOR) { HP(100); MaxHP(100); Speed(100); Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_AERIAL_ASSAULT); Moves(MOVE_BRAVE_BIRD); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BRAVE_BIRD); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BRAVE_BIRD, player);
        HP_BAR(opponent);
        NONE_OF {
            MESSAGE("Staraptor is hit with recoil!");
            HP_BAR(player);
        }
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Aerial Assault does not prevent recoil when moving after the target")
{
    GIVEN {
        PLAYER(SPECIES_STARAPTOR) { HP(100); MaxHP(100); Speed(50); Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_AERIAL_ASSAULT); Moves(MOVE_BRAVE_BIRD); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BRAVE_BIRD); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BRAVE_BIRD, player);
        HP_BAR(opponent);
        MESSAGE("Staraptor is hit with recoil!");
    } THEN {
        EXPECT_LT(player->hp, player->maxHP);
    }
}
