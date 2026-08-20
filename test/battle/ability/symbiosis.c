#include "global.h"
#include "test/battle.h"

DOUBLE_BATTLE_TEST("A spread move does not consume items received through Symbiosis as resistance Berries")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_EARTHQUAKE].target == MOVE_TARGET_FOES_AND_ALLY);
        ASSUME(gBattleMoves[MOVE_EARTHQUAKE].type == TYPE_GROUND);
        ASSUME(gItems[ITEM_SHUCA_BERRY].holdEffect == HOLD_EFFECT_RESIST_BERRY);
        ASSUME(gItems[ITEM_SHUCA_BERRY].holdEffectParam == TYPE_GROUND);
        PLAYER(SPECIES_ORANGURU) { Ability(ABILITY_SYMBIOSIS); Item(ITEM_POTION); }
        PLAYER(SPECIES_PIKACHU) { Item(ITEM_SHUCA_BERRY); }
        OPPONENT(SPECIES_PIKACHU) { Item(ITEM_SHUCA_BERRY); }
        OPPONENT(SPECIES_ORANGURU) { Ability(ABILITY_SYMBIOSIS); Item(ITEM_POTION); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_EARTHQUAKE); }
    } THEN {
        EXPECT_EQ(playerLeft->item, ITEM_NONE);
        EXPECT_EQ(playerRight->item, ITEM_POTION);
        EXPECT_EQ(opponentLeft->item, ITEM_POTION);
        EXPECT_EQ(opponentRight->item, ITEM_NONE);
    }
}
