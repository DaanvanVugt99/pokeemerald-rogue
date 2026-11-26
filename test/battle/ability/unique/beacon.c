#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Beacon sets Ion Deluge again on re-entry but grants only one Wish")
{
    GIVEN {
        PLAYER(SPECIES_AMPHAROS) { Ability(ABILITY_OVERCOAT); UniqueAbility(ABILITY_BEACON); HP(400); MaxHP(400); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(100); MaxHP(400); }
        OPPONENT(SPECIES_WYNAUT) { Ability(ABILITY_TELEPATHY); Attack(100); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_TACKLE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BEACON);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ION_DELUGE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        MESSAGE("It's not very effective…");
        ABILITY_POPUP(player, ABILITY_BEACON);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WISH, player);
        MESSAGE("Go! Wobbuffet!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        MESSAGE("Ampharos's WISH came true!");
        HP_BAR(player);
        MESSAGE("Go! Ampharos!");
        ABILITY_POPUP(player, ABILITY_BEACON);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ION_DELUGE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        MESSAGE("It's not very effective…");
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_WISH, player);
    } THEN {
        EXPECT_EQ(gWishFutureKnock.wishCounter[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)], 0);
        EXPECT(!(gFieldStatuses & STATUS_FIELD_ION_DELUGE));
    }
}

SINGLE_BATTLE_TEST("Beacon saves Wish for a resisted hit after Ion Deluge expires")
{
    GIVEN {
        PLAYER(SPECIES_AMPHAROS) { Ability(ABILITY_OVERCOAT); UniqueAbility(ABILITY_BEACON); HP(400); MaxHP(400); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(100); SpAttack(100); Moves(MOVE_BITE, MOVE_THUNDER_SHOCK); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_BITE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_THUNDER_SHOCK); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ION_DELUGE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BITE, opponent);
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_WISH, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDER_SHOCK, opponent);
        ABILITY_POPUP(player, ABILITY_BEACON);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WISH, player);
    } THEN {
        EXPECT_EQ(gWishFutureKnock.wishCounter[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)], 1);
    }
}

SINGLE_BATTLE_TEST("Beacon does not grant Wish when its holder faints from a resisted hit")
{
    GIVEN {
        PLAYER(SPECIES_AMPHAROS) { Ability(ABILITY_OVERCOAT); UniqueAbility(ABILITY_BEACON); HP(1); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT) { Ability(ABILITY_TELEPATHY); Attack(100); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); SEND_OUT(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ION_DELUGE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        MESSAGE("It's not very effective…");
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_WISH, player);
    } THEN {
        EXPECT(!(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & 1));
    }
}

DOUBLE_BATTLE_TEST("Beacon converts a partner's Normal move into Electric on entry")
{
    GIVEN {
        PLAYER(SPECIES_MAREEP) { Ability(ABILITY_OVERCOAT); UniqueAbility(ABILITY_BEACON); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Attack(100); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_GOLBAT) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerRight, MOVE_TACKLE, target: opponentLeft); }
        TURN { MOVE(playerRight, MOVE_TACKLE, target: opponentLeft); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_BEACON);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ION_DELUGE, playerLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, playerRight);
        MESSAGE("It's super effective!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, playerRight);
        NOT MESSAGE("It's super effective!");
    }
}
