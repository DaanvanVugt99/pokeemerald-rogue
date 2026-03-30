#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WING_ATTACK].makesContact);
    ASSUME(!gBattleMoves[MOVE_AIR_SLASH].makesContact);
}

SINGLE_BATTLE_TEST("Vampiric heals for one-third of damage when moving first with a contact move")
{
    s16 damage;
    s16 healed;

    GIVEN {
        PLAYER(SPECIES_CROBAT) { HP(60); Speed(100); Ability(ABILITY_INNER_FOCUS); UniqueAbility(ABILITY_VAMPIRIC); Moves(MOVE_WING_ATTACK); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WING_ATTACK); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WING_ATTACK, player);
        HP_BAR(opponent, captureDamage: &damage);
        ABILITY_POPUP(player, ABILITY_VAMPIRIC);
        HP_BAR(player, captureDamage: &healed);
        MESSAGE("Foe Wobbuffet had its energy drained!");
    } THEN {
        EXPECT_MUL_EQ(damage, Q_4_12(-1.0 / 3.0), healed);
    }
}

SINGLE_BATTLE_TEST("Vampiric does not heal when moving second")
{
    GIVEN {
        PLAYER(SPECIES_CROBAT) { HP(60); Speed(50); Ability(ABILITY_INNER_FOCUS); UniqueAbility(ABILITY_VAMPIRIC); Moves(MOVE_WING_ATTACK); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WING_ATTACK); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_VAMPIRIC);
            HP_BAR(player);
            MESSAGE("Foe Wobbuffet had its energy drained!");
        }
    }
}

SINGLE_BATTLE_TEST("Vampiric does not heal on non-contact moves")
{
    GIVEN {
        PLAYER(SPECIES_CROBAT) { HP(60); Speed(100); Ability(ABILITY_INNER_FOCUS); UniqueAbility(ABILITY_VAMPIRIC); Moves(MOVE_AIR_SLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_AIR_SLASH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_VAMPIRIC);
            HP_BAR(player);
            MESSAGE("Foe Wobbuffet had its energy drained!");
        }
    }
}

SINGLE_BATTLE_TEST("Vampiric is reversed by Liquid Ooze")
{
    s16 damage;
    s16 lostHp;

    GIVEN {
        PLAYER(SPECIES_CROBAT) { HP(120); Speed(100); Ability(ABILITY_INNER_FOCUS); UniqueAbility(ABILITY_VAMPIRIC); Moves(MOVE_WING_ATTACK); }
        OPPONENT(SPECIES_TENTACRUEL) { Ability(ABILITY_LIQUID_OOZE); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WING_ATTACK); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WING_ATTACK, player);
        HP_BAR(opponent, captureDamage: &damage);
        ABILITY_POPUP(player, ABILITY_VAMPIRIC);
        ABILITY_POPUP(opponent, ABILITY_LIQUID_OOZE);
        HP_BAR(player, captureDamage: &lostHp);
        MESSAGE("It sucked up the liquid ooze!");
    } THEN {
        EXPECT_MUL_EQ(damage, Q_4_12(1.0 / 3.0), lostHp);
    }
}
