#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_DRAGON_HAMMER].split == SPLIT_PHYSICAL);
    ASSUME(gBattleMoves[MOVE_DRAGON_HAMMER].effect == EFFECT_HIT);
    ASSUME(gBattleMoves[MOVE_GROWTH].effect == EFFECT_GROWTH);
}

SINGLE_BATTLE_TEST("Tall Tale makes Dragon Hammer use Sp. Atk if it is higher", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_TALL_TALE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(50); SpAttack(200); Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_DRAGON_HAMMER); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Defense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_HAMMER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[1].damage, results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Tall Tale uses Growth after Dragon Hammer knocks out a target")
{
    GIVEN {
        PLAYER(SPECIES_EXEGGUTOR_ALOLAN) { Ability(ABILITY_FRISK); UniqueAbility(ABILITY_TALL_TALE); Moves(MOVE_DRAGON_HAMMER); }
        OPPONENT(SPECIES_CATERPIE) { HP(1); MaxHP(1); Ability(ABILITY_SHIELD_DUST); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_HAMMER); SEND_OUT(opponent, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_HAMMER, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_TALL_TALE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWTH, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
    }
}
