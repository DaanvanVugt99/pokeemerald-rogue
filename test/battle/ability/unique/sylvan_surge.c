#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_MAGICAL_LEAF].type == TYPE_GRASS);
    ASSUME(gBattleMoves[MOVE_GRASSY_TERRAIN].effect == EFFECT_GRASSY_TERRAIN);
    ASSUME(gSpeciesInfo[SPECIES_ODDISH].types[0] == TYPE_GRASS || gSpeciesInfo[SPECIES_ODDISH].types[1] == TYPE_GRASS);
}

SINGLE_BATTLE_TEST("Sylvan Surge boosts resisted Grass-type move damage like Tinted Lens", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_SYLVAN_SURGE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_MAGICAL_LEAF); }
        OPPONENT(SPECIES_ODDISH) { HP(300); MaxHP(300); }
    } WHEN {
        TURN { MOVE(player, MOVE_MAGICAL_LEAF); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(2.0), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Sylvan Surge multiplies Speed by 1.5x in Grassy Terrain")
{
    GIVEN {
        PLAYER(SPECIES_LEAFEON) { Speed(80); Ability(ABILITY_LEAF_GUARD); Moves(MOVE_CELEBRATE, MOVE_MAGICAL_LEAF); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_GRASSY_TERRAIN, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_GRASSY_TERRAIN); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_MAGICAL_LEAF); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRASSY_TERRAIN, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MAGICAL_LEAF, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}
