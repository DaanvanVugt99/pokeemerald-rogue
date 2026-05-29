#include "global.h"
#include "event_data.h"
#include "test/battle.h"
#include "constants/vars.h"

#define PLAYER_LEFT_BATTLER (&gBattleMons[0])
#define OPPONENT_LEFT_BATTLER (&gBattleMons[1])

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_AERIAL_ACE].type == TYPE_FLYING);
    ASSUME(gBattleMoves[MOVE_ACROBATICS].type == TYPE_FLYING);
    ASSUME(gBattleMoves[MOVE_PECK].type == TYPE_FLYING);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_FLYING);
    ASSUME(gBattleMoves[MOVE_FLAME_CHARGE].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_FLAME_CHARGE].power == 50);
}

SINGLE_BATTLE_TEST("Windsurge uses Flame Charge after the first Flying move each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_TALONFLAME) { Ability(ABILITY_FLAME_BODY); UniqueAbility(ABILITY_WINDSURGE); Moves(MOVE_PECK); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_PECK); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PECK, player);
        HP_BAR(opponent);
        ABILITY_POPUP(PLAYER_LEFT_BATTLER, ABILITY_WINDSURGE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLAME_CHARGE, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Windsurge belongs to the actual attacker after hitting a target with Windsurge")
{
    GIVEN {
        PLAYER(SPECIES_CHARIZARD) { Speed(1); HP(1000); MaxHP(1000); Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_WINDSURGE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_TALONFLAME) { Speed(100); Ability(ABILITY_FLAME_BODY); UniqueAbility(ABILITY_WINDSURGE); Moves(MOVE_ACROBATICS); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_ACROBATICS); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ACROBATICS, opponent);
        ABILITY_POPUP(OPPONENT_LEFT_BATTLER, ABILITY_WINDSURGE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLAME_CHARGE, OPPONENT_LEFT_BATTLER);
        NONE_OF {
            ABILITY_POPUP(PLAYER_LEFT_BATTLER, ABILITY_WINDSURGE);
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].uniqueOncePerSwitchInUsed);
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Windsurge does not trigger after non-Flying moves")
{
    GIVEN {
        PLAYER(SPECIES_TALONFLAME) { Ability(ABILITY_FLAME_BODY); UniqueAbility(ABILITY_WINDSURGE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent);
        NONE_OF {
            ABILITY_POPUP(PLAYER_LEFT_BATTLER, ABILITY_WINDSURGE);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_FLAME_CHARGE, player);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Windsurge only triggers once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_TALONFLAME) { Ability(ABILITY_FLAME_BODY); UniqueAbility(ABILITY_WINDSURGE); Moves(MOVE_PECK); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_PECK); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_PECK); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(PLAYER_LEFT_BATTLER, ABILITY_WINDSURGE);
        NONE_OF {
            ABILITY_POPUP(PLAYER_LEFT_BATTLER, ABILITY_WINDSURGE);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Windsurge Flame Charge is 25 BP", s16 damage)
{
    bool32 windsurge;

    PARAMETRIZE { windsurge = FALSE; }
    PARAMETRIZE { windsurge = TRUE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) {
            Ability(ABILITY_SHADOW_TAG);
            if (windsurge)
                UniqueAbility(ABILITY_WINDSURGE);
            Moves(windsurge ? MOVE_PECK : MOVE_FLAME_CHARGE);
        }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, windsurge ? MOVE_PECK : MOVE_FLAME_CHARGE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        if (windsurge) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_PECK, player);
            HP_BAR(opponent);
            ABILITY_POPUP(PLAYER_LEFT_BATTLER, ABILITY_WINDSURGE);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_FLAME_CHARGE, player);
        } else {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_FLAME_CHARGE, player);
        }
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Windsurge clears its temporary move effect when Flame Charge fails")
{
    GIVEN {
        PLAYER(SPECIES_TALONFLAME) { Ability(ABILITY_FLAME_BODY); UniqueAbility(ABILITY_WINDSURGE); Moves(MOVE_AERIAL_ACE); }
        OPPONENT(SPECIES_PONYTA) { Ability(ABILITY_FLASH_FIRE); HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_AERIAL_ACE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AERIAL_ACE, player);
        HP_BAR(opponent);
        ABILITY_POPUP(PLAYER_LEFT_BATTLER, ABILITY_WINDSURGE);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT_EQ(VarGet(VAR_TEMP_MOVEEFECT_CHANCE), 0);
        EXPECT_EQ(VarGet(VAR_TEMP_MOVEEFFECT), 0);
    }
}
