#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_MAGICAL_LEAF].type == TYPE_GRASS);
    ASSUME(gBattleMoves[MOVE_RAZOR_LEAF].type == TYPE_GRASS);
    ASSUME(gBattleMoves[MOVE_RAZOR_LEAF].split == SPLIT_PHYSICAL);
    ASSUME(gBattleMoves[MOVE_INGRAIN].effect == EFFECT_INGRAIN);
    ASSUME(gBattleMoves[MOVE_WOOD_HAMMER].type == TYPE_GRASS);
    ASSUME(gBattleMoves[MOVE_WOOD_HAMMER].effect == EFFECT_RECOIL_33);
}

SINGLE_BATTLE_TEST("Uproot triggers only on the first Grass-type move after switch-in")
{
    GIVEN {
        PLAYER(SPECIES_TORTERRA) { Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_UPROOT); Moves(MOVE_MAGICAL_LEAF); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MAGICAL_LEAF); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_MAGICAL_LEAF); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MAGICAL_LEAF, player);
        ABILITY_POPUP(player, ABILITY_UPROOT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_INGRAIN, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MAGICAL_LEAF, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_UPROOT);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_INGRAIN, player);
        }
    } THEN {
        EXPECT(gStatuses3[B_POSITION_PLAYER_LEFT] & STATUS3_ROOTED);
    }
}

SINGLE_BATTLE_TEST("Uproot does not consume its trigger on non-Grass moves")
{
    GIVEN {
        PLAYER(SPECIES_TORTERRA) { Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_UPROOT); Moves(MOVE_TACKLE, MOVE_MAGICAL_LEAF); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_MAGICAL_LEAF); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ABILITY_POPUP(player, ABILITY_UPROOT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_INGRAIN, player);
    }
}

SINGLE_BATTLE_TEST("Uproot allows a rooted user to switch out and refreshes after switching back in")
{
    GIVEN {
        PLAYER(SPECIES_TORTERRA) { Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_UPROOT); Moves(MOVE_MAGICAL_LEAF); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MAGICAL_LEAF); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_MAGICAL_LEAF); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_UPROOT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_INGRAIN, player);
        ABILITY_POPUP(player, ABILITY_UPROOT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_INGRAIN, player);
    }
}

SINGLE_BATTLE_TEST("Uproot makes rooted Grass-type moves use Defense for damage", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_STILL_WATER; }
    PARAMETRIZE { uniqueAbility = ABILITY_UPROOT; }

    GIVEN {
        PLAYER(SPECIES_TORTERRA) { Attack(50); Defense(200); Ability(ABILITY_OVERGROW); UniqueAbility(uniqueAbility); Moves(MOVE_INGRAIN, MOVE_RAZOR_LEAF); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Defense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_INGRAIN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_RAZOR_LEAF); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[1].damage, results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Uproot does not call Ingrain if the user faints before move end")
{
    GIVEN {
        PLAYER(SPECIES_TORTERRA) { HP(1); Attack(999); Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_UPROOT); Moves(MOVE_WOOD_HAMMER); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Defense(1); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WOOD_HAMMER); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WOOD_HAMMER, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_UPROOT);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_INGRAIN, player);
        }
    } THEN {
        EXPECT_EQ(player->hp, 0);
    }
}

SINGLE_BATTLE_TEST("Uproot still calls Ingrain if Rock Head prevents pending recoil")
{
    GIVEN {
        PLAYER(SPECIES_TORTERRA) { HP(1); Attack(1); Ability(ABILITY_ROCK_HEAD); UniqueAbility(ABILITY_UPROOT); Moves(MOVE_WOOD_HAMMER); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Defense(999); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WOOD_HAMMER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WOOD_HAMMER, player);
        ABILITY_POPUP(player, ABILITY_UPROOT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_INGRAIN, player);
    } THEN {
        EXPECT_GT(player->hp, 1);
        EXPECT(gStatuses3[B_POSITION_PLAYER_LEFT] & STATUS3_ROOTED);
    }
}
