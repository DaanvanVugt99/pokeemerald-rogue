#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gSpeciesInfo[SPECIES_GLASTRIER].types[0] == TYPE_ICE || gSpeciesInfo[SPECIES_GLASTRIER].types[1] == TYPE_ICE);
    ASSUME(gBattleMoves[MOVE_FLAMETHROWER].type == TYPE_FIRE);
    ASSUME(IS_MOVE_SPECIAL(MOVE_FLAMETHROWER));
    ASSUME(gBattleMoves[MOVE_SNOWSCAPE].effect == EFFECT_SNOWSCAPE);
}

SINGLE_BATTLE_TEST("Frozen Bastion reduces super effective damage in Snow", s16 damage)
{
    bool32 snow;

    PARAMETRIZE { snow = FALSE; }
    PARAMETRIZE { snow = TRUE; }

    GIVEN {
        PLAYER(SPECIES_GLASTRIER) { HP(500); MaxHP(500); Speed(50); SpDefense(100); Ability(ABILITY_CHILLING_NEIGH); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); SpAttack(100); Moves(MOVE_CELEBRATE, MOVE_SNOWSCAPE, MOVE_FLAMETHROWER); }
    } WHEN {
        TURN { MOVE(opponent, snow ? MOVE_SNOWSCAPE : MOVE_CELEBRATE); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_FLAMETHROWER, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
        MESSAGE("It's super effective!");
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.65), results[1].damage);
    }
}
