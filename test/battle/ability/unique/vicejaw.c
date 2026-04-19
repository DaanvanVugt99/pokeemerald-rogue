#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BITE].bitingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].bitingMove);
}

SINGLE_BATTLE_TEST("Vicejaw boosts biting damage against targets that already moved this turn", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_VICEJAW; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_BITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Vicejaw does not boost biting damage if the target has not moved yet", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_VICEJAW; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_BITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Vicejaw does not boost non-biting moves", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_VICEJAW; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}
