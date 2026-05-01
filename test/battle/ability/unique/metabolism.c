#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_SLUDGE].type == TYPE_POISON);
    ASSUME(gBattleMoves[MOVE_TOXIC].effect == EFFECT_TOXIC);
    ASSUME(gBattleMoves[MOVE_POISON_POWDER].effect == EFFECT_POISON);
    ASSUME(gBattleMoves[MOVE_WILL_O_WISP].effect == EFFECT_WILL_O_WISP);
}

SINGLE_BATTLE_TEST("Metabolism doubles Fire- and Poison-type move damage", s16 damage)
{
    u16 move;
    bool32 hasMetabolism;

    PARAMETRIZE { move = MOVE_EMBER; hasMetabolism = FALSE; }
    PARAMETRIZE { move = MOVE_EMBER; hasMetabolism = TRUE; }
    PARAMETRIZE { move = MOVE_SLUDGE; hasMetabolism = FALSE; }
    PARAMETRIZE { move = MOVE_SLUDGE; hasMetabolism = TRUE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) {
            Ability(ABILITY_SHADOW_TAG);
            if (hasMetabolism)
                UniqueAbility(ABILITY_METABOLISM);
            Moves(move);
        }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, move, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(2.0), results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, UQ_4_12(2.0), results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Metabolism prevents burn and poison")
{
    GIVEN {
        PLAYER(SPECIES_DIGGERSBY) { Ability(ABILITY_HUGE_POWER); UniqueAbility(ABILITY_METABOLISM); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_GENGAR) { Moves(MOVE_WILL_O_WISP, MOVE_TOXIC, MOVE_POISON_POWDER); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WILL_O_WISP); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TOXIC); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_POISON_POWDER); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_METABOLISM);
        MESSAGE("Diggersby's Metabolism prevents burns!");
        ABILITY_POPUP(player, ABILITY_METABOLISM);
        MESSAGE("Diggersby's Metabolism prevents poisoning!");
        ABILITY_POPUP(player, ABILITY_METABOLISM);
        MESSAGE("Diggersby's Metabolism prevents poisoning!");
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_WILL_O_WISP, opponent);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TOXIC, opponent);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_POISON_POWDER, opponent);
        }
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}
