#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BITE].power > 30);
}

SINGLE_BATTLE_TEST("Feeding Frenzy uses 30 BP Bite when hitting a target at half HP or below")
{
    s16 firstHit;
    s16 secondHit;

    GIVEN {
        PLAYER(SPECIES_CARVANHA) { Ability(ABILITY_ROUGH_SKIN); UniqueAbility(ABILITY_FEEDING_FRENZY); Moves(MOVE_SONIC_BOOM); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(100); HP(50); Moves(MOVE_CELEBRATE); }
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

SINGLE_BATTLE_TEST("Feeding Frenzy does not trigger if the target was above half HP before the hit")
{
    GIVEN {
        PLAYER(SPECIES_CARVANHA) { Ability(ABILITY_ROUGH_SKIN); UniqueAbility(ABILITY_FEEDING_FRENZY); Moves(MOVE_SONIC_BOOM); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(100); HP(60); Moves(MOVE_CELEBRATE); }
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
        EXPECT_EQ(opponent->hp, 40);
    }
}
