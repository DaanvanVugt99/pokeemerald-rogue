#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ENCORE].effect == EFFECT_ENCORE);
    ASSUME(gBattleMoves[MOVE_DISABLE].effect == EFFECT_DISABLE);
    ASSUME(gBattleMoves[MOVE_TORMENT].effect == EFFECT_TORMENT);
    ASSUME(gBattleMoves[MOVE_ATTRACT].effect == EFFECT_ATTRACT);
}

SINGLE_BATTLE_TEST("Beguile badly poisons targets after disruptive status moves")
{
    u32 move;

    PARAMETRIZE { move = MOVE_ENCORE; }
    PARAMETRIZE { move = MOVE_DISABLE; }
    PARAMETRIZE { move = MOVE_TORMENT; }

    GIVEN {
        PLAYER(SPECIES_SALAZZLE) { Speed(1); Gender(MON_FEMALE); Ability(ABILITY_CORROSION); UniqueAbility(ABILITY_BEGUILE); Moves(MOVE_ENCORE, MOVE_DISABLE, MOVE_TORMENT, MOVE_ATTRACT); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Gender(MON_MALE); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_SPLASH, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        ABILITY_POPUP(player, ABILITY_BEGUILE);
        MESSAGE("Foe Wobbuffet is badly poisoned!");
        STATUS_ICON(opponent, STATUS1_TOXIC_POISON);
    } THEN {
        EXPECT(opponent->status1 & STATUS1_TOXIC_POISON);
    }
}

SINGLE_BATTLE_TEST("Beguile badly poisons targets after Attract")
{
    GIVEN {
        PLAYER(SPECIES_SALAZZLE) { Gender(MON_FEMALE); Ability(ABILITY_CORROSION); UniqueAbility(ABILITY_BEGUILE); Moves(MOVE_ATTRACT); }
        OPPONENT(SPECIES_TAUROS) { Ability(ABILITY_NONE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ATTRACT); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BEGUILE);
        MESSAGE("Foe Tauros is badly poisoned!");
        STATUS_ICON(opponent, STATUS1_TOXIC_POISON);
    } THEN {
        EXPECT(opponent->status1 & STATUS1_TOXIC_POISON);
    }
}

SINGLE_BATTLE_TEST("Beguile does not trigger after unrelated status moves")
{
    GIVEN {
        PLAYER(SPECIES_SALAZZLE) { Gender(MON_FEMALE); Ability(ABILITY_CORROSION); UniqueAbility(ABILITY_BEGUILE); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Gender(MON_MALE); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_BEGUILE);
            MESSAGE("Foe Wobbuffet is badly poisoned!");
        }
    } THEN {
        EXPECT_EQ(opponent->status1 & STATUS1_TOXIC_POISON, 0);
    }
}

SINGLE_BATTLE_TEST("Beguile respects poison immunity")
{
    GIVEN {
        PLAYER(SPECIES_SALAZZLE) { Gender(MON_FEMALE); Ability(ABILITY_CORROSION); UniqueAbility(ABILITY_BEGUILE); Moves(MOVE_TORMENT); }
        OPPONENT(SPECIES_WOBBUFFET) { Gender(MON_MALE); Ability(ABILITY_IMMUNITY); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TORMENT); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TORMENT, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_BEGUILE);
            MESSAGE("Foe Wobbuffet is badly poisoned!");
        }
    } THEN {
        EXPECT_EQ(opponent->status1 & STATUS1_TOXIC_POISON, 0);
    }
}
