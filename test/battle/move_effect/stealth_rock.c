#include "global.h"
#include "constants/rogue.h"
#include "pokemon.h"
#include "test/battle.h"

static u32 DynamicTypeCustomMonId(u32 type, u32 typeSlot)
{
    return OTID_FLAG_CUSTOM_MON
        | OTID_FLAG_DYNAMIC_CUSTOM_MON
        | (1 << 28) // COMPRESSED_FORMAT_MON_TYPE
        | (typeSlot << 5)
        | type;
}

static void CreateDynamicTypeMon(struct Pokemon *mon, u16 species, u32 type, u32 typeSlot)
{
    CreateMon(mon, species, 100, 0, TRUE, 0, OT_ID_CUSTOM_MON, DynamicTypeCustomMonId(type, typeSlot));
}

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_STEALTH_ROCK].effect == EFFECT_STEALTH_ROCK);
}

SINGLE_BATTLE_TEST("Stealth Rock damage on switch in based on typing")
{
    u32 species, divisor;

    PARAMETRIZE { species = SPECIES_CHARIZARD; divisor = 2; } // Fire / Flying
    PARAMETRIZE { species = SPECIES_PIDGEOT; divisor = 4; } // Flying
    PARAMETRIZE { species = SPECIES_RATICATE; divisor = 8; } // Normal dmg
    PARAMETRIZE { species = SPECIES_GOLEM; divisor = 16; } // Ground not very effective
    PARAMETRIZE { species = SPECIES_STEELIX; divisor = 32; } // Ground/Steel not very effective

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(species);
    } WHEN {
        TURN { MOVE(player, MOVE_STEALTH_ROCK); }
        TURN { SWITCH(opponent, 1); }
    } SCENE {
        s32 maxHP = GetMonData(&OPPONENT_PARTY[1], MON_DATA_MAX_HP);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STEALTH_ROCK, player);
        // MESSAGE("2 sent out species!");
        HP_BAR(opponent, damage: maxHP / divisor);
    }
}

SINGLE_BATTLE_TEST("Stealth Rock uses dynamic typing for switch ins")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_RATICATE);

        CreateDynamicTypeMon(&OPPONENT_PARTY[1], SPECIES_RATICATE, TYPE_FIRE, 0);
    } WHEN {
        TURN { MOVE(player, MOVE_STEALTH_ROCK); }
        TURN { SWITCH(opponent, 1); }
    } SCENE {
        s32 maxHP = GetMonData(&OPPONENT_PARTY[1], MON_DATA_MAX_HP);
        HP_BAR(opponent, damage: maxHP / 4);
    } THEN {
        EXPECT_EQ(opponent->type1, TYPE_FIRE);
    }
}
