#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gSpeciesInfo[SPECIES_CHARIZARD].types[0] != TYPE_DRAGON);
    ASSUME(gSpeciesInfo[SPECIES_CHARIZARD].types[1] != TYPE_DRAGON);
}

SINGLE_BATTLE_TEST("Draconic grants STAB to Dragon-type moves", s16 damage)
{
    u16 uniqueAbility;
    PARAMETRIZE { uniqueAbility = ABILITY_SOUNDPROOF; }
    PARAMETRIZE { uniqueAbility = ABILITY_DRACONIC; }
    GIVEN {
        ASSUME(gBattleMoves[MOVE_DRAGON_CLAW].type == TYPE_DRAGON);
        PLAYER(SPECIES_CHARIZARD) { Ability(ABILITY_BLAZE); UniqueAbility(uniqueAbility); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_CLAW); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Draconic boosts Dragon-type move damage below half HP", s16 damage)
{
    u16 hp;
    PARAMETRIZE { hp = 99; }
    PARAMETRIZE { hp = 49; }
    GIVEN {
        ASSUME(gBattleMoves[MOVE_DRAGON_CLAW].type == TYPE_DRAGON);
        PLAYER(SPECIES_CHARIZARD) { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_DRACONIC); MaxHP(99); HP(hp); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_CLAW); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
    }
}
