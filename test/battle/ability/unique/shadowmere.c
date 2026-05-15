#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gSpeciesInfo[SPECIES_SPECTRIER].types[0] == TYPE_GHOST || gSpeciesInfo[SPECIES_SPECTRIER].types[1] == TYPE_GHOST);
    ASSUME(gSpeciesInfo[SPECIES_GASTLY].types[0] == TYPE_GHOST || gSpeciesInfo[SPECIES_GASTLY].types[1] == TYPE_GHOST);
    ASSUME(gSpeciesInfo[SPECIES_CHARMANDER].types[0] != TYPE_GHOST && gSpeciesInfo[SPECIES_CHARMANDER].types[1] != TYPE_GHOST);
    ASSUME(gBattleMoves[MOVE_SHADOW_BALL].type == TYPE_GHOST);
    ASSUME(gBattleMoves[MOVE_SHADOW_BALL].power > 0);
}

SINGLE_BATTLE_TEST("Shadowmere makes Spectrier immune to Ghost moves when its party shares a type")
{
    bool32 sharedParty;

    PARAMETRIZE { sharedParty = FALSE; }
    PARAMETRIZE { sharedParty = TRUE; }

    GIVEN {
        PLAYER(SPECIES_SPECTRIER) { HP(500); MaxHP(500); Speed(50); Ability(ABILITY_GRIM_NEIGH); Moves(MOVE_CELEBRATE); }
        PLAYER(sharedParty ? SPECIES_GASTLY : SPECIES_CHARMANDER) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); SpAttack(200); Moves(MOVE_SHADOW_BALL); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SHADOW_BALL); }
    } SCENE {
        if (sharedParty)
        {
            ABILITY_POPUP(player, ABILITY_SHADOWMERE);
            NONE_OF {
                HP_BAR(player);
            }
        }
        else
        {
            HP_BAR(player);
        }
    } THEN {
        if (sharedParty)
            EXPECT_EQ(player->hp, player->maxHP);
        else
            EXPECT_LT(player->hp, player->maxHP);
    }
}
