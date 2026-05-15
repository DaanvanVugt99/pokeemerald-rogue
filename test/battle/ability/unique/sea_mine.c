#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
    ASSUME(!gBattleMoves[MOVE_SWIFT].makesContact);
}

SINGLE_BATTLE_TEST("Sea Mine sets Electric Terrain on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_PINCURCHIN) { Ability(ABILITY_LIGHTNING_ROD); UniqueAbility(ABILITY_SEA_MINE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SEA_MINE);
        MESSAGE("An electric current runs across\nthe battlefield!");
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_ELECTRIC_TERRAIN);
    }
}

SINGLE_BATTLE_TEST("Sea Mine paralyzes the first contact attacker each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_PINCURCHIN) { HP(500); MaxHP(500); Speed(1); Ability(ABILITY_LIGHTNING_ROD); UniqueAbility(ABILITY_SEA_MINE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_SEA_MINE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ABILITY_POPUP(opponent, ABILITY_SEA_MINE);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_PRZ, player);
        STATUS_ICON(player, paralysis: TRUE);
    } THEN {
        EXPECT(player->status1 & STATUS1_PARALYSIS);
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Sea Mine is not consumed by non-contact moves")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_SWIFT, MOVE_TACKLE); }
        OPPONENT(SPECIES_PINCURCHIN) { HP(500); MaxHP(500); Speed(1); Ability(ABILITY_LIGHTNING_ROD); UniqueAbility(ABILITY_SEA_MINE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWIFT); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWIFT, player);
        NONE_OF {
            ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_PRZ, player);
            STATUS_ICON(player, paralysis: TRUE);
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ABILITY_POPUP(opponent, ABILITY_SEA_MINE);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_PRZ, player);
        STATUS_ICON(player, paralysis: TRUE);
    } THEN {
        EXPECT(player->status1 & STATUS1_PARALYSIS);
    }
}
