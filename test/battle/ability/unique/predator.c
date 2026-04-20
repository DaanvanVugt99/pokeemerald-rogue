#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WING_ATTACK].type == TYPE_FLYING);
    ASSUME(gBattleMoves[MOVE_DRAGON_CLAW].type != TYPE_FLYING);
}

SINGLE_BATTLE_TEST("Predator heals for one-quarter of damage dealt by Flying-type moves below half HP")
{
    s16 damage;
    s16 healed;

    GIVEN {
        PLAYER(SPECIES_SALAMENCE) { HP(80); MaxHP(200); Speed(100); Ability(ABILITY_INTIMIDATE); Moves(MOVE_WING_ATTACK); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WING_ATTACK); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WING_ATTACK, player);
        HP_BAR(opponent, captureDamage: &damage);
        ABILITY_POPUP(player, ABILITY_PREDATOR);
        HP_BAR(player, captureDamage: &healed);
        MESSAGE("Foe Wobbuffet had its energy drained!");
    } THEN {
        EXPECT_MUL_EQ(damage, Q_4_12(-1.0 / 4.0), healed);
    }
}

SINGLE_BATTLE_TEST("Predator does not heal at exactly half HP")
{
    GIVEN {
        PLAYER(SPECIES_SALAMENCE) { HP(100); MaxHP(200); Speed(100); Ability(ABILITY_INTIMIDATE); Moves(MOVE_WING_ATTACK); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WING_ATTACK); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_PREDATOR);
            HP_BAR(player);
            MESSAGE("Foe Wobbuffet had its energy drained!");
        }
    }
}

SINGLE_BATTLE_TEST("Predator does not heal from non-Flying moves")
{
    GIVEN {
        PLAYER(SPECIES_SALAMENCE) { HP(80); MaxHP(200); Speed(100); Ability(ABILITY_INTIMIDATE); Moves(MOVE_DRAGON_CLAW); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_CLAW); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_PREDATOR);
            HP_BAR(player);
            MESSAGE("Foe Wobbuffet had its energy drained!");
        }
    }
}
