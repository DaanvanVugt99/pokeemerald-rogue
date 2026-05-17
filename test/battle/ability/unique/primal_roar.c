#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BULK_UP].effect == EFFECT_BULK_UP);
}

SINGLE_BATTLE_TEST("Primal Roar uses Bulk Up after Koraidon knocks out a target if the party shares a type")
{
    bool32 partySharesType;

    PARAMETRIZE { partySharesType = TRUE; }
    PARAMETRIZE { partySharesType = FALSE; }

    GIVEN {
        PLAYER(SPECIES_KORAIDON) { Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_DEINO) { Speed(50); Moves(MOVE_CELEBRATE); }
        if (partySharesType)
            PLAYER(SPECIES_SCRAFTY) { Speed(50); Moves(MOVE_CELEBRATE); }
        else
            PLAYER(SPECIES_PIKACHU) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CATERPIE) { HP(1); MaxHP(1); Speed(50); Ability(ABILITY_SHIELD_DUST); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); SEND_OUT(opponent, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
        if (partySharesType)
        {
            ABILITY_POPUP(player, ABILITY_PRIMAL_ROAR);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_BULK_UP, player);
        }
        else
        {
            NONE_OF {
                ABILITY_POPUP(player, ABILITY_PRIMAL_ROAR);
                ANIMATION(ANIM_TYPE_MOVE, MOVE_BULK_UP, player);
            }
        }
    } THEN {
        if (partySharesType)
        {
            EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        }
        else
        {
            EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
            EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        }
    }
}

SINGLE_BATTLE_TEST("Primal Roar does not trigger if Koraidon does not knock out the target")
{
    GIVEN {
        PLAYER(SPECIES_KORAIDON) { Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_DEINO) { Speed(50); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_SCRAFTY) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_PRIMAL_ROAR);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_BULK_UP, player);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}
