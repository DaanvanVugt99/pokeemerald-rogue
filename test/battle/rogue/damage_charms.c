#include "global.h"
#include "test/battle.h"

#include "charm_test.h"

static void ClearDamageCharms(void)
{
    ClearCharmTestState();
}

static void SetDamageCharms(u16 techCount, u16 tintedCount, u16 ironFistCount)
{
    BeginCharmTestRun();
    AddCharmForTest(ITEM_TECHNICIAN_CHARM, techCount);
    AddCharmForTest(ITEM_TINTED_CHARM, tintedCount);
    AddCharmForTest(ITEM_IRON_FIST_CHARM, ironFistCount);
    FinishCharmTestSetup();
}

SINGLE_BATTLE_TEST("Damage charms: Tech Charm boosts moves with 60 base power by 1.5x", s16 damage)
{
    bool32 hasCharm;

    PARAMETRIZE { hasCharm = FALSE; }
    PARAMETRIZE { hasCharm = TRUE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_SWIFT].power == 60);
        SetDamageCharms(hasCharm, 0, 0);
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(120); Moves(MOVE_SWIFT); }
        OPPONENT(SPECIES_WOBBUFFET) { SpDefense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWIFT); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.5), results[1].damage);
        ClearDamageCharms();
    }
}

SINGLE_BATTLE_TEST("Damage charms: Tech Charm does not boost moves above 60 base power", s16 damage)
{
    bool32 hasCharm;

    PARAMETRIZE { hasCharm = FALSE; }
    PARAMETRIZE { hasCharm = TRUE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_DRAGON_BREATH].power == 70);
        SetDamageCharms(hasCharm, 0, 0);
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(120); Moves(MOVE_DRAGON_BREATH); }
        OPPONENT(SPECIES_WOBBUFFET) { SpDefense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_BREATH); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        ClearDamageCharms();
    }
}

SINGLE_BATTLE_TEST("Damage charms: Tech Charm stacks with Technician", s16 damage)
{
    u32 ability;
    bool32 hasCharm;

    PARAMETRIZE { ability = ABILITY_KLUTZ;      hasCharm = FALSE; }
    PARAMETRIZE { ability = ABILITY_TECHNICIAN; hasCharm = FALSE; }
    PARAMETRIZE { ability = ABILITY_KLUTZ;      hasCharm = TRUE; }
    PARAMETRIZE { ability = ABILITY_TECHNICIAN; hasCharm = TRUE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_SWIFT].power == 60);
        SetDamageCharms(hasCharm, 0, 0);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); SpAttack(120); Moves(MOVE_SWIFT); }
        OPPONENT(SPECIES_WOBBUFFET) { SpDefense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWIFT); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.5), results[1].damage);
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.5), results[2].damage);
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(2.25), results[3].damage);
        ClearDamageCharms();
    }
}

SINGLE_BATTLE_TEST("Damage charms: Iron Fist Charm boosts punching moves and stacks with Iron Fist", s16 damage)
{
    u32 ability;
    bool32 hasCharm;

    PARAMETRIZE { ability = ABILITY_KLUTZ;     hasCharm = FALSE; }
    PARAMETRIZE { ability = ABILITY_IRON_FIST; hasCharm = FALSE; }
    PARAMETRIZE { ability = ABILITY_KLUTZ;     hasCharm = TRUE; }
    PARAMETRIZE { ability = ABILITY_IRON_FIST; hasCharm = TRUE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_MEGA_PUNCH].punchingMove);
        SetDamageCharms(0, 0, hasCharm);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Attack(120); Moves(MOVE_MEGA_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_MEGA_PUNCH); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.3), results[1].damage);
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.5), results[2].damage);
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.95), results[3].damage);
        ClearDamageCharms();
    }
}

SINGLE_BATTLE_TEST("Damage charms: Iron Fist Charm does not boost non-punching moves", s16 damage)
{
    bool32 hasCharm;

    PARAMETRIZE { hasCharm = FALSE; }
    PARAMETRIZE { hasCharm = TRUE; }

    GIVEN {
        ASSUME(!gBattleMoves[MOVE_TACKLE].punchingMove);
        SetDamageCharms(0, 0, hasCharm);
        PLAYER(SPECIES_WOBBUFFET) { Attack(120); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        ClearDamageCharms();
    }
}

SINGLE_BATTLE_TEST("Damage charms: Tech and Iron Fist Charms stack on low-power punching moves", s16 damage)
{
    bool32 hasCharms;

    PARAMETRIZE { hasCharms = FALSE; }
    PARAMETRIZE { hasCharms = TRUE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_MACH_PUNCH].power <= 60);
        ASSUME(gBattleMoves[MOVE_MACH_PUNCH].punchingMove);
        SetDamageCharms(hasCharms, 0, hasCharms);
        PLAYER(SPECIES_WOBBUFFET) { Attack(120); Moves(MOVE_MACH_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_MACH_PUNCH); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(2.25), results[1].damage);
        ClearDamageCharms();
    }
}

SINGLE_BATTLE_TEST("Damage charms: Tinted Charm normalizes single and double resistances", s16 damage)
{
    u32 targetSpecies;
    bool32 hasCharm;

    PARAMETRIZE { targetSpecies = SPECIES_WOBBUFFET; hasCharm = FALSE; }
    PARAMETRIZE { targetSpecies = SPECIES_SQUIRTLE;  hasCharm = FALSE; }
    PARAMETRIZE { targetSpecies = SPECIES_SQUIRTLE;  hasCharm = TRUE; }
    PARAMETRIZE { targetSpecies = SPECIES_OMANYTE;   hasCharm = FALSE; }
    PARAMETRIZE { targetSpecies = SPECIES_OMANYTE;   hasCharm = TRUE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
        ASSUME(gSpeciesInfo[SPECIES_WOBBUFFET].types[0] == TYPE_PSYCHIC);
        ASSUME(gSpeciesInfo[SPECIES_SQUIRTLE].types[0] == TYPE_WATER);
        ASSUME(gSpeciesInfo[SPECIES_OMANYTE].types[0] == TYPE_ROCK);
        ASSUME(gSpeciesInfo[SPECIES_OMANYTE].types[1] == TYPE_WATER);
        SetDamageCharms(0, hasCharm, 0);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_KLUTZ); SpAttack(120); Moves(MOVE_EMBER); }
        OPPONENT(targetSpecies) { Ability(ABILITY_KLUTZ); SpDefense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.5), results[1].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.25), results[3].damage);
        EXPECT_EQ(results[0].damage, results[4].damage);
        ClearDamageCharms();
    }
}

SINGLE_BATTLE_TEST("Damage charms: Tinted Charm stacks with Tinted Lens and keeps the resisted message", s16 damage)
{
    u32 ability;
    bool32 hasCharm;

    PARAMETRIZE { ability = ABILITY_KLUTZ;       hasCharm = FALSE; }
    PARAMETRIZE { ability = ABILITY_KLUTZ;       hasCharm = TRUE; }
    PARAMETRIZE { ability = ABILITY_TINTED_LENS; hasCharm = FALSE; }
    PARAMETRIZE { ability = ABILITY_TINTED_LENS; hasCharm = TRUE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
        ASSUME(gSpeciesInfo[SPECIES_SQUIRTLE].types[0] == TYPE_WATER);
        SetDamageCharms(0, hasCharm, 0);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); SpAttack(120); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_SQUIRTLE) { Ability(ABILITY_KLUTZ); SpDefense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
        MESSAGE("It's not very effective…");
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(2.0), results[1].damage);
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(2.0), results[2].damage);
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(4.0), results[3].damage);
        ClearDamageCharms();
    }
}

SINGLE_BATTLE_TEST("Damage charms: Tinted Charm does not affect immunities")
{
    bool32 hasCharm;

    PARAMETRIZE { hasCharm = FALSE; }
    PARAMETRIZE { hasCharm = TRUE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
        ASSUME(gSpeciesInfo[SPECIES_GASTLY].types[0] == TYPE_GHOST || gSpeciesInfo[SPECIES_GASTLY].types[1] == TYPE_GHOST);
        SetDamageCharms(0, hasCharm, 0);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_GASTLY) { Ability(ABILITY_KLUTZ); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        MESSAGE("It doesn't affect Foe Gastly…");
        NOT HP_BAR(opponent);
    } FINALLY {
        ClearDamageCharms();
    }
}

SINGLE_BATTLE_TEST("Damage charms: player charms do not boost opponent attacks", s16 damage)
{
    bool32 hasCharm;

    PARAMETRIZE { hasCharm = FALSE; }
    PARAMETRIZE { hasCharm = TRUE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_SWIFT].power == 60);
        SetDamageCharms(hasCharm, 0, 0);
        PLAYER(SPECIES_WOBBUFFET) { SpDefense(120); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { SpAttack(120); Moves(MOVE_SWIFT); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SWIFT); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        ClearDamageCharms();
    }
}

SINGLE_BATTLE_TEST("Damage charms: duplicate copies do not increase their effect", s16 damage)
{
    u16 charmCount;

    PARAMETRIZE { charmCount = 1; }
    PARAMETRIZE { charmCount = 2; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_SWIFT].power == 60);
        SetDamageCharms(charmCount, 0, 0);
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(120); Moves(MOVE_SWIFT); }
        OPPONENT(SPECIES_WOBBUFFET) { SpDefense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWIFT); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } THEN {
        EXPECT_EQ(GetCharmValue(EFFECT_TECHNICIAN_DAMAGE), 1);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        ClearDamageCharms();
    }
}

SINGLE_BATTLE_TEST("Damage charms: Conversion Charm changes Normal moves to the user's primary type")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
        ASSUME(gSpeciesInfo[SPECIES_CHARMANDER].types[0] == TYPE_FIRE);
        ASSUME(gSpeciesInfo[SPECIES_TANGELA].types[0] == TYPE_GRASS);
        SetSingleCharmForTest(ITEM_CONVERSION_CHARM, 1);
        PLAYER(SPECIES_CHARMANDER) { Ability(ABILITY_KLUTZ); UniqueAbility(ABILITY_NONE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_TANGELA) { Ability(ABILITY_KLUTZ); UniqueAbility(ABILITY_NONE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent);
        MESSAGE("It's super effective!");
    } THEN {
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("Damage charms: Conversion Charm follows the user's current primary type")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_SOAK].effect == EFFECT_SOAK);
        ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
        ASSUME(gSpeciesInfo[SPECIES_CHARMANDER].types[0] == TYPE_FIRE);
        SetSingleCharmForTest(ITEM_CONVERSION_CHARM, 1);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
        OPPONENT(SPECIES_CHARMANDER) { Ability(ABILITY_KLUTZ); UniqueAbility(ABILITY_NONE); Moves(MOVE_SOAK, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SOAK); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SOAK, opponent);
        HP_BAR(opponent);
        MESSAGE("It's super effective!");
    } THEN {
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("Damage charms: Conversion Charm leaves non-Normal moves unchanged")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
        ASSUME(gSpeciesInfo[SPECIES_SQUIRTLE].types[0] == TYPE_WATER);
        SetSingleCharmForTest(ITEM_CONVERSION_CHARM, 1);
        PLAYER(SPECIES_CHARMANDER) { Ability(ABILITY_KLUTZ); UniqueAbility(ABILITY_NONE); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_SQUIRTLE) { Ability(ABILITY_KLUTZ); UniqueAbility(ABILITY_NONE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); }
    } SCENE {
        HP_BAR(opponent);
        MESSAGE("It's not very effective…");
    } THEN {
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("Damage charms: Conversion Charm does not convert opponent moves")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
        SetSingleCharmForTest(ITEM_CONVERSION_CHARM, 1);
        PLAYER(SPECIES_GASTLY) { Ability(ABILITY_KLUTZ); UniqueAbility(ABILITY_NONE); }
        OPPONENT(SPECIES_CHARMANDER) { Ability(ABILITY_KLUTZ); UniqueAbility(ABILITY_NONE); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        MESSAGE("It doesn't affect Gastly…");
        NOT HP_BAR(player);
    } THEN {
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("Damage charms: duplicate Conversion Charms clamp to one")
{
    GIVEN {
        SetSingleCharmForTest(ITEM_CONVERSION_CHARM, 2);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetCharmValue(EFFECT_CONVERSION_TYPE), 1);
        ClearCharmTestState();
    }
}
