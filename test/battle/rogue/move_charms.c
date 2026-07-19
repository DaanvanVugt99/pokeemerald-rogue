#include "global.h"
#include "test/battle.h"

#include "charm_test.h"

static void ClearMoveCharms(void)
{
    ClearCharmTestState();
}

static void SetMoveCharms(u16 sharpnessCount, u16 jawCount, u16 skillCount)
{
    BeginCharmTestRun();
    AddCharmForTest(ITEM_SHARPNESS_CHARM, sharpnessCount);
    AddCharmForTest(ITEM_STRONG_JAW_CHARM, jawCount);
    AddCharmForTest(ITEM_SKILL_CHARM, skillCount);
    FinishCharmTestSetup();
}

SINGLE_BATTLE_TEST("charms: moves - Sharpness Charm boosts slicing moves and stacks with Sharpness", s16 damage)
{
    u32 ability;
    bool32 hasCharm;

    PARAMETRIZE { ability = ABILITY_KLUTZ;     hasCharm = FALSE; }
    PARAMETRIZE { ability = ABILITY_SHARPNESS; hasCharm = FALSE; }
    PARAMETRIZE { ability = ABILITY_KLUTZ;     hasCharm = TRUE; }
    PARAMETRIZE { ability = ABILITY_SHARPNESS; hasCharm = TRUE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_AERIAL_ACE].slicingMove);
        SetMoveCharms(hasCharm, 0, 0);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Attack(120); Moves(MOVE_AERIAL_ACE); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_AERIAL_ACE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.5), results[1].damage);
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.5), results[2].damage);
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(2.25), results[3].damage);
        ClearMoveCharms();
    }
}

SINGLE_BATTLE_TEST("charms: moves - Sharpness Charm does not boost non-slicing moves", s16 damage)
{
    bool32 hasCharm;

    PARAMETRIZE { hasCharm = FALSE; }
    PARAMETRIZE { hasCharm = TRUE; }

    GIVEN {
        ASSUME(!gBattleMoves[MOVE_TACKLE].slicingMove);
        SetMoveCharms(hasCharm, 0, 0);
        PLAYER(SPECIES_WOBBUFFET) { Attack(120); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        ClearMoveCharms();
    }
}

SINGLE_BATTLE_TEST("charms: moves - Jaw Charm boosts biting moves and stacks with Strong Jaw", s16 damage)
{
    u32 ability;
    bool32 hasCharm;

    PARAMETRIZE { ability = ABILITY_KLUTZ;      hasCharm = FALSE; }
    PARAMETRIZE { ability = ABILITY_STRONG_JAW; hasCharm = FALSE; }
    PARAMETRIZE { ability = ABILITY_KLUTZ;      hasCharm = TRUE; }
    PARAMETRIZE { ability = ABILITY_STRONG_JAW; hasCharm = TRUE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_JAW_LOCK].bitingMove);
        SetMoveCharms(0, hasCharm, 0);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Attack(120); Moves(MOVE_JAW_LOCK); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_JAW_LOCK); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.5), results[1].damage);
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.5), results[2].damage);
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(2.25), results[3].damage);
        ClearMoveCharms();
    }
}

SINGLE_BATTLE_TEST("charms: moves - Jaw Charm does not boost non-biting moves", s16 damage)
{
    bool32 hasCharm;

    PARAMETRIZE { hasCharm = FALSE; }
    PARAMETRIZE { hasCharm = TRUE; }

    GIVEN {
        ASSUME(!gBattleMoves[MOVE_TACKLE].bitingMove);
        SetMoveCharms(0, hasCharm, 0);
        PLAYER(SPECIES_WOBBUFFET) { Attack(120); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        ClearMoveCharms();
    }
}

SINGLE_BATTLE_TEST("charms: moves - Skill Charm makes variable multi-hit moves hit four times half the time")
{
    PASSES_RANDOMLY(50, 100, RNG_LOADED_DICE);

    GIVEN {
        ASSUME(gBattleMoves[MOVE_BULLET_SEED].effect == EFFECT_MULTI_HIT);
        SetMoveCharms(0, 0, 1);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_BULLET_SEED); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_BULLET_SEED); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        MESSAGE("Hit 4 time(s)!");
    } THEN {
        ClearMoveCharms();
    }
}

SINGLE_BATTLE_TEST("charms: moves - Skill Charm makes variable multi-hit moves hit five times half the time")
{
    PASSES_RANDOMLY(50, 100, RNG_LOADED_DICE);

    GIVEN {
        ASSUME(gBattleMoves[MOVE_BULLET_SEED].effect == EFFECT_MULTI_HIT);
        SetMoveCharms(0, 0, 1);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_BULLET_SEED); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_BULLET_SEED); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        MESSAGE("Hit 5 time(s)!");
    } THEN {
        ClearMoveCharms();
    }
}

SINGLE_BATTLE_TEST("charms: moves - Skill Link retains guaranteed five hits with Skill Charm")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_BULLET_SEED].effect == EFFECT_MULTI_HIT);
        SetMoveCharms(0, 0, 1);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SKILL_LINK); Moves(MOVE_BULLET_SEED); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_BULLET_SEED); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        MESSAGE("Hit 5 time(s)!");
    } THEN {
        ClearMoveCharms();
    }
}

SINGLE_BATTLE_TEST("charms: moves - Skill Charm and Loaded Dice retain the same four-or-five roll")
{
    PASSES_RANDOMLY(50, 100, RNG_LOADED_DICE);

    GIVEN {
        ASSUME(gBattleMoves[MOVE_BULLET_SEED].effect == EFFECT_MULTI_HIT);
        ASSUME(gItems[ITEM_LOADED_DICE].holdEffect == HOLD_EFFECT_LOADED_DICE);
        SetMoveCharms(0, 0, 1);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_LOADED_DICE); Moves(MOVE_BULLET_SEED); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_BULLET_SEED); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        MESSAGE("Hit 4 time(s)!");
    } THEN {
        ClearMoveCharms();
    }
}

SINGLE_BATTLE_TEST("charms: moves - Skill Charm leaves fixed-count multi-hit moves unchanged")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_DOUBLE_KICK].strikeCount == 2);
        SetMoveCharms(0, 0, 1);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_DOUBLE_KICK); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_DOUBLE_KICK); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DOUBLE_KICK, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DOUBLE_KICK, player);
        MESSAGE("Hit 2 time(s)!");
    } THEN {
        ClearMoveCharms();
    }
}

SINGLE_BATTLE_TEST("charms: moves - player charms do not boost opponent slicing attacks", s16 damage)
{
    bool32 hasCharm;

    PARAMETRIZE { hasCharm = FALSE; }
    PARAMETRIZE { hasCharm = TRUE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_AERIAL_ACE].slicingMove);
        SetMoveCharms(hasCharm, 0, 0);
        PLAYER(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(120); Moves(MOVE_AERIAL_ACE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_AERIAL_ACE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        ClearMoveCharms();
    }
}

SINGLE_BATTLE_TEST("charms: moves - player Skill Charm does not affect opponent multi-hit attacks")
{
    PASSES_RANDOMLY(35, 100, RNG_HITS);

    GIVEN {
        ASSUME(B_MULTI_HIT_CHANCE >= GEN_5);
        ASSUME(gBattleMoves[MOVE_BULLET_SEED].effect == EFFECT_MULTI_HIT);
        SetMoveCharms(0, 0, 1);
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_BULLET_SEED); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_BULLET_SEED); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, opponent);
        MESSAGE("Hit 2 time(s)!");
    } THEN {
        ClearMoveCharms();
    }
}

SINGLE_BATTLE_TEST("charms: moves - duplicate copies do not increase charm values")
{
    u16 charmCount;

    PARAMETRIZE { charmCount = 1; }
    PARAMETRIZE { charmCount = 2; }

    GIVEN {
        SetMoveCharms(charmCount, charmCount, charmCount);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetCharmValue(EFFECT_SHARPNESS_DAMAGE), 1);
        EXPECT_EQ(GetCharmValue(EFFECT_STRONG_JAW_DAMAGE), 1);
        EXPECT_EQ(GetCharmValue(EFFECT_SKILL_MULTI_HIT), 1);
    } FINALLY {
        ClearMoveCharms();
    }
}
