#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Unknown Biology restores HP from poison instead of taking damage")
{
    u16 hp;

    GIVEN {
        PLAYER(SPECIES_DEOXYS_NORMAL) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_UNKNOWN_BIOLOGY); Status1(STATUS1_POISON); HP(1); MaxHP(400); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        hp = player->hp;
        EXPECT_EQ(hp, 51);
    }
}

SINGLE_BATTLE_TEST("Unknown Biology prevents burn damage")
{
    u16 hp;

    GIVEN {
        PLAYER(SPECIES_DEOXYS_NORMAL) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_UNKNOWN_BIOLOGY); Status1(STATUS1_BURN); HP(1); MaxHP(400); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        hp = player->hp;
        EXPECT_EQ(hp, 51);
    }
}

SINGLE_BATTLE_TEST("Unknown Biology burn boosts Special Attack by 1.5x", s16 damage)
{
    bool32 burned;

    PARAMETRIZE { burned = FALSE; }
    PARAMETRIZE { burned = TRUE; }

    GIVEN {
        PLAYER(SPECIES_DEOXYS_NORMAL) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_UNKNOWN_BIOLOGY); Status1(burned ? STATUS1_BURN : STATUS1_NONE); SpAttack(100); Moves(MOVE_PSYBEAM); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(200); }
    } WHEN {
        TURN { MOVE(player, MOVE_PSYBEAM); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Unknown Biology burn does not reduce physical damage", s16 damage)
{
    bool32 burned;

    PARAMETRIZE { burned = FALSE; }
    PARAMETRIZE { burned = TRUE; }

    GIVEN {
        PLAYER(SPECIES_DEOXYS_NORMAL) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_UNKNOWN_BIOLOGY); Status1(burned ? STATUS1_BURN : STATUS1_NONE); Attack(100); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(200); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Unknown Biology still suffers Smolder's physical damage penalty when not burned", s16 damage)
{
    bool32 smolderActive;

    PARAMETRIZE { smolderActive = FALSE; }
    PARAMETRIZE { smolderActive = TRUE; }

    GIVEN {
        PLAYER(SPECIES_DEOXYS_NORMAL) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_UNKNOWN_BIOLOGY); Attack(100); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(smolderActive ? ABILITY_SMOLDER : ABILITY_NONE); HP(200); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, UQ_4_12(2.0), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Unknown Biology paralysis keeps Speed")
{
    GIVEN {
        PLAYER(SPECIES_DEOXYS_NORMAL) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_UNKNOWN_BIOLOGY); Status1(STATUS1_PARALYSIS); Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(80); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

SINGLE_BATTLE_TEST("Unknown Biology paralysis does not prevent moving")
{
    GIVEN {
        PLAYER(SPECIES_DEOXYS_NORMAL) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_UNKNOWN_BIOLOGY); Status1(STATUS1_PARALYSIS); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, WITH_RNG(RNG_PARALYSIS, TRUE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    }
}

SINGLE_BATTLE_TEST("Unknown Biology paralysis boosts Defense", s16 damage)
{
    u32 status;

    PARAMETRIZE { status = STATUS1_NONE; }
    PARAMETRIZE { status = STATUS1_PARALYSIS; }

    GIVEN {
        PLAYER(SPECIES_DEOXYS_NORMAL) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_UNKNOWN_BIOLOGY); Status1(status); Defense(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(120); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(1.5), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Unknown Biology sleep boosts Speed")
{
    GIVEN {
        PLAYER(SPECIES_DEOXYS_NORMAL) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_UNKNOWN_BIOLOGY); Status1(STATUS1_SLEEP_TURN(2)); Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(120); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

SINGLE_BATTLE_TEST("Unknown Biology sleep does not prevent moving")
{
    GIVEN {
        PLAYER(SPECIES_DEOXYS_NORMAL) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_UNKNOWN_BIOLOGY); Status1(STATUS1_SLEEP_TURN(2)); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    }
}

SINGLE_BATTLE_TEST("Unknown Biology frostbite boosts Special Defense", s16 damage)
{
    u32 status;

    PARAMETRIZE { status = STATUS1_NONE; }
    PARAMETRIZE { status = STATUS1_FROSTBITE; }

    GIVEN {
        PLAYER(SPECIES_DEOXYS_NORMAL) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_UNKNOWN_BIOLOGY); Status1(status); SpDefense(100); Moves(MOVE_CELEBRATE); HP(300); }
        OPPONENT(SPECIES_ALAKAZAM) { Moves(MOVE_PSYCHIC); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_PSYCHIC); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(1.5), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Unknown Biology freeze does not prevent moving")
{
    GIVEN {
        PLAYER(SPECIES_DEOXYS_NORMAL) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_UNKNOWN_BIOLOGY); Status1(STATUS1_FREEZE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, WITH_RNG(RNG_FROZEN, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    }
}

SINGLE_BATTLE_TEST("Unknown Biology frostbite does not reduce Special Attack damage", s16 damage)
{
    bool32 frostbitten;

    PARAMETRIZE { frostbitten = FALSE; }
    PARAMETRIZE { frostbitten = TRUE; }

    GIVEN {
        PLAYER(SPECIES_DEOXYS_NORMAL) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_UNKNOWN_BIOLOGY); Status1(frostbitten ? STATUS1_FROSTBITE : STATUS1_NONE); SpAttack(100); Moves(MOVE_PSYBEAM); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(200); }
    } WHEN {
        TURN { MOVE(player, MOVE_PSYBEAM); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}
