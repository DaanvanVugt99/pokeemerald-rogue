#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gItems[ITEM_ROCKY_HELMET].holdEffect == HOLD_EFFECT_ROCKY_HELMET);
    ASSUME(gBattleMoves[MOVE_LEAF_BLADE].slicingMove);
    ASSUME(gBattleMoves[MOVE_LEAF_BLADE].makesContact);
    ASSUME(!gBattleMoves[MOVE_TACKLE].slicingMove);
}

SINGLE_BATTLE_TEST("Singularity Prism sets Psychic Terrain after Iron Leaves's first slicing move if it is the only Paradox")
{
    bool32 hasOtherParadox;

    PARAMETRIZE { hasOtherParadox = FALSE; }
    PARAMETRIZE { hasOtherParadox = TRUE; }

    GIVEN {
        PLAYER(SPECIES_IRON_LEAVES) { Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_LEAF_BLADE); }
        if (hasOtherParadox)
            PLAYER(SPECIES_IRON_BUNDLE) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        else
            PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_LEAF_BLADE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEAF_BLADE, player);
        HP_BAR(opponent);
        if (!hasOtherParadox)
            ABILITY_POPUP(player, ABILITY_SINGULARITY_PRISM);
        else
            NOT ABILITY_POPUP(player, ABILITY_SINGULARITY_PRISM);
    } THEN {
        if (hasOtherParadox)
        {
            EXPECT(!(gFieldStatuses & STATUS_FIELD_PSYCHIC_TERRAIN));
            EXPECT(!(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]));
        }
        else
        {
            EXPECT(gFieldStatuses & STATUS_FIELD_PSYCHIC_TERRAIN);
            EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
        }
    }
}

SINGLE_BATTLE_TEST("Singularity Prism only triggers once per battle")
{
    GIVEN {
        PLAYER(SPECIES_IRON_LEAVES) { Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_LEAF_BLADE, MOVE_PLAIN_TERRAIN); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_LEAF_BLADE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_PLAIN_TERRAIN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_LEAF_BLADE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEAF_BLADE, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_SINGULARITY_PRISM);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PLAIN_TERRAIN, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEAF_BLADE, player);
        HP_BAR(opponent);
        NOT ABILITY_POPUP(player, ABILITY_SINGULARITY_PRISM);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_PLAIN_TERRAIN);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

SINGLE_BATTLE_TEST("Singularity Prism does not trigger after non-slicing moves")
{
    GIVEN {
        PLAYER(SPECIES_IRON_LEAVES) { Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
        NOT ABILITY_POPUP(player, ABILITY_SINGULARITY_PRISM);
    } THEN {
        EXPECT(!(gFieldStatuses & STATUS_FIELD_PSYCHIC_TERRAIN));
        EXPECT(!(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]));
    }
}

SINGLE_BATTLE_TEST("Singularity Prism does not trigger if pending Rocky Helmet damage would faint the user")
{
    GIVEN {
        PLAYER(SPECIES_IRON_LEAVES) { HP(1); MaxHP(100); Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_LEAF_BLADE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Item(ITEM_ROCKY_HELMET); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_LEAF_BLADE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEAF_BLADE, player);
        NOT ABILITY_POPUP(player, ABILITY_SINGULARITY_PRISM);
    } THEN {
        EXPECT_EQ(player->hp, 0);
        EXPECT(!(gFieldStatuses & STATUS_FIELD_PSYCHIC_TERRAIN));
        EXPECT(!(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]));
    }
}
