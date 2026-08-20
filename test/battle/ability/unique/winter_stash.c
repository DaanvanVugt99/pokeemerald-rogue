#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_DRAGON_RAGE].effect == EFFECT_DRAGON_RAGE);
    ASSUME(gBattleMoves[MOVE_SNOWSCAPE].effect == EFFECT_SNOWSCAPE);
    ASSUME(gBattleMoves[MOVE_HYPNOSIS].accuracy == 60);
    ASSUME(gItems[ITEM_MICLE_BERRY].holdEffect == HOLD_EFFECT_MICLE_BERRY);
    ASSUME(gItems[ITEM_SHUCA_BERRY].holdEffect == HOLD_EFFECT_RESIST_BERRY);
}

SINGLE_BATTLE_TEST("Winter Stash doubles Berry healing")
{
    GIVEN {
        PLAYER(SPECIES_PACHIRISU) { Ability(ABILITY_RUN_AWAY); UniqueAbility(ABILITY_WINTER_STASH); MaxHP(300); HP(160); Item(ITEM_SITRUS_BERRY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } THEN {
        EXPECT_EQ(player->hp, 270);
        EXPECT_EQ(player->item, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Winter Stash heals extra HP after eating a Berry in Snow")
{
    GIVEN {
        PLAYER(SPECIES_PACHIRISU) { Ability(ABILITY_RUN_AWAY); UniqueAbility(ABILITY_WINTER_STASH); MaxHP(300); HP(160); Item(ITEM_SITRUS_BERRY); Moves(MOVE_SNOWSCAPE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE, MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SNOWSCAPE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_WINTER_STASH);
    } THEN {
        EXPECT_EQ(player->hp, 300);
        EXPECT_EQ(player->item, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Winter Stash doubles stat Berry boosts")
{
    GIVEN {
        PLAYER(SPECIES_PACHIRISU) { Ability(ABILITY_RUN_AWAY); UniqueAbility(ABILITY_WINTER_STASH); MaxHP(160); HP(80); Item(ITEM_LIECHI_BERRY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        MESSAGE("Using Liechi Berry, the Attack of Pachirisu sharply rose!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->item, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Winter Stash does not add extra Berry healing outside Snow")
{
    GIVEN {
        PLAYER(SPECIES_PACHIRISU) { Ability(ABILITY_RUN_AWAY); UniqueAbility(ABILITY_WINTER_STASH); MaxHP(300); HP(160); Item(ITEM_SITRUS_BERRY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } THEN {
        EXPECT_EQ(player->hp, 270);
    }
}

SINGLE_BATTLE_TEST("Winter Stash doubles Micle Berry accuracy as a unique ability")
{
    PASSES_RANDOMLY(84, 100, RNG_ACCURACY);

    GIVEN {
        PLAYER(SPECIES_PACHIRISU) { Speed(1); Ability(ABILITY_RUN_AWAY); UniqueAbility(ABILITY_WINTER_STASH); MaxHP(160); HP(80); Item(ITEM_MICLE_BERRY); Moves(MOVE_HYPNOSIS); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPNOSIS); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_RAGE, opponent);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_BERRY, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPNOSIS, player);
    } THEN {
        EXPECT(opponent->status1 & STATUS1_SLEEP);
    }
}

SINGLE_BATTLE_TEST("Winter Stash doubles resist Berry mitigation as a unique ability", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_WINTER_STASH; }

    GIVEN {
        PLAYER(SPECIES_PACHIRISU) { Ability(ABILITY_RUN_AWAY); UniqueAbility(uniqueAbility); Defense(100); MaxHP(400); HP(400); Item(ITEM_SHUCA_BERRY); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(100); Moves(MOVE_EARTHQUAKE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_EARTHQUAKE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.5), results[1].damage);
    }
}
