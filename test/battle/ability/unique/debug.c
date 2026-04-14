#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BUG_BITE].type == TYPE_BUG);
    ASSUME(gBattleMoves[MOVE_SWIFT].type != TYPE_BUG);
}

SINGLE_BATTLE_TEST("Debug heals for three-quarters of damage dealt by Bug-type moves")
{
    s16 damage;
    s16 healed;

    GIVEN {
        PLAYER(SPECIES_PORYGON) { HP(60); Speed(100); Ability(ABILITY_TRACE); UniqueAbility(ABILITY_DEBUG); Moves(MOVE_BUG_BITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BUG_BITE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BUG_BITE, player);
        HP_BAR(opponent, captureDamage: &damage);
        ABILITY_POPUP(player, ABILITY_DEBUG);
        HP_BAR(player, captureDamage: &healed);
    } THEN {
        EXPECT_MUL_EQ(damage, Q_4_12(-3.0 / 4.0), healed);
    }
}

SINGLE_BATTLE_TEST("Debug does not heal from non-Bug moves")
{
    GIVEN {
        PLAYER(SPECIES_PORYGON) { HP(60); Speed(100); Ability(ABILITY_TRACE); UniqueAbility(ABILITY_DEBUG); Moves(MOVE_SWIFT); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWIFT); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_DEBUG);
            HP_BAR(player);
        }
    }
}
