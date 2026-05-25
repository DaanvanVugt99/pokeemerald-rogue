#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_MEGA_PUNCH].punchingMove);
    ASSUME(gBattleMoves[MOVE_MEGA_PUNCH].type == TYPE_FIGHTING);
    ASSUME(!gBattleMoves[MOVE_TACKLE].punchingMove);
    ASSUME(gBattleMoves[MOVE_SMACK_DOWN].power == 50);
}

SINGLE_BATTLE_TEST("Rocket Fist uses Smack Down after punching moves")
{
    GIVEN {
        PLAYER(SPECIES_GOLURK) { Ability(ABILITY_IRON_FIST); UniqueAbility(ABILITY_ROCKET_FIST); Moves(MOVE_MEGA_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_MEGA_PUNCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MEGA_PUNCH, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_ROCKET_FIST);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SMACK_DOWN, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Rocket Fist does not trigger on non-punching moves", s16 damage)
{
    u16 uniqueAbility;
    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_ROCKET_FIST; }

    GIVEN {
        PLAYER(SPECIES_GOLURK) { Ability(ABILITY_IRON_FIST); UniqueAbility(uniqueAbility); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Rocket Fist follow-up Smack Down is boosted by Iron Fist", s16 firstHit, s16 extraHit)
{
    u16 ability;
    PARAMETRIZE { ability = ABILITY_KLUTZ; }
    PARAMETRIZE { ability = ABILITY_IRON_FIST; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); UniqueAbility(ABILITY_ROCKET_FIST); Attack(120); Moves(MOVE_MEGA_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); Defense(120); MaxHP(1000); HP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_MEGA_PUNCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MEGA_PUNCH, player);
        HP_BAR(opponent, captureDamage: &results[i].firstHit);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SMACK_DOWN, player);
        HP_BAR(opponent, captureDamage: &results[i].extraHit);
    } FINALLY {
        EXPECT_GT(results[1].extraHit, results[0].extraHit);
    }
}
