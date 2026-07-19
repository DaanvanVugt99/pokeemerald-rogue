#include "global.h"
#include "test/battle.h"

#include "constants/flags.h"
#include "event_data.h"
#include "item.h"
#include "rogue_charms.h"

static void ClearUtilityCharms(void)
{
    const u16 items[] =
    {
        ITEM_IRON_FIST_CHARM,
        ITEM_REACH_CHARM,
        ITEM_ACCURACY_CHARM,
        ITEM_RECOVERY_CHARM,
    };
    u32 i;

    for (i = 0; i < ARRAY_COUNT(items); i++)
    {
        u16 count = GetItemCountInBag(items[i]);

        if (count != 0)
            RemoveBagItem(items[i], count);
    }

    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    RecalcCharmCurseValues();
}

static void SetUtilityCharms(u16 reachCount, u16 accuracyCount, u16 recoveryCount)
{
    ClearUtilityCharms();
    FlagSet(FLAG_ROGUE_RUN_ACTIVE);

    if (reachCount != 0)
        AddBagItem(ITEM_REACH_CHARM, reachCount);
    if (accuracyCount != 0)
        AddBagItem(ITEM_ACCURACY_CHARM, accuracyCount);
    if (recoveryCount != 0)
        AddBagItem(ITEM_RECOVERY_CHARM, recoveryCount);

    RecalcCharmCurseValues();
}

SINGLE_BATTLE_TEST("Utility charms: Reach Charm boosts inherent contact moves and disables Tough Claws", s16 damage)
{
    u32 ability;
    bool32 hasCharm;

    PARAMETRIZE { ability = ABILITY_KLUTZ;       hasCharm = FALSE; }
    PARAMETRIZE { ability = ABILITY_TOUGH_CLAWS; hasCharm = FALSE; }
    PARAMETRIZE { ability = ABILITY_KLUTZ;       hasCharm = TRUE; }
    PARAMETRIZE { ability = ABILITY_TOUGH_CLAWS; hasCharm = TRUE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
        SetUtilityCharms(hasCharm, 0, 0);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Attack(120); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.3), results[1].damage);
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.25), results[2].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
        ClearUtilityCharms();
    }
}

SINGLE_BATTLE_TEST("Utility charms: Reach Charm leaves non-contact move damage unchanged", s16 damage)
{
    bool32 hasCharm;

    PARAMETRIZE { hasCharm = FALSE; }
    PARAMETRIZE { hasCharm = TRUE; }

    GIVEN {
        ASSUME(!gBattleMoves[MOVE_SWIFT].makesContact);
        SetUtilityCharms(hasCharm, 0, 0);
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(120); Moves(MOVE_SWIFT); }
        OPPONENT(SPECIES_WOBBUFFET) { SpDefense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWIFT); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        ClearUtilityCharms();
    }
}

SINGLE_BATTLE_TEST("Utility charms: Reach Charm prevents contact retaliation")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
        SetUtilityCharms(1, 0, 0);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_PIKACHU) { Ability(ABILITY_STATIC); UniqueAbility(ABILITY_NONE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(opponent, ABILITY_STATIC);
            STATUS_ICON(player, paralysis: TRUE);
        }
    } THEN {
        ClearUtilityCharms();
    }
}

SINGLE_BATTLE_TEST("Utility charms: Reach Charm recognizes physical Shell Side Arm as inherent contact", s16 damage)
{
    bool32 hasCharm;

    PARAMETRIZE { hasCharm = FALSE; }
    PARAMETRIZE { hasCharm = TRUE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_SHELL_SIDE_ARM].effect == EFFECT_SHELL_SIDE_ARM);
        ASSUME(!gBattleMoves[MOVE_SHELL_SIDE_ARM].makesContact);
        SetUtilityCharms(hasCharm, 0, 0);
        PLAYER(SPECIES_WOBBUFFET) { Attack(300); SpAttack(10); Moves(MOVE_SHELL_SIDE_ARM); }
        OPPONENT(SPECIES_PIKACHU) { Ability(ABILITY_STATIC); UniqueAbility(ABILITY_NONE); Defense(100); SpDefense(100); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHELL_SIDE_ARM); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
        if (hasCharm)
        {
            NONE_OF {
                ABILITY_POPUP(opponent, ABILITY_STATIC);
                STATUS_ICON(player, paralysis: TRUE);
            }
        }
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.25), results[1].damage);
        ClearUtilityCharms();
    }
}

SINGLE_BATTLE_TEST("Utility charms: Reach and Iron Fist Charms stack on punching contact moves", s16 damage)
{
    bool32 hasCharms;

    PARAMETRIZE { hasCharms = FALSE; }
    PARAMETRIZE { hasCharms = TRUE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_MEGA_PUNCH].makesContact);
        ASSUME(gBattleMoves[MOVE_MEGA_PUNCH].punchingMove);
        SetUtilityCharms(hasCharms, 0, 0);
        if (hasCharms)
        {
            AddBagItem(ITEM_IRON_FIST_CHARM, 1);
            RecalcCharmCurseValues();
        }
        PLAYER(SPECIES_WOBBUFFET) { Attack(120); Moves(MOVE_MEGA_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_MEGA_PUNCH); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.875), results[1].damage);
        ClearUtilityCharms();
    }
}

SINGLE_BATTLE_TEST("Utility charms: Reach Charm does not boost opponent contact moves", s16 damage)
{
    bool32 hasCharm;

    PARAMETRIZE { hasCharm = FALSE; }
    PARAMETRIZE { hasCharm = TRUE; }

    GIVEN {
        SetUtilityCharms(hasCharm, 0, 0);
        PLAYER(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(120); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        ClearUtilityCharms();
    }
}

SINGLE_BATTLE_TEST("Utility charms: Accuracy Charm makes damaging moves bypass accuracy rolls")
{
    GIVEN {
        SetUtilityCharms(0, 1, 0);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_THUNDER); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER, WITH_RNG(RNG_ACCURACY, FALSE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDER, player);
        HP_BAR(opponent);
    } THEN {
        ClearUtilityCharms();
    }
}

SINGLE_BATTLE_TEST("Utility charms: Accuracy Charm makes status moves bypass accuracy rolls")
{
    GIVEN {
        SetUtilityCharms(0, 1, 0);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_HYPNOSIS); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_HYPNOSIS, WITH_RNG(RNG_ACCURACY, FALSE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPNOSIS, player);
        STATUS_ICON(opponent, sleep: TRUE);
    } THEN {
        ClearUtilityCharms();
    }
}

SINGLE_BATTLE_TEST("Utility charms: Accuracy Charm makes variable multi-hit moves pass their accuracy check")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_BULLET_SEED].effect == EFFECT_MULTI_HIT);
        SetUtilityCharms(0, 1, 0);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_BULLET_SEED); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_BULLET_SEED, WITH_RNG(RNG_ACCURACY, FALSE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        HP_BAR(opponent);
    } THEN {
        ClearUtilityCharms();
    }
}

SINGLE_BATTLE_TEST("Utility charms: Accuracy Charm cannot hit semi-invulnerable targets")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_FLY].effect == EFFECT_SEMI_INVULNERABLE);
        SetUtilityCharms(0, 1, 0);
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_FLY); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_FLY); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        MESSAGE("Wobbuffet's attack missed!");
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
    } THEN {
        ClearUtilityCharms();
    }
}

SINGLE_BATTLE_TEST("Utility charms: Accuracy Charm does not affect OHKO accuracy")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_FISSURE].effect == EFFECT_OHKO);
        SetUtilityCharms(0, 1, 0);
        PLAYER(SPECIES_WOBBUFFET) { Level(50); Moves(MOVE_FISSURE); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); }
    } WHEN {
        TURN { MOVE(player, MOVE_FISSURE, WITH_RNG(RNG_ACCURACY, FALSE)); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_FISSURE, player);
        MESSAGE("Wobbuffet's attack missed!");
    } THEN {
        ClearUtilityCharms();
    }
}

SINGLE_BATTLE_TEST("Utility charms: Accuracy Charm does not affect opponent moves")
{
    GIVEN {
        SetUtilityCharms(0, 1, 0);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_THUNDER); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_THUNDER, WITH_RNG(RNG_ACCURACY, FALSE)); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDER, opponent);
        MESSAGE("Foe Wobbuffet's attack missed!");
    } THEN {
        ClearUtilityCharms();
    }
}

SINGLE_BATTLE_TEST("Utility charms: Recovery Charm boosts direct healing", s16 healing)
{
    bool32 hasCharm;

    PARAMETRIZE { hasCharm = FALSE; }
    PARAMETRIZE { hasCharm = TRUE; }

    GIVEN {
        SetUtilityCharms(0, 0, hasCharm);
        PLAYER(SPECIES_WOBBUFFET) { HP(1); MaxHP(400); Moves(MOVE_RECOVER); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_RECOVER); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].healing);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].healing, UQ_4_12(1.5), results[1].healing);
        ClearUtilityCharms();
    }
}

SINGLE_BATTLE_TEST("Utility charms: Recovery Charm stacks with Big Root on draining moves", s16 damage, s16 healing)
{
    bool32 hasCharm;
    u32 item;

    PARAMETRIZE { hasCharm = FALSE; item = ITEM_NONE; }
    PARAMETRIZE { hasCharm = TRUE;  item = ITEM_NONE; }
    PARAMETRIZE { hasCharm = FALSE; item = ITEM_BIG_ROOT; }
    PARAMETRIZE { hasCharm = TRUE;  item = ITEM_BIG_ROOT; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_ABSORB].effect == EFFECT_ABSORB);
        SetUtilityCharms(0, 0, hasCharm);
        PLAYER(SPECIES_WOBBUFFET) { HP(1); Item(item); Moves(MOVE_ABSORB); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_ABSORB); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
        HP_BAR(player, captureDamage: &results[i].healing);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(-0.5), results[0].healing);
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(-0.75), results[1].healing);
        EXPECT_MUL_EQ(results[2].damage, Q_4_12(-0.65), results[2].healing);
        EXPECT_MUL_EQ(results[3].damage, Q_4_12(-0.975), results[3].healing);
        ClearUtilityCharms();
    }
}

DOUBLE_BATTLE_TEST("Utility charms: Recovery Charm boosts ally healing including Pollen Puff")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_POLLEN_PUFF].effect == EFFECT_HIT_ENEMY_HEAL_ALLY);
        SetUtilityCharms(0, 0, 1);
        PLAYER(SPECIES_WOBBUFFET) { HP(1); MaxHP(400); }
        PLAYER(SPECIES_WYNAUT) { Moves(MOVE_POLLEN_PUFF); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(playerRight, MOVE_POLLEN_PUFF, target: playerLeft); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_POLLEN_PUFF, playerRight);
        HP_BAR(playerLeft, damage: -300);
    } THEN {
        ClearUtilityCharms();
    }
}

SINGLE_BATTLE_TEST("Utility charms: Recovery Charm does not boost Pollen Puff damage", s16 damage)
{
    bool32 hasCharm;

    PARAMETRIZE { hasCharm = FALSE; }
    PARAMETRIZE { hasCharm = TRUE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_POLLEN_PUFF].effect == EFFECT_HIT_ENEMY_HEAL_ALLY);
        SetUtilityCharms(0, 0, hasCharm);
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(120); Moves(MOVE_POLLEN_PUFF); }
        OPPONENT(SPECIES_WOBBUFFET) { SpDefense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_POLLEN_PUFF); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        ClearUtilityCharms();
    }
}

SINGLE_BATTLE_TEST("Utility charms: Recovery Charm boosts delayed Wish healing")
{
    GIVEN {
        SetUtilityCharms(0, 0, 1);
        PLAYER(SPECIES_WOBBUFFET) { HP(1); MaxHP(400); Moves(MOVE_WISH, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WISH); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(player, damage: -300);
    } THEN {
        ClearUtilityCharms();
    }
}

SINGLE_BATTLE_TEST("Utility charms: Recovery Charm revives with three quarters HP")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_REVIVAL_BLESSING].effect == EFFECT_REVIVAL_BLESSING);
        SetUtilityCharms(0, 0, 1);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_REVIVAL_BLESSING); }
        PLAYER(SPECIES_WYNAUT) { HP(0); MaxHP(400); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_REVIVAL_BLESSING); SEND_OUT(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->hp, 300);
        ClearUtilityCharms();
    }
}

SINGLE_BATTLE_TEST("Utility charms: Recovery Charm does not boost Aqua Ring healing")
{
    GIVEN {
        SetUtilityCharms(0, 0, 1);
        PLAYER(SPECIES_WOBBUFFET) { HP(100); MaxHP(400); Moves(MOVE_AQUA_RING); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_AQUA_RING); }
    } SCENE {
        HP_BAR(player, damage: -25);
    } THEN {
        ClearUtilityCharms();
    }
}

SINGLE_BATTLE_TEST("Utility charms: Recovery Charm does not boost opponent healing")
{
    GIVEN {
        SetUtilityCharms(0, 0, 1);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); MaxHP(400); Moves(MOVE_RECOVER); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_RECOVER); }
    } SCENE {
        HP_BAR(opponent, damage: -200);
    } THEN {
        ClearUtilityCharms();
    }
}

SINGLE_BATTLE_TEST("Utility charms: duplicate copies do not increase charm values")
{
    u16 charmCount;

    PARAMETRIZE { charmCount = 1; }
    PARAMETRIZE { charmCount = 2; }

    GIVEN {
        SetUtilityCharms(charmCount, charmCount, charmCount);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetCharmValue(EFFECT_REACH_DAMAGE), 1);
        EXPECT_EQ(GetCharmValue(EFFECT_ACCURACY), 1);
        EXPECT_EQ(GetCharmValue(EFFECT_RECOVERY), 1);
    } FINALLY {
        ClearUtilityCharms();
    }
}
