#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].power > 0);
}

SINGLE_BATTLE_TEST("Fungal Infection seeds damaged targets and drains at end of turn")
{
    u16 uniqueAbility;
    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_FUNGAL_INFECTION; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); HP(50); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        if (uniqueAbility == ABILITY_FUNGAL_INFECTION)
            ANIMATION(ANIM_TYPE_MOVE, MOVE_LEECH_SEED, player);
        else
            NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_LEECH_SEED, player);
    } THEN {
        if (uniqueAbility == ABILITY_FUNGAL_INFECTION)
            EXPECT_GT(player->hp, 50);
        else
            EXPECT_EQ(player->hp, 50);
    }
}

SINGLE_BATTLE_TEST("Fungal Infection does not affect Grass-type targets")
{
    GIVEN {
        ASSUME(gSpeciesInfo[SPECIES_ODDISH].types[0] == TYPE_GRASS);
        PLAYER(SPECIES_PARAS) { Ability(ABILITY_EFFECT_SPORE); UniqueAbility(ABILITY_FUNGAL_INFECTION); HP(50); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_ODDISH) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->hp, 50);
    }
}
