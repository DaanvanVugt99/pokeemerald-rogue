#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Insectivore uses Stockpile after resisted Fire Lash and Bug Bite up to the normal cap")
{
    GIVEN {
        PLAYER(SPECIES_HEATMOR) { Attack(60); Ability(ABILITY_GLUTTONY); UniqueAbility(ABILITY_INSECTIVORE); Moves(MOVE_FIRE_LASH, MOVE_BUG_BITE); }
        OPPONENT(SPECIES_COMBUSKEN) { HP(1000); MaxHP(1000); Defense(200); Ability(ABILITY_BLAZE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FIRE_LASH); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_BUG_BITE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_FIRE_LASH); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_BUG_BITE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FIRE_LASH, player);
        ABILITY_POPUP(player, ABILITY_INSECTIVORE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STOCKPILE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BUG_BITE, player);
        ABILITY_POPUP(player, ABILITY_INSECTIVORE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STOCKPILE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FIRE_LASH, player);
        ABILITY_POPUP(player, ABILITY_INSECTIVORE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STOCKPILE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BUG_BITE, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_INSECTIVORE);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_STOCKPILE, player);
        }
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].stockpileCounter, 3);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 3);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 3);
    }
}

SINGLE_BATTLE_TEST("Insectivore requires a resisted hit from Fire Lash or Bug Bite")
{
    GIVEN {
        PLAYER(SPECIES_HEATMOR) { Attack(60); Speed(100); Ability(ABILITY_GLUTTONY); UniqueAbility(ABILITY_INSECTIVORE); Moves(MOVE_FIRE_LASH, MOVE_BUG_BITE, MOVE_SIGNAL_BEAM); }
        OPPONENT(SPECIES_CLEFAIRY) { HP(1000); MaxHP(1000); Defense(200); Speed(50); Moves(MOVE_CELEBRATE, MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(player, MOVE_FIRE_LASH); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SIGNAL_BEAM, WITH_RNG(RNG_SECONDARY_EFFECT, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_BUG_BITE, hit: FALSE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_INSECTIVORE);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_STOCKPILE, player);
        }
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].stockpileCounter, 0);
    }
}

SINGLE_BATTLE_TEST("Insectivore lets Bug moves hit and grants no healing on Bug-type KOs")
{
    GIVEN {
        PLAYER(SPECIES_HEATMOR) { HP(200); MaxHP(400); Speed(50); Ability(ABILITY_GLUTTONY); UniqueAbility(ABILITY_INSECTIVORE); Moves(MOVE_FIRE_LASH); }
        OPPONENT(SPECIES_WURMPLE) { HP(1); Speed(100); Moves(MOVE_BUG_BITE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(30); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_BUG_BITE); MOVE(player, MOVE_FIRE_LASH); SEND_OUT(opponent, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BUG_BITE, opponent);
        HP_BAR(player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FIRE_LASH, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_INSECTIVORE);
            HP_BAR(player);
        }
    }
}
