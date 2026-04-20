#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BITE].power > 30);
}

SINGLE_BATTLE_TEST("Feeding Frenzy uses 30 BP Bite if the hit leaves the target below half HP")
{
    s16 firstHit;
    s16 secondHit;

    GIVEN {
        PLAYER(SPECIES_CARVANHA) { Ability(ABILITY_ROUGH_SKIN); UniqueAbility(ABILITY_FEEDING_FRENZY); Moves(MOVE_SONIC_BOOM); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(100); HP(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SONIC_BOOM); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SONIC_BOOM, player);
        HP_BAR(opponent, captureDamage: &firstHit);
        ABILITY_POPUP(player, ABILITY_FEEDING_FRENZY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BITE, player);
        HP_BAR(opponent, captureDamage: &secondHit);
    } THEN {
        EXPECT_EQ(firstHit, 20);
        EXPECT_GT(secondHit, 0);
        EXPECT_LT(secondHit, gBattleMoves[MOVE_BITE].power);
    }
}

SINGLE_BATTLE_TEST("Feeding Frenzy does not trigger if the target is left at exactly half HP")
{
    GIVEN {
        PLAYER(SPECIES_CARVANHA) { Ability(ABILITY_ROUGH_SKIN); UniqueAbility(ABILITY_FEEDING_FRENZY); Moves(MOVE_SONIC_BOOM); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(100); HP(70); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SONIC_BOOM); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SONIC_BOOM, player);
        HP_BAR(opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_FEEDING_FRENZY);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_BITE, player);
            HP_BAR(opponent);
        }
    } THEN {
        EXPECT_EQ(opponent->hp, 50);
    }
}

SINGLE_BATTLE_TEST("Feeding Frenzy does not trigger if the target was already below half HP")
{
    GIVEN {
        PLAYER(SPECIES_CARVANHA) { Ability(ABILITY_ROUGH_SKIN); UniqueAbility(ABILITY_FEEDING_FRENZY); Moves(MOVE_SONIC_BOOM); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(100); HP(40); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SONIC_BOOM); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SONIC_BOOM, player);
        HP_BAR(opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_FEEDING_FRENZY);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_BITE, player);
            HP_BAR(opponent);
        }
    }
}

SINGLE_BATTLE_TEST("Feeding Frenzy does not trigger after status moves against targets below half HP")
{
    GIVEN {
        PLAYER(SPECIES_CARVANHA) { Ability(ABILITY_ROUGH_SKIN); UniqueAbility(ABILITY_FEEDING_FRENZY); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(100); HP(40); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_FEEDING_FRENZY);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_BITE, player);
            HP_BAR(opponent);
        }
    }
}

DOUBLE_BATTLE_TEST("Feeding Frenzy does not trigger from prior mixed-category damage in the same turn")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_CARVANHA) { Ability(ABILITY_ROUGH_SKIN); UniqueAbility(ABILITY_FEEDING_FRENZY); Moves(MOVE_WATER_PULSE); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(100); HP(65); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft);
            MOVE(playerRight, MOVE_WATER_PULSE, target: opponentLeft);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, playerLeft);
        HP_BAR(opponentLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_PULSE, playerRight);
        HP_BAR(opponentLeft);
        NONE_OF {
            ABILITY_POPUP(playerRight, ABILITY_FEEDING_FRENZY);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_BITE, playerRight);
            HP_BAR(opponentLeft);
        }
    }
}
