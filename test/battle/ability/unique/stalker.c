#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_LEAF_BLADE].slicingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].slicingMove);
    ASSUME(gBattleMoves[MOVE_CELEBRATE].priority == 0);
}

SINGLE_BATTLE_TEST("Stalker scales slicing damage by qualifying ally count", s16 damage)
{
    u16 ally1, ally2, ally3;
    PARAMETRIZE { ally1 = SPECIES_WOBBUFFET; ally2 = SPECIES_WOBBUFFET; ally3 = SPECIES_WOBBUFFET; } // 0 allies
    PARAMETRIZE { ally1 = SPECIES_SHIFTRY;   ally2 = SPECIES_WOBBUFFET; ally3 = SPECIES_WOBBUFFET; } // 1 ally (dual-type counts once)
    PARAMETRIZE { ally1 = SPECIES_SHIFTRY;   ally2 = SPECIES_DRATINI;   ally3 = SPECIES_WOBBUFFET; } // 2 allies
    PARAMETRIZE { ally1 = SPECIES_SHIFTRY;   ally2 = SPECIES_DRATINI;   ally3 = SPECIES_ODDISH; }    // 3 allies

    GIVEN {
        PLAYER(SPECIES_SCEPTILE) { Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_SUNSTALKER); Moves(MOVE_LEAF_BLADE); }
        PLAYER(ally1);
        PLAYER(ally2);
        PLAYER(ally3);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_LEAF_BLADE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.1), results[1].damage);
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.2), results[2].damage);
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.3), results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Stalker gives +1 priority only at 3 qualifying allies, and only for first slicing move after switch-in")
{
    GIVEN {
        PLAYER(SPECIES_SCEPTILE) { Speed(50); Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_SUNSTALKER); Moves(MOVE_LEAF_BLADE); }
        PLAYER(SPECIES_ODDISH) { Speed(40); }
        PLAYER(SPECIES_DRATINI) { Speed(40); }
        PLAYER(SPECIES_POOCHYENA) { Speed(40); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_LEAF_BLADE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_LEAF_BLADE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEAF_BLADE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEAF_BLADE, player);
    }
}

SINGLE_BATTLE_TEST("Stalker does not grant priority at fewer than 3 qualifying allies")
{
    GIVEN {
        PLAYER(SPECIES_SCEPTILE) { Speed(50); Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_SUNSTALKER); Moves(MOVE_LEAF_BLADE); }
        PLAYER(SPECIES_SHIFTRY) { Speed(40); }
        PLAYER(SPECIES_DRATINI) { Speed(40); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(40); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_LEAF_BLADE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEAF_BLADE, player);
    }
}
