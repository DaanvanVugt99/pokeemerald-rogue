#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ACID].power == 40);
    ASSUME(gBattleMoves[MOVE_ACID].type == TYPE_POISON);
}

SINGLE_BATTLE_TEST("Acid Reflux uses Acid after this Pokemon takes damage")
{
    GIVEN {
        PLAYER(SPECIES_GULPIN) { Ability(ABILITY_LIQUID_OOZE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_ACID_REFLUX);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ACID, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Acid Reflux uses Acid at 20 BP", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_ACID_REFLUX; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_ACID, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
    } WHEN {
        if (uniqueAbility == ABILITY_NONE)
            TURN { MOVE(player, MOVE_ACID); MOVE(opponent, MOVE_CELEBRATE); }
        else
            TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, UQ_4_12(2.0), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Acid Reflux does not trigger if this Pokemon takes no damage")
{
    GIVEN {
        PLAYER(SPECIES_GULPIN) { Ability(ABILITY_LIQUID_OOZE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_GROWL); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_GROWL); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_ACID_REFLUX);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_ACID, player);
            HP_BAR(opponent);
        }
    }
}
