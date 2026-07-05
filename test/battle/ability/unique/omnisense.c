#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Omnisense bounces back status moves")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_TOXIC].effect == EFFECT_TOXIC);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_XATU) { Ability(ABILITY_SYNCHRONIZE); UniqueAbility(ABILITY_OMNISENSE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TOXIC); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_OMNISENSE);
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_TOXIC, player);
        MESSAGE("Wynaut's Toxic was bounced back by Foe Xatu's Omnisense!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TOXIC, opponent);
        STATUS_ICON(player, badPoison: TRUE);
    }
}

SINGLE_BATTLE_TEST("Omnisense reveals and disables the foe's held item on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_SENTRET) { Item(ITEM_POTION); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_XATU) { Ability(ABILITY_SYNCHRONIZE); UniqueAbility(ABILITY_OMNISENSE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_OMNISENSE);
        MESSAGE("Foe Xatu frisked Sentret and disabled its Potion!");
    } THEN {
        EXPECT(gStatuses3[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)] & STATUS3_EMBARGO);
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].embargoTimer, 2);
    }
}
