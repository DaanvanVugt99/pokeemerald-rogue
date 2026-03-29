#include "constants/moves.h"
#include "constants/species.h"
#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_MAGICAL_LEAF].type == TYPE_GRASS);
    ASSUME(gBattleMoves[MOVE_MAGICAL_LEAF].secondaryEffectChance == 0);
    ASSUME(gBattleMoves[MOVE_RAIN_DANCE].effect == EFFECT_RAIN_DANCE);
    ASSUME(gBattleMoves[MOVE_GRASSY_TERRAIN].effect == EFFECT_GRASSY_TERRAIN);
}

SINGLE_BATTLE_TEST("Chlorofumes makes Grass-type moves lower the target's Attack")
{
    GIVEN {
        PLAYER(SPECIES_VENUSAUR) { Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_CHLOROFUMES); Moves(MOVE_RAIN_DANCE, MOVE_MAGICAL_LEAF); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_MAGICAL_LEAF); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT(!(opponent->status1 & (STATUS1_POISON | STATUS1_TOXIC_POISON)));
    }
}

SINGLE_BATTLE_TEST("Chlorofumes only poisons on Grass-type move hit in Grassy Terrain")
{
    bool8 inGrassyTerrain;
    u16 setupMove;
    PARAMETRIZE { inGrassyTerrain = FALSE; setupMove = MOVE_PLAIN_TERRAIN; }
    PARAMETRIZE { inGrassyTerrain = TRUE; setupMove = MOVE_GRASSY_TERRAIN; }
    GIVEN {
        PLAYER(SPECIES_VENUSAUR) { Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_CHLOROFUMES); Moves(setupMove, MOVE_MAGICAL_LEAF); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, setupMove); }
        TURN { MOVE(player, MOVE_MAGICAL_LEAF); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        if (inGrassyTerrain)
            EXPECT(opponent->status1 & STATUS1_POISON);
        else
            EXPECT(!(opponent->status1 & (STATUS1_POISON | STATUS1_TOXIC_POISON)));
    }
}

SINGLE_BATTLE_TEST("Chlorofumes can trigger alongside other move-end attacker abilities")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_MAGICAL_LEAF].type == TYPE_GRASS);
        ASSUME(gBattleMoves[MOVE_MAGICAL_LEAF].power > 0);
        PLAYER(SPECIES_VENUSAUR) { Ability(ABILITY_STENCH); UniqueAbility(ABILITY_CHLOROFUMES); Speed(100); Moves(MOVE_MAGICAL_LEAF); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); MaxHP(999); HP(999); Moves(MOVE_AERIAL_ACE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MAGICAL_LEAF, WITH_RNG(RNG_STENCH, TRUE)); MOVE(opponent, MOVE_AERIAL_ACE); }
    } THEN {
        EXPECT_GT(opponent->hp, 0);
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->hp, player->maxHP);
    }
}
