#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_MEGA_PUNCH].punchingMove);
    ASSUME(gBattleMoves[MOVE_MEGA_PUNCH].makesContact);
    ASSUME(!gBattleMoves[MOVE_TACKLE].punchingMove);
    ASSUME(gBattleMoves[MOVE_FOCUS_ENERGY].effect == EFFECT_FOCUS_ENERGY);
    ASSUME(gItems[ITEM_ROCKY_HELMET].holdEffect == HOLD_EFFECT_ROCKY_HELMET);
}

SINGLE_BATTLE_TEST("Spellfist triggers only on the first punching move after switch-in")
{
    GIVEN {
        PLAYER(SPECIES_INFERNAPE) { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_SPELLFIST); Moves(MOVE_MEGA_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MEGA_PUNCH); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_MEGA_PUNCH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MEGA_PUNCH, player);
        ABILITY_POPUP(player, ABILITY_SPELLFIST);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FOCUS_ENERGY, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MEGA_PUNCH, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SPELLFIST);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_FOCUS_ENERGY, player);
        }
    } THEN {
        EXPECT(player->status2 & STATUS2_FOCUS_ENERGY);
    }
}

SINGLE_BATTLE_TEST("Spellfist does not consume its trigger on non-punching moves")
{
    GIVEN {
        PLAYER(SPECIES_INFERNAPE) { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_SPELLFIST); Moves(MOVE_TACKLE, MOVE_MEGA_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_MEGA_PUNCH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ABILITY_POPUP(player, ABILITY_SPELLFIST);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FOCUS_ENERGY, player);
    }
}

SINGLE_BATTLE_TEST("Spellfist refreshes after the user switches out and back in")
{
    GIVEN {
        PLAYER(SPECIES_INFERNAPE) { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_SPELLFIST); Moves(MOVE_MEGA_PUNCH); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MEGA_PUNCH); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_MEGA_PUNCH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SPELLFIST);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FOCUS_ENERGY, player);
        ABILITY_POPUP(player, ABILITY_SPELLFIST);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FOCUS_ENERGY, player);
    }
}

SINGLE_BATTLE_TEST("Spellfist punching moves target Special Defense for damage", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_STILL_WATER; }
    PARAMETRIZE { uniqueAbility = ABILITY_SPELLFIST; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(200); Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_MEGA_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Defense(50); SpDefense(200); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MEGA_PUNCH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Spellfist does not call Focus Energy if the user faints before move end")
{
    GIVEN {
        PLAYER(SPECIES_INFERNAPE) { HP(1); Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_SPELLFIST); Moves(MOVE_MEGA_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_ROCKY_HELMET); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MEGA_PUNCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MEGA_PUNCH, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SPELLFIST);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_FOCUS_ENERGY, player);
        }
    } THEN {
        EXPECT_EQ(player->hp, 0);
    }
}
