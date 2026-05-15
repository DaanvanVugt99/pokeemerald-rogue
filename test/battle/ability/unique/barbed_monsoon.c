#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ACID_RAIN].effect == EFFECT_CORROSIVE_CLOUDS);
    ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
    ASSUME(!gBattleMoves[MOVE_WATER_GUN].makesContact);
}

SINGLE_BATTLE_TEST("Barbed Monsoon poisons contact move targets used by or against the holder during Acid Rain")
{
    bool32 holderAttacks;

    PARAMETRIZE { holderAttacks = TRUE; }
    PARAMETRIZE { holderAttacks = FALSE; }

    GIVEN {
        PLAYER(SPECIES_OVERQWIL) { Speed(100); Ability(ABILITY_SWIFT_SWIM); Moves(MOVE_ACID_RAIN, MOVE_TACKLE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ACID_RAIN); MOVE(opponent, MOVE_CELEBRATE); }
        if (holderAttacks)
            TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        else
            TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        if (holderAttacks)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
            HP_BAR(opponent);
        }
        else
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
            HP_BAR(player);
        }
        ABILITY_POPUP(player, ABILITY_BARBED_MONSOON);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_PSN, opponent);
        MESSAGE("Foe Wobbuffet was poisoned by Overqwil's Barbed Monsoon!");
    } THEN {
        EXPECT(opponent->status1 & STATUS1_POISON);
    }
}

SINGLE_BATTLE_TEST("Barbed Monsoon does not poison without Acid Rain or without contact")
{
    u16 setupMove;
    u16 attackMove;

    PARAMETRIZE { setupMove = MOVE_CELEBRATE; attackMove = MOVE_TACKLE; }
    PARAMETRIZE { setupMove = MOVE_ACID_RAIN; attackMove = MOVE_WATER_GUN; }

    GIVEN {
        PLAYER(SPECIES_OVERQWIL) { Speed(100); Ability(ABILITY_SWIFT_SWIM); Moves(MOVE_CELEBRATE, MOVE_ACID_RAIN, MOVE_TACKLE, MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, setupMove); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, attackMove); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_BARBED_MONSOON);
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}
