#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_PHYSICAL(MOVE_TACKLE));
    ASSUME(IS_MOVE_SPECIAL(MOVE_DRAGON_PULSE));
}

SINGLE_BATTLE_TEST("Skyscraper halves only the first special hit taken each battle")
{
    s16 physicalDamage;
    s16 firstSpecialDamage;
    s16 secondSpecialDamage;

    GIVEN {
        PLAYER(SPECIES_DURALUDON) { HP(500); MaxHP(500); Defense(100); SpDefense(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(100); SpAttack(100); Moves(MOVE_TACKLE, MOVE_DRAGON_PULSE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_PULSE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_PULSE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        HP_BAR(player, captureDamage: &physicalDamage);
        HP_BAR(player, captureDamage: &firstSpecialDamage);
        HP_BAR(player, captureDamage: &secondSpecialDamage);
    } THEN {
        EXPECT_GT(physicalDamage, 0);
        EXPECT_MUL_EQ(secondSpecialDamage, UQ_4_12(0.5), firstSpecialDamage);
    }
}
